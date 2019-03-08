#include <iostream>
#include <fstream>
#include "thread.h"
using namespace std;

int lock = 0;
int cond = 110;


// This test tests thread_create before running thread_libinit and calling thread_libinit twice


void thread1(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 1 recieved: " << num << "\n";
  thread_lock(lock);
  cout<< "Thread 1 middle\n";
  thread_unlock(lock);
  cout<< "Thread 1 out\n";
}


void startHelper() {
  if (thread_libinit((thread_startfunc_t ) startHelper, (void *) 5) = -1){
    cout << "Thread libinit failed";
  };
}

int main(int argc, char** argv) {

  if (thread_create((thread_startfunc_t ) thread1, (void *) 1) = -1){
    cout << "Thread Create Failed";
  };
  if (thread_lock(lock) = -1) {
    cout << "Thread Lock failed"
  }
  if (thread_unlock(lock) = -1) {
    cout << "Thread Unlock failed"
  }
  if (thread_wait(lock, cond) = -1) {
    cout << "Thread wait failed"
  }
  if (thread_signal(lock, cond) = -1) {
    cout << "Thread wait failed"
  }

  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
  // return 1;
}
