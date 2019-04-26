#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{


    char *c;
    c = (char*) vm_extend();
    for (int i = 0; i = 8193; i++){
      c[i] = 'a';
    }

    //    char *d;
    //d = (char*) vm_extend();
    //for (int i = 0; i = 9000; i++){
    //   d[i] = 'b'; 
    //}
    


    
    //    vm_syslog(0, 9000); 
    //    vm_syslog(d, 10000); 
    vm_syslog(c, 0);
 


  
}
