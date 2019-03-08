#include <iostream>
#include <fstream>
#include "thread.h"
using namespace std;
int lock = 0;

//Testing unlock before lock
void thread1(void* arg) {
  cout<< "Thread 1 recieved: " << num << "\n";
  thread_unlock(lock);
  cout<< "Made it through unlock before lock\n";
  thread_lock(lock);
  thread_unlock(lock);
  cout<< "Thread 1 out\n";
}

int main(int argc, char** argv) {
  thread_libinit((thread_startfunc_t ) thread1, (void *) 5);
}
