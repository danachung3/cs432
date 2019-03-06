#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <ucontext.h>
#include "interrupt.h"
#include "thread.h"
using namespace std;

typedef void (*thread_startfunc_t) (void *);

struct lockStruct {
  unsigned int id;
  bool busy;
  vector<ucontext_t*> blocked;
  ucontext_t* currentOwner;
};



//const int STACK_SIZE = 100;

vector<ucontext_t*> readyQueue;
vector<lockStruct> locks;
//Map
ucontext_t* current;
ucontext_t* previous;

ucontext_t* pop() {
  ucontext_t* popped = readyQueue[0];
  readyQueue.erase(readyQueue.begin());
  return popped;
}


int findLock(unsigned int id) {
  for(int i = 0; i < locks.size(); i++) {
    if(locks[i].id == id) {
      return i;
    }
  }
  return -1;
}

int start_thread (thread_startfunc_t func, void *arg) {
  interrupt_enable();
  (*func)(arg);
  interrupt_disable();

  //Cleanup, free previously used thread
  if(previous != NULL) {
    free(previous->uc_stack.ss_sp);
    free(previous);
  }
  previous = current;
  if(readyQueue.empty()) {
    cout << "Thread library exiting.\n";
    exit(0);
  }
  current = pop();

  swapcontext(previous, current);
  return 0;
}


int thread_libinit (thread_startfunc_t func, void *arg) {
  current = new ucontext_t;
  getcontext(current);
  
  char *stack = new char [STACK_SIZE];
  current->uc_stack.ss_sp = stack;
  current->uc_stack.ss_size = STACK_SIZE;
  current->uc_stack.ss_flags = 0;
  current->uc_link = NULL;
  
  interrupt_disable();
  makecontext(current, (void (*) ()) start_thread, 2, func, arg);
  setcontext(current);
}


int thread_create (thread_startfunc_t func, void *arg) {
  ucontext_t* newThread = new ucontext_t;
  getcontext(newThread);

  char *stack = new char [STACK_SIZE];
  newThread->uc_stack.ss_sp = stack;
  newThread->uc_stack.ss_size = STACK_SIZE;
  newThread->uc_stack.ss_flags = 0;
  newThread->uc_link = NULL;

  interrupt_disable();
  makecontext(newThread, (void(*)()) start_thread, 2, func, arg);
  auto iter = readyQueue.insert(readyQueue.end(), newThread);
  //If there are no other running threads, run?
  
  interrupt_enable();
  return 0;
}



int thread_lock (unsigned int lock) {
  interrupt_disable();
  //If lock doesnt exist, create it
  int index = findLock(lock);
  if(index < 0) {
    //create lock and add to vector
    vector<ucontext_t*> newBlock; 
    struct lockStruct newLock = {lock, true, newBlock, current};
    auto iter = locks.insert(locks.end(), newLock);
    index = locks.size()-1;
  }
  //We are so sorry for our variable names
  lockStruct ourLock = locks[index];
  if(!ourLock.busy) {
    ourLock.busy = true;
    auto iter = readyQueue.insert(readyQueue.end(), current); 
  } else {
    //What thread do we add? Not currentThread right?
    auto iter = ourLock.blocked.insert(ourLock.blocked.end(), current);
  }

  ucontext* next = pop();
  if(next == NULL) {
    exit(0);
  }
  else {
    ucontext* prev = current;
    current = next;
    swapcontext(prev, current);
  }
  
  interrupt_enable();
  return 1;
}

int thread_unlock (unsigned int lock) {
  interrupt_disable();
  int index = findLock(lock);

  if (index < 0) {
    return -1;
  }
  lockStruct ourLock = locks[index];
  if(ourLock.busy) {
    ourLock.busy = false;
    if (!ourLock.blocked.empty()) {
      ucontext* popped = ourLock.blocked[0];
      ourLock.blocked.erase(ourLock.blocked.begin());
      ucontext* unblock = popped;
      ourLock.currentOwner = unblock;
      ourLock.busy = true; 
      auto iter = readyQueue.insert(readyQueue.end(), unblock); 
    }
  } 
}

/*
int thread_wait (unsigned int lock, unsigned int cond) {return 1;}
int thread_signal (unsigned int lock, unsigned int cond) {return 1;}
int thread_broadcast (unsigned int lock, unsigned int cond) {return 1;}
*/


int thread_yield(void) {
  //Check that there is another thread to run

  interrupt_disable();
  ucontext* next = pop();
  if(next == NULL) {
    setcontext(current);
  }
  else {
    ucontext* prev = current;
    current = next;
    auto iter = readyQueue.insert(readyQueue.end(), prev);
    swapcontext(prev, current);
  }
  interrupt_enable();
  
  return 0;
}
