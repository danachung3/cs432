#include <iostream> 
#include <fstream>
#include "thread.h" 
using namespace std; 




int main(int argc, char** argv) {
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5);
  thread_libinit((thread_startfunc_t ) startHelper, (void *) 5); 
}
