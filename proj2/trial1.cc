#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{
  char *p;
  p = (char *) vm_extend();
  p[0] = 'h';
  p[1] = 'e';
  p[2] = 'l';
  p[3] = 'l';
  p[4] = 'o';

  char *c;
  c = (char *) vm_extend();
  c[0] = 'o';
  c[1] = 'h';
  c[2] = 'd';
  c[3] = 'e';
  c[4] = 'a';
  c[5] = 'r';

  char *s;
  s = (char *) vm_extend();
  s[0] = 'h';
  s[1] = 'e';
  s[2] = 'l';
  s[3] = 'p';
  s[4] = 'p';
  s[5] = 'l';
  s[6] = 's';

  char *t;
  t = (char *) vm_extend();
  t[0] = 'r';

  char *h;
  h = (char *) vm_extend();
  h[0] = 'F';


  //  vm_syslog(p, 5);
}
