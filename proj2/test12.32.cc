#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{

  for (int i = 0; i < 33; i++){
    char *p;
    p = (char *) vm_extend();
    p[0] = 'h';
    p[1] = 'e';
    p[2] = 'l';
    p[3] = 'l';
    p[4] = 'o';

    vm_syslog(p, 100); 
  }

}

