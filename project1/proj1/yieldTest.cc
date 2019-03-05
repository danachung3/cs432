#include <iostream>
#include <fstream>
#include "thread.h"
using namespace std;


void thread1(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 1 recieved: " << num << "\n";
  cout<< "Thread 1 starting\n";
  thread_yield();
  cout<< "Thread 1 yielded\n";
  thread_yield();
  cout<< "Thread 1 out\n";
}

void thread2(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 2 recieved: " << num << "\n"; 
  cout<< "Thread 2 starting\n";
  thread_yield();
  cout<< "Thread 2 yielded\n";
  thread_yield();
  cout<< "Thread 2 out\n";
}

void startHelper() {
  thread_create((thread_startfunc_t ) thread1, (void *) 1);
  thread_create((thread_startfunc_t ) thread2, (void *) 2);
}

int main(int argc, char** argv) {
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
}
