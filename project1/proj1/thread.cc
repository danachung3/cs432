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
      //setcontext(current);
      interrupt_enable();
      cout <<"deadlock\n";
      return 1;
    }
    else {
      ucontext* prev = current;
      current = next;
      swapcontext(prev, current);
      //    swapcontext(current, prev);  
    }
    return 0;
}


int thread_lock (unsigned int lock) {
  interrupt_disable();
  //cout << "get here";
  //If lock doesnt exist, create it
  int index = findLock(lock);
  if(index < 0) {
    //create lock and add to vector
    vector<ucontext_t*> newBlock;
    map<unsigned int, vector<ucontext_t*>> condMap;
    struct lockStruct newLock = {lock, false, newBlock, current, condMap};
    auto iter = locks.insert(locks.end(), newLock);
    index = locks.size()-1;
  }
  //  cout << "lock";
  //We are so sorry for our variable names
  lockStruct ourLock = locks[index];
  if(!ourLock.busy) {
    //cout << "hey";
    ourLock.busy = true;
    ourLock.currentOwner = current;
    // auto iter = readyQueue.insert(readyQueue.end(), current);
    //cout << "moving on \n";
    interrupt_enable();
    return 0;

  } else {
    //cout << "sup";
    auto iter = ourLock.blocked.insert(ourLock.blocked.end(), current);
    swap();
  }
  //  cout <<"end lock\n"; 
  interrupt_enable();
  return 0;
}

int thread_unlock (unsigned int lock) {
  interrupt_disable();
  //cout << "starting unlock";
  int index = findLock(lock);

  if (index < 0) {
    //cout<< "Can't find lock";
    return -1;
  }
  //  cout << "getting lockstruct in unlock";
  lockStruct ourLock = locks[index];
  if(ourLock.busy) {
    ourLock.busy = false;
    if (!ourLock.blocked.empty()) {
      //  cout << "about to get off queue";
      ucontext* popped = ourLock.blocked[0];
      //cout << "got off queue";
      ourLock.blocked.erase(ourLock.blocked.begin());
      ourLock.currentOwner = popped;
      ourLock.busy = true; 
      auto iter = readyQueue.insert(readyQueue.end(), popped); 
    }
  }
  //  cout << "FUCK";
  interrupt_enable();
  return 0;
}




int thread_wait (unsigned int lock, unsigned int cond) {
  interrupt_disable();
  cout << "got to WAIT\n";

  int index = findLock(lock);
  if (index < 0){
    cout << "Can't find lock";
    return -1;
  } 
  else {
    cout << "waiting\n";
    lockStruct ourLock = locks[index];
    auto iter = ourLock.condMap.find(cond);
    if(iter == ourLock.condMap.end()) {
      //Condition not found, must add association to map
      cout << "adding condition to map\n";
      vector<ucontext_t*> waiting;
      waiting.insert(waiting.end(), current);
      ourLock.condMap.insert(pair<unsigned int, vector<ucontext_t*>>(cond, waiting));
    }
    else {
      //Adding current thread to end of waiting vector
      ourLock.condMap.find(cond)->second.insert(ourLock.condMap.find(cond)->second.end(), current);
    }

    cout << "unlocking in wait\n";
    //Unlock, put stuff on readyQueue
    ourLock.busy = false;
    if (!ourLock.blocked.empty()) {
      cout << "putting stuff somewhere\n";
      ucontext* popped = ourLock.blocked[0];
      ourLock.blocked.erase(ourLock.blocked.begin());
      ourLock.currentOwner = popped;
      ourLock.busy = true; 
      auto iter = readyQueue.insert(readyQueue.end(), popped); 
    }
    cout << "swapping context";
    //Swap Context
    swap();

    //ADD LOCK
  }
  interrupt_enable(); 
  return 1;
}

int thread_signal (unsigned int lock, unsigned int cond) {
  interrupt_disable();
  //cout << "SIGNAL START\n";
  int index = findLock(lock);
  //cout << "SIGNALING";
  if (index < 0){
    //cout << "Can't find lock";
    return 1;
  } 
  else {
    lockStruct ourLock = locks[index];
    auto iter = ourLock.condMap.find(cond);
    if (iter == ourLock.condMap.end()){
	cout << "empty";
	
    }
    if(ourLock.condMap.find(cond)->second.empty()) {
      //cout <<"throw a fit";
      return 1;
    }
    cout << "\nis it failing here\n"; 
    ucontext_t* popped = ourLock.condMap.find(cond)->second[0];
    cout << "no it's failing AFTER \n";
    ourLock.condMap.find(cond)->second.erase(ourLock.condMap.find(cond)->second.begin());
    if(!ourLock.busy){
      ourLock.busy = true;
      ourLock.currentOwner = popped;
      readyQueue.insert(readyQueue.end(), popped);
    }
    else {
      ourLock.blocked.insert(ourLock.blocked.begin(),popped);
    }
  }
  interrupt_enable();
  return 1;
}





/*
int thread_broadcast (unsigned int lock, unsigned int cond) {return 1;}
*/


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
