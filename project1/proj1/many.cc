#include <iostream>
#include <fstream>
#include "thread.h"
using namespace std;
int lock = 0;

void thread1(void* arg) {
  int* num = (int*) arg;
  cout<< "Thread 1 recieved: " << num << "\n";
  thread_lock(lock);
  thread_yield();

  cout<< "Thread 1 middle\n";
  thread_unlock(lock);
  cout<< "Thread 1 out\n";
}

void startHelper() {
  for(int i = 0; i < 1000; i++) {
    thread_create((thread_startfunc_t ) thread1, (void *) 1);
    cout << "Thread Create Failed";
  }
}

int main(int argc, char** argv) {
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
}
