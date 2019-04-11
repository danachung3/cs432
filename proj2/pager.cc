#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>
#include <stack>
#include "vm_pager.h"
#include <cstdint>

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
  vector<vpage_t*> vPages;
};

map<pid_t, process_t*> processes;
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
//  struct process_t nil = {NULL, NULL, NULL};
//  currentProc = nil;
}

extern void vm_create(pid_t pid) {
  struct process_t *newProcess = new process_t;
  newProcess->processID = pid;

  for(int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE ; i++){
    newProcess->pageTable.ptes[i].ppage = 100000;
    newProcess->pageTable.ptes[i].read_enable = 0;
    newProcess->pageTable.ptes[i].write_enable = 0;
  }

  processes.insert(pair<pid_t, process_t*>(pid, newProcess));
}

extern void * vm_extend(){
  if(disk.size() == 0) {
    return NULL;
  }
  int diskLoc = disk.top();
  disk.pop();

  struct vpage_t *pte = new vpage_t;
  pte->dirty=0;
  pte->zero=1;
  pte->resident=0;
  pte->disk_block = diskLoc;
  pte->read = 0;
  pte->write = 0;

  currentProc.vPages.insert((currentProc.vPages.end()), pte);

  void* address = (void *)VM_ARENA_BASEADDR + ((currentProc.vPages.size() - 1) * VM_PAGESIZE);
  return address;
}

extern int vm_fault(void *addr, bool write_flag){
  //If the physical memory process has not been set
  //define pm_physmem

  //assuming addr is the address of the vpage_t
  //get index to find page_table_t entry, not sure if thats right?
  //  int index = (addr - VM_ARENA_BASEADDR) / VM_PAGESIZE;
  unsigned int index = (unsigned int)((unsigned long) addr - (unsigned long)VM_ARENA_BASEADDR) / VM_PAGESIZE;
  // unsigned int index = (unsigned int)((unsigned long) addr / VM_PAGESIZE);


  //int index = ((int)addr - (int)VM_ARENA_BASEADDR) / (int)VM_PAGESIZE;
  page_table_entry_t page_table_entry = currentProc.pageTable.ptes[index]; 
  vpage_t* current_vpage = currentProc.vPages[index];
  int i = 0;
  //if address to invalid page
  /**if (i){
    return -1;
    }*/


  //if virtual page does not have a physical page
  if (page_table_entry.ppage == 100000){
    int ppage = physicalMem.top();
    physicalMem.pop();
    
    memset((char*) pm_physmem + ppage * VM_PAGESIZE, 0, VM_PAGESIZE);

    currentProc.pageTable.ptes[index].ppage = ppage;
    currentProc.vPages[index]->resident = 1;
    currentProc.vPages[index]->zero = 0;


    //    page_table_entry.ppage = ppage;

    //    current_vpage->resident = 1; 
    //current_vpage->zero = 0; 

  }


  //if access is read
  if (!write_flag){

    if (page_table_entry.read_enable == 0){
      //do stuff
      
      currentProc.pageTable.ptes[index].read_enable = 1;
      currentProc.vPages[index]->read = 1;

      //      page_table_entry.read_enable = 1;
      //current_vpage->read = 1;

      return 0; 
    }
  }

  //if access is write 
  if (write_flag){

    if ((page_table_entry.read_enable == 0) | (page_table_entry.write_enable == 0)){
      // do stuff


      currentProc.pageTable.ptes[index].read_enable = 1;
      currentProc.pageTable.ptes[index].write_enable = 1;
      currentProc.vPages[index]->read = 1;
      currentProc.vPages[index]->write = 1;
      currentProc.vPages[index]->dirty = 1;
      /**      
      page_table_entry.read_enable = 1;
      page_table_entry.write_enable = 1;
      current_vpage->read = 1;
      current_vpage->write = 1;
      current_vpage->dirty = 1;
      */
      return 0; 
      
    }
  }



  return -1;


}

extern void vm_switch(pid_t pid){
  currentProc = *processes.at(pid);
  page_table_base_register = &currentProc.pageTable;
  return;

}

extern void vm_destroy(){ return;}
/**
extern void vm_destroy(){
  for(int i = 0; i < currentProc->vPages.size(); i++) {
    vpage_t* temp = currentProc->vPages.at(i);
    if(temp.resident) {
      //add physical mem back, doesnt matter what integer 
      physicalMem.push(temp.disk_block);
    }
    disk.push(temp.disk_block);

    // free or delete vpage 
  }


  return;
  }*/


extern int vm_syslog(void *message, unsigned int len){return 1;}
