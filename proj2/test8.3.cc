#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{

  char *p; 
  p = (char *) vm_extend();
  
  vm_syslog(p,5); 

}
