#include <iostream>
#include <stdio.h>
#include "vm_app.h"
using namespace std;
int main()
{
  char* p;
  while(1){
    char*p;
    p = (char*) vm_extend();
    p[0] = 1;
  }
}
