#include <iostream>
#include <fstream>
#include "vm_pager.h"
#include <vector>
#include <tuple>
#include <map>
#include <stack>
using namespace std;

struct vpage_t {
  bool dirty;
  bool zero;
  bool resident;
  unsigned int disk_block;
  bool read;
  bool write;
};

struct process_t {
  pid_t processID;
  page_table_t pageTable;
  vector<vpage_t> vPages;
};

map<pid_t, process_t> processes;
stack<int> physicalMem;
stack<int> disk; 
process_t currentProc;


extern void vm_init(unsigned int memory_pages, unsigned int disk_blocks) {
  //physicalMem = new stack<int>(memory_pages);
  //disk = new int [disk_blocks];
    map<pid_t, process_t> processes;
}



extern void vm_create(pid_t pid);
extern void vm_switch(pid_t pid);
extern int vm_fault(void *addr, bool write_flag);
extern void vm_destroy();
extern void * vm_extend();
extern int vm_syslog(void *message, unsigned int len);
