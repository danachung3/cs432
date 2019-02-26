#include <iostream>
#include <fstream>
#include "thread.h"
#include <vector>
#include <tuple>
using namespace std;

vector<tuple<int,int>> queue;
int livethreads;
int lock = 0;
int hasRequest = 1;
int hasRoom = 2;


bool queueFull() {return 1;}
bool queueEmpty() {return 1;}
tuple<int,int> queuePop(int disk) {return queue[0];}

void consumer(void* a) {
  int disk = 0;
  //While there are still requests to handle
  while(livethreads > 0 && !queueEmpty()) {
    thread_lock(lock);
    //Wait until the queue is as full as possible so can take most efficient disk track
    while(!queueFull() && livethreads) {
      thread_wait(lock, canTake);
    }
    //Gets closest request to current disk
    tuple<int,int> request = queuePop(disk);
    disk = get<1>(request);
    cout << "service requester " << get<0>(request) << " track " << get<1>(request) << endl;
    //Calls for another producer
    thread_signal(lock, hasRoom);
    thread_unlock(lock);
  }
  cout << "Finished servicer";
}

void producer(void* a) {
  int fileNumber = 0;
  while(1) {//file a has more disk requests
    thread_lock(lock);
    while(queueFull()) {
      wait(lock, hasRoom);
    }
    tuple<int,int> request = make_tuple(fileNumber,1);
    queue.insert(0, request);
    cout << "requester " << fileNumber << " track " << track << endl;
    thread_signal(hasRequest);
    thread_unlock(lock);
  }
}

int main(int argc, char** argv[]) {
  int max_disk_queue = argv[1]; //maximum number of requests the disk can hold
  int requester_thread_count = argc - 2; //number of threads making disk requests

  for (int i = 2; i < argc; ++i) //for every input file...
    string filename = argv[i];
    ifstream file (filename.c_str());
    string line;
    if (!file.is_open()) return 0; //file cannot be opened

    thread_create( producer  , tuple?   )//create thread??
    while (getline (file, line)){ //for each line in the input file...
        int track_number = line; // get track number of request



    }


}
