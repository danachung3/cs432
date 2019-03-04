#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
using namespace std;


int thread_libinit (thread_startfunct_t func, void *arg)


int thread_create (thread_startfunc_t func, void *arg)

int thread_lock (unsigned int lock)
int thread_unlock (unsigned int lock)
int thread_wait (unsigned int lock, unsigned int cond)
int thread_signal (unsigned int lock, unsigned int cond)
int thread_broadcast (unsigned int lock, unsigned int cond)

int thread_yield(void)
