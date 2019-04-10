#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>
#include <stack>
#include "vm_pager.h"

using namespace std;

struct vpage_t {
  bool dirty;
  bool zero;
  bool resident;
  int disk_block;
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
  for(int i = 0; i < memory_pages; i++){
    physicalMem.push(i); 
  }
  for(int i = 0; i < disk_blocks; i++){
    disk.push(i); 
  }
  struct process_t nil = {NULL, NULL, NULL}; 
  currentProc = nil; 
}

extern void vm_create(pid_t pid) {
  page_table_t pt;
  vector<vpage_t> v;
  struct process_t newProcess = {pid, pt, v};
  if(currentProc.processID == NULL) {
     currentProc = newProcess;
     page_table_base_register = &pt; 
  }
  processes.insert(pair<pid_t, process_t>(pid, newProcess));
}

extern void * vm_extend(){
  if(disk.size() == 0) {
    return NULL;
  }
  int diskLoc = disk.top();
  disk.pop();
  struct vpage_t pte = {0, 0, 0, diskLoc, 0, 0};
  currentProc.vPages.insert(currentProc.vPages.end(), pte); 
  return &currentProc.vPages.at(currentProc.vPages.size() - 1);
}

extern int vm_fault(void *addr, bool write_flag){
  return 1;


}

extern void vm_switch(pid_t pid){
  currentProc = processes.at(pid);
  page_table_base_register = &currentProc.pageTable;
  return;

}

extern void vm_destroy(){
  for(int i = 0; i < currentProc.vPages.size(); i++) {
    vpage_t temp = currentProc.vPages.at(i);
    if(temp.resident) {
      //add physical mem back
    }
    disk.push(temp.disk_block);
    

  }
  

  return;
}


extern int vm_syslog(void *message, unsigned int len){return 1;}
