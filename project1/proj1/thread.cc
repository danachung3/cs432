#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include "ucontext.h"
#include "interrupt.h"
using namespace std;

typedef void (*thread_startfunc_t) (void *);

struct lockStruct {
  int id;
  bool busy;
  vector<ucontext_t*> blocked;
  ucontext_t* currentOwner;
} ;

const int STACK_SIZE = 100;

vector<ucontext_t*> readyQueue;
vector<lockStruct> locks;
ucontext_t* current;
ucontext_t* previous;

ucontext_t* pop() {
  ucontext_t* popped = readyQueue[0];
  readyQueue.erase(readyQueue.begin());
  return popped;
}

int findLock(int id) {
  for(int i = 0; i < locks.size(); i++) {
    if(locks[i].id == id) {
      return i;
    }
  }
  return -1;
}


int thread_libinit (thread_startfunc_t func, void *arg) {
  current = new ucontext_t;
  getcontext(current);
  
  char *stack = new char [STACK_SIZE];
  current->uc_stack.ss_sp = stack;
  current->uc_stack.ss_size = STACK_SIZE;
  current->uc_stack.ss_flags = 0;
  current->uc_link = NULL;
  
  
  makecontext(current, (void (*) ()) func, 1, arg);
  setcontext(current);
}

int start_thread (thread_startfunc_t func, void *arg) {
  interrupt_enable();
  (*func)(arg);
  interrupt_disable();
  free(previous->uc_stack.ss_sp);
  free(previous);
  previous = current;
  swapcontext(current, pop());
  return 1;
}


int thread_create (thread_startfunc_t func, void *arg) {
  ucontext_t* newThread = new ucontext_t;
  getcontext(newThread);

  char *stack = new char [STACK_SIZE];
  newThread->uc_stack.ss_sp = stack;
  newThread->uc_stack.ss_size = STACK_SIZE;
  newThread->uc_stack.ss_flags = 0;
  newThread->uc_link = NULL;

  
  //How do we makecontext for our helper method that isn't a ucontext? Make it one?
  //makecontext(start_thread, 2, func, arg);
  auto iter = readyQueue.insert(readyQueue.end(), newThread);
  return 1;
}

int thread_lock (unsigned int lock) {
  interrupt_disable();
  //If lock doesnt exist, create it
  int index = findLock(lock);
  if(index < 0) {
    //create lock and add to vector
  }
  //We are so sorry for our variable names
  lockStruct ourLock = locks[index];
  if(!ourLock.busy) {
    ourLock.busy = true;
  } else {
    //What thread do we add? Not currentThread right?
    auto iter = ourLock.blocked.insert(ourLock.blocked.end(), current);
    swapcontext(current, pop());
  }
  interrupt_enable();
  return 1;
}

/**
int thread_unlock (unsigned int lock) {return 1;}
int thread_wait (unsigned int lock, unsigned int cond) {return 1;}
int thread_signal (unsigned int lock, unsigned int cond) {return 1;}
int thread_broadcast (unsigned int lock, unsigned int cond) {return 1;}
*/


int thread_yield(void) {
  ucontext* next = pop();
  swapcontext(current, next);
  auto iter = readyQueue.insert(readyQueue.end(), current);
  current = next;
  return 1;
}
