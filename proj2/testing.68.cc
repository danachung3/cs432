#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{
  char *p;
  p = (char *) vm_extend();
  for (int i=0; i < 33; i++){

    p[0] = 'h';
    p[1] = 'e';
    p[2] = 'l';
    p[3] = 'l';
    p[4] = 'o';
  }
  //  vm_syslog(p, 100);

  char* pages[64]; 
  for (int i=0; i< 64; i++){
    pages[i] = (char *) vm_extend();
    pages[i][0] = 'hi';
  }

  for (int i=0; i<64; i++){
    vm_syslog(pages[i], 2); 
  }


}
