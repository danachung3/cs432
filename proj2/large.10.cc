#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{

  for (int i = 0; i < 65; i++){
    char *p;
    p = (char *) vm_extend();
    //for (int y = 0; i< 65; i++){
      p[0] = 'h';
      p[1] = 'I';
      //}
    vm_syslog(p, 100);
  }

  char *d;
  d = (char*) vm_extend(); 
  d[0] = 'H';
  d[1] = 'A';
  d[2] = 'H';
  d[3] = 'A';

  vm_syslog(d, 4);
}
