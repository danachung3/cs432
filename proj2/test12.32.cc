#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{
  char *p;
  for (int i = 0; i < 64; i++){

    p = (char *) vm_extend();
    p[0] = 'h';
    p[1] = 'e';
    p[2] = 'l';
    p[3] = 'l';
    p[4] = 'o';

    vm_syslog(p, 5); 
  }

  char *q;
  char *w;
  char *e;
  char *r;
  char *t;
  char *y;
  q = (char *) vm_extend();
  q[0] = 'h';
  q[1] = 'e';
  q[2] = 'l';
  q[3] = 'l';
  q[4] = 'o';
  w = (char *) vm_extend();
  e = (char *) vm_extend();
  e[0] = 'f';
  r = (char *) vm_extend();
  r[300] = 't';
  t = (char *) vm_extend();
  p[4] = 'l';
  p[2] = 'e';
  vm_syslog(p,5);
  y = (char *) vm_extend();
  vm_syslog(q,5);
  vm_syslog(e,1);
  w[0] = 'p';
  vm_syslog(p,5);


}

