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

  //  if (vm_syslog(p, 5) == 0){
  // cout << "syslog successful"; 
  //}
  //else {
  //  cout << "syslog failed"; 
  //}

  vm_syslog(p,5); 

}
