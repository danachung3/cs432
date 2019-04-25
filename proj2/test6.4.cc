#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{
  char *p;
  vm_syslog(p, 5);
  p = (char *) vm_extend();
  p = (char *) vm_extend();
  p[0] = 'h';
  p[1] = 'e';
  vm_destroy();
  p[2] = 'l';
  p[3] = 'l';
  p[4] = 'o';
  
}
