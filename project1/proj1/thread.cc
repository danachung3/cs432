#include <iostream>
#include <fstream>
#include <vector>
#include <map>
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
  map<unsigned int, vector<ucontext_t*>> condMap;
};



//const int STACK_SIZE = 100;

vector<ucontext_t*> readyQueue;
vector<lockStruct> locks;
//Map
ucontext_t* current;
ucontext_t* previous;

ucontext_t* pop() {
  if(readyQueue.empty()) { return NULL;}
  ucontext_t* popped = readyQueue[0];
  readyQueue.erase(readyQueue.begin());
  return popped;
}


int findLock(unsigned int id) {
  for(int i = 0; i < locks.size(); i++) {
    //    cout << "print i" << i << locks.size();
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
  
  interrupt_enable();
  return 0;
}


int swap() {
  //Swap context
  ucontext* next = pop();
  if(next == NULL) {
    cout << "Thread library exiting.";
    exit(0);
  }
  else {
    ucontext* prev = current;
    current = next;
    swapcontext(prev, current);
  }
  return 0;
}




int lockHelper (unsigned int lock) {

  //cout << "Size of readyQueue at start of lock: " << readyQueue.size() << endl;
  int index = findLock(lock);
  if(index < 0) {
    //create lock and add to vector
    vector<ucontext_t*> newBlock;
    map<unsigned int, vector<ucontext_t*>> condMap;
    struct lockStruct newLock = {lock, false, newBlock, current, condMap};
    auto iter = locks.insert(locks.end(), newLock);
    index = locks.size()-1;
  }

  //We are so sorry for our variable names
  if(!locks[index].busy) {
    locks[index].busy = true;
    locks[index].currentOwner = current;
    return 0;

  } else {
    //Lock is held by someone else:
    auto iter = locks[index].blocked.insert(locks[index].blocked.end(), current);
    swap();
  }
  return 0;
}

int thread_lock(unsigned int lock) {
  interrupt_disable();
  int temp = lockHelper(lock);
  interrupt_enable();
  return temp;
}




int unlockHelper (unsigned int lock) {
  //  interrupt_disable();
  int index = findLock(lock);

  if (index < 0) {
    return 1;
  }
  if(locks[index].busy) {
    locks[index].busy = false;
    if (!locks[index].blocked.empty()) {
      ucontext* popped = locks[index].blocked[0];
      locks[index].blocked.erase(locks[index].blocked.begin());
      locks[index].currentOwner = popped;
      locks[index].busy = true; 
      auto iter = readyQueue.insert(readyQueue.end(), popped); 
    }
  }
  return 0;
}

int thread_unlock (unsigned int lock) {
  interrupt_disable();
  int temp = unlockHelper(lock);
  interrupt_enable();
  return temp;
}




int thread_wait (unsigned int lock, unsigned int cond) {
  interrupt_disable();
  int index = findLock(lock);
  if (index < 0){
    return -1;
  } 
  else {
    auto iter = locks[index].condMap.find(cond);
    if(iter == locks[index].condMap.end()) {
       vector<ucontext_t*> waiting;
      waiting.insert(waiting.end(), current);
      locks[index].condMap.insert(pair<unsigned int, vector<ucontext_t*>>(cond, waiting));
    }
    else {
      locks[index].condMap.find(cond)->second.insert(locks[index].condMap.find(cond)->second.end(), current);
    }

    unlockHelper(lock);
    swap();
    lockHelper(lock);
  }
  interrupt_enable(); 
  return 0;
}

int thread_signal (unsigned int lock, unsigned int cond) {
  interrupt_disable();
  int index = findLock(lock);
  if (index < 0){
    return 1;
  } 
  else {
    auto iter = locks[index].condMap.find(cond);
    if (iter == locks[index].condMap.end()){
      interrupt_enable();
      return 1;
    }
    if(locks[index].condMap.find(cond)->second.empty()) {
      interrupt_enable();
      return 0;
    }
    ucontext_t* popped = locks[index].condMap.find(cond)->second[0];
    locks[index].condMap.find(cond)->second.erase(locks[index].condMap.find(cond)->second.begin());
    
    locks[index].busy = true;
    locks[index].currentOwner = popped;
    readyQueue.insert(readyQueue.end(), popped);
  }
  interrupt_enable();
  return 0;
}






int thread_broadcast (unsigned int lock, unsigned int cond) {
  

}



int thread_yield(void) {

  //  cout << "got to yield \n";
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
