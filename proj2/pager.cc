#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>
#include <stack>
#include <queue>
#include "vm_pager.h"
#include <cstdint>
#include <string>
using namespace std;

struct vpage_t {
  bool dirty;
  bool zero;
  bool resident;
  int disk_block;
  bool read;
  int proc;
  bool write;
  bool reference;
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
queue<tuple<int,vpage_t*>> tickTock;

extern void vm_init(unsigned int memory_pages, unsigned int disk_blocks) {
  for(int i = 0; i < memory_pages; i++){
    physicalMem.push(i);
  }
  for(int i = 0; i < disk_blocks; i++){
    disk.push(i);
  }
}

extern void vm_create(pid_t pid) {
  struct process_t *newProcess = new process_t;
  newProcess->processID = pid;

  for(int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE ; i++){
    newProcess->pageTable.ptes[i].ppage = 10000;
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
  pte->proc = (int)currentProc.processID;
  pte->zero=1;
  pte->resident=0;
  pte->disk_block = diskLoc;
  pte->read = 0;
  pte->write = 0;
  pte->reference = 0;

  currentProc.vPages.insert((currentProc.vPages.end()), pte);

  void* address = (void *)VM_ARENA_BASEADDR + ((currentProc.vPages.size() - 1) * VM_PAGESIZE);
  return address;
}

extern int vm_fault(void *addr, bool write_flag){
  unsigned int index = (unsigned int)((unsigned long) addr - (unsigned long)VM_ARENA_BASEADDR) / VM_PAGESIZE;
  page_table_entry_t page_table_entry = currentProc.pageTable.ptes[index]; 
  //vpage_t* current_vpage = currentProc.vPages[index];
  //int i = 0;

  pid_t cur = currentProc.processID;
  //if virtual page does not have a physical page
  if (page_table_entry.ppage == 10000){    

    //If no physical memory available, must run clock algo
    if (physicalMem.empty()){
      //no free space in physical mem, need to evict

      //Loop through clock struct until reference bit of first element is 0
      while (get<1>(tickTock.front())->reference == 1){
	get<1>(tickTock.front())->reference = 0;
	get<1>(tickTock.front())->read = 0;
	get<1>(tickTock.front())->write = 0;
	int i = get<0>(tickTock.front());
	currentProc.pageTable.ptes[i].read_enable = 0;
	currentProc.pageTable.ptes[i].write_enable = 0;
	
	tickTock.push(tickTock.front());
	tickTock.pop();
      }

      //If its dirty, write to disk
      if (get<1>(tickTock.front())->dirty){
	//	disk_write(get<1>(currentProc.clock.front())->disk_block, get<0>(currentProc.clock.front()));
	disk_write(get<1>(tickTock.front())->disk_block, currentProc.pageTable.ptes[get<0>(tickTock.front())].ppage);
	get<1>(tickTock.front())->dirty = 0;
	get<1>(tickTock.front())->read = 0;
	get<1>(tickTock.front())->write = 0;
	get<1>(tickTock.front())->resident = 0;
	get<1>(tickTock.front())->reference = 0;
      }

      //Eviction stuff, check if this is ok
      int evictIndex = get<0>(tickTock.front());
      physicalMem.push(currentProc.pageTable.ptes[evictIndex].ppage);
      currentProc.pageTable.ptes[evictIndex].ppage = 10000;
      currentProc.pageTable.ptes[evictIndex].read_enable = 0;
      currentProc.pageTable.ptes[evictIndex].write_enable = 0;
      tickTock.pop();
    }
    //Grabbing free memory
    int ppage = physicalMem.top();
    physicalMem.pop();

    currentProc.pageTable.ptes[index].ppage = ppage;
    currentProc.vPages[index]->resident = 1;
    
    if(currentProc.vPages[index]->zero == 1) {
      memset((char*) pm_physmem + (ppage * VM_PAGESIZE), 0, VM_PAGESIZE);
    }
    else {
      disk_read(currentProc.vPages[index]->disk_block, currentProc.pageTable.ptes[index].ppage);
    }
    tuple<int,vpage_t*> temp = make_tuple(index, currentProc.vPages[index]);
    tickTock.push(temp);
  }

  //Maybe else if, do we want to make it readable right after giving new phys?
  if (!write_flag){ 
    if (page_table_entry.read_enable == 0){
      currentProc.pageTable.ptes[index].read_enable = 1;
      currentProc.vPages[index]->read = 1;
      currentProc.vPages[index]->reference = 1;

      return 0; 
      }
  }

  //if access is write 
  if (write_flag || currentProc.vPages[index]->dirty == 1){
    if ((page_table_entry.read_enable == 0) || (page_table_entry.write_enable == 0)){
      currentProc.pageTable.ptes[index].read_enable = 1;
      currentProc.pageTable.ptes[index].write_enable = 1;
      currentProc.vPages[index]->read = 1;
      currentProc.vPages[index]->write = 1;
      currentProc.vPages[index]->dirty = 1;
      currentProc.vPages[index]->reference = 1;
      currentProc.vPages[index]->zero = 0;
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

extern void vm_destroy(){
  for(int i = 0; i < currentProc.vPages.size(); i++) {
    vpage_t* temp = currentProc.vPages.at(i);
    if(temp->resident) {
      physicalMem.push(currentProc.pageTable.ptes[i].ppage);
    }
    disk.push(temp->disk_block);
    free(currentProc.vPages[i]);
  }

  int i = tickTock.size();
  while(i > 0) {
    if(get<1>(tickTock.front())->proc == currentProc.processID) {
      tickTock.pop();
    }
    else{
      tickTock.push(tickTock.front());
      tickTock.pop();
    }
    i--;
  }
  
  return;
}


extern int vm_syslog(void *message, unsigned int len){
  if((unsigned long) message + len > (unsigned long)VM_ARENA_SIZE + (unsigned long)VM_ARENA_BASEADDR || (unsigned long) message < (unsigned long)VM)ARENA_BASEADDR || len == 0) { //Maybe more
      return -1;
  }
  string s = "";
  while(len > 0) {
    unsigned int index = ((unsigned long) message - (unsigned long)VM_ARENA_BASEADDR) / (unsigned long) VM_PAGESIZE;
    unsigned int offset = (unsigned int)((unsigned long)message % (unsigned int)VM_PAGESIZE);
    unsigned int temp = VM_PAGESIZE - offset;

    if(currentProc.vPages[index]->resident == 0) {
      currentProc.pageTable.ptes[index].read_enable == 0;
      currentProc.pageTable.ptes[index].write_enable == 0;
      vm_fault(message, 0);
    }

    //ZERO FILLED PAGE
    /**
       if(currentProc.vPages[index]->zero == 0) {
      return -1;
      }*/

    while(offset - VM_PAGESIZE != 0 && len > 0) { 
      unsigned int paddress =  (currentProc.pageTable.ptes[index].ppage * VM_PAGESIZE) + offset;
      s += ((char *)pm_physmem)[paddress];
      offset++;
      len--;
    }
    message += temp;
  }
  s += "\0";
  cout << "syslog \t\t\t" << s << endl;
  return 0;
}
