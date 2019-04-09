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
stack<unsigned int> physicalMem;
stack<unsigned int> disk; 
process_t currentProc;


extern void vm_init(unsigned int memory_pages, unsigned int disk_blocks) {
  physicalMem = new unsigned int [memory_pages];
  disk = new unsigned int [disk_blocks];
  //  map<pid_t, process_t> processes;
}



