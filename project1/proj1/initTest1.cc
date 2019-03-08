#include <iostream>
#include <fstream>
#include "thread.h"
using namespace std;

int lock = 0;


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
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
}

int main(int argc, char** argv) {

  thread_create((thread_startfunc_t ) thread1, (void *) 1);
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
  // return 1;
}
