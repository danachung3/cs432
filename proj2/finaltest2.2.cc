#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{
  char *p;
  p = (char*) vm_extend(); 
  for (int i = 0; i < 10; i++){
    p[i] = 'd';
  }

  char *q;
  q = (char*) vm_extend();
  for (int i = 0; i < 10; i++){
    q[i] = 'e';
  }

  char *r;
  r = (char*) vm_extend();
  for (int i= 0;i < 10; i++){
    r[i] = 'f';
  }
  

  for(int i=0; i < 10; i++){
    p[i];
    q[i];
    r[i];
  }

  vm_syslog(p, 10); 
  
}
