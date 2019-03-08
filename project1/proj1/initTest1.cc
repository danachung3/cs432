#include <iostream> 
#include <fstream>
#include "thread.h" 
using namespace std; 

int lock = 0;

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
  cout << thread_libinit((thread_startfunc_t) startHelper, (void *) 5); 
  cout << "\nThis should fail again 2\n"; 
  
}

void secondHelper() {
  thread_create((thread_startfunc_t ) thread1, (void *) 1);
}


int main(int argc, char** argv) {
  //  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);

  cout << "\ntesting\n"; 
  cout << thread_create((thread_startfunc_t ) thread1, (void *) 1); 
  //  return thread_create((thread_startfunc_t ) thread1, (void *) 1);
  cout << "\nThis should fail\n";

  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
  // return 1; 
}




