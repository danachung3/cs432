#include <iostream>
#include <fstream>
#include "thread.h"
using namespace std;
int lock = 0;

void thread1(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 1 recieved: " << num << "\n";
  thread_lock(lock);
  thread_lock(lock);
  thread_yield();

  cout<< "Thread 1 middle\n";
  thread_unlock(lock);
  cout<< "Thread 1 out\n";
}

void thread2(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 2 recieved: " << num << "\n"; 
  thread_lock(lock);
  cout<< "Thread 2 middle\n";
  thread_unlock(lock);
  cout<< "Thread 2 out\n";
}

void thread3(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 3 recieved: " << num << "\n"; 
  thread_lock(lock);
  cout<< "Thread 3 middle\n";
  thread_unlock(lock);
  cout<< "Thread 3 out\n";
}





void startHelper() {
  thread_create((thread_startfunc_t ) thread1, (void *) 1);
  thread_create((thread_startfunc_t ) thread2, (void *) 2);
  thread_create((thread_startfunc_t ) thread3, (void *) 2);
}

int main(int argc, char** argv) {
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
}
