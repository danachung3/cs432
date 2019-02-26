#include <iostream>
#include <fstream>
#include "thread.h"
#include <vector>
#include <tuple>
using namespace std;

vector<tuple<int,int>> queue;
int livethreads = 0;
int lock = 0;
int hasRequest = 1;
int hasRoom = 2;
int canTake = 3;
int max_disk_queue = 0;
vector<string> filenames;

bool queueFull() {
  return  queue.size() ==  max_disk_queue; 
}
bool queueEmpty() {
  return queue.size() == 0; 
  
}

tuple<int,int> queuePop(int disk) {
  int index = 0;
  int min = abs(disk - get<1>(queue[0]));
  for(int i = 0; i < queue.size(); i++){
    if (min > abs(disk - get<1>(queue[i]))){
	min = abs(disk - get<1>(queue[i]));
	index = i;
      }
  }
  tuple<int,int> minimum  = queue[index]; 
  queue.erase(queue.begin()+index); 
  return minimum; 
}

void producer(void* arg) {
  cout << arg; 
  int fileNumber = 0;
  string line;
  string &filename = *(static_cast<string*>(arg));
  ifstream file (filename);
  cout << "Processing " << filename;
  while(getline (file, line)) {//file a has more disk requests
    thread_lock(lock);
    while(queueFull()) {
      thread_wait(lock, hasRoom);
    }
    int track = stoi(line, nullptr, 0); //String to int conversion
    tuple<int,int> request = make_tuple(fileNumber,track);
    auto iter = queue.insert (queue.begin(), request);
    cout << "requester " << fileNumber << " track " << track << endl;
    thread_signal(lock, hasRequest);
    thread_unlock(lock);
  }
}



void consumer() {
  int disk = 0;
  for(int i = 0; i < filenames.size(); i++){
    thread_create((thread_startfunc_t) producer, (void *) static_cast<void*>(&filenames[i]));
  }
  //While there are still requests to handle
  while(livethreads > 0 || !queueEmpty()) {
    thread_lock(lock);

    //Wait until the queue is as full as possible so can take most efficient disk track
    while(!queueFull() && livethreads > 0 ) {
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
}


int main(int argc, char** argv) {
  max_disk_queue = stoi(argv[1], nullptr, 0); //maximum number of requests the disk can hold
  int requester_thread_count = argc - 2; //number of threads making disk requests
  //void* a;
  
  for (int i = 2; i < argc; ++i) {//for every input file...
    auto iter = filenames.insert(filenames.end(), argv[i]);

    //  string filename = argv[i];
  //ifstream file (filename.c_str());
  //string line;
  //if (!file.is_open()) return 0; //file cannot be opened
  //  cout << "creating " << filename << "\n";
  //  thread_create((thread_startfunc_t) producer, (void *) static_cast<void*>(&filename));//create thread??
      //while (getline (file, line)){ //for each line in the input file...
      // int track_number = line;
    }
  thread_libinit((thread_startfunc_t ) consumer, (void *) 5);

}
