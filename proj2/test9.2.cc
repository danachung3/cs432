#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{


    char *c;
    c = (char*) vm_extend();
    for (int i = 0; i = 8191; i++){
      c[i] = 'a';
    }
    vm_syslog(c, 100); 

    char *d;
    d = (char*) vm_extend();
    for (int i = 0; i = 100; i++){
       d[i] = 'b'; 
    }
    


    
    vm_syslog(c+8100, 100); 
    vm_syslog(d, 10000); 

 


  
}
