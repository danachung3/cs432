#include <iostream>
#include <fstream>
#include "thread.h"
#include <vector>
#include <tuple>
using namespace std;


int livethreads = 0;
int requests = 0;
int lock = 100;
int hasRequest = 110;
int hasRoom = 120;
int canTake = 130;
int live = 140;
int max_disk_queue = 0;
vector<tuple<int,string>> filenames;
vector<tuple<int,int>> queue;

bool queueFull() {
  return queue.size() == max_disk_queue; 
}
bool queueEmpty() {
  return queue.size() == 0;   
}
bool needsToWait() {
  return !queueFull();
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
  thread_lock(live);
  livethreads++;
  thread_unlock(live);
  string line;
  tuple<int,string>* fileInfo = (tuple<int,string>*) arg;
  int fileNumber = get<0>(*fileInfo);
  string fileName = get<1>(*fileInfo);
  ifstream file (fileName);
  //  cout << "Processing " << fileName << "\n";
  while(getline (file, line)) {//file a has more disk requests
    thread_lock(lock);
    requests++;
    //cout << filename << " aquired lock \n";
    while(queueFull()) {
      thread_wait(lock, hasRoom);
    }
    //cout << filename << " is done waiting \n";
    int track = stoi(line, nullptr, 0); //String to int conversion
    tuple<int,int> request = make_tuple(fileNumber,track);
    auto iter = queue.insert (queue.begin(), request);
    cout << "requester " << fileNumber << " track " << track << endl;
    thread_signal(lock, hasRequest);
    //cout << "Signaling hasRequest\n";
    //Waiting until specific request is serviced
    thread_wait(lock, fileNumber);
    requests--;
    thread_unlock(lock);
  }
  thread_lock(live);
  livethreads--;
  thread_unlock(live);
}



void consumer() {
  int disk = 0;
  while(1){
    thread_lock(lock);
    //Need to wait until queue is full to get optimal disk track pick
    while(needsToWait()){//&& livethreads > 0 ) {
      thread_wait(lock, hasRequest);
    }
   
    //Processes disk request
    tuple<int,int> request = queuePop(disk);
    disk = get<1>(request);
    cout << "service requester " << get<0>(request) << " track " << get<1>(request) << endl;

    //Alerts specific producer that the request has been serviced
    thread_signal(lock, hasRoom);
    thread_signal(lock, get<0>(request));
    thread_unlock(lock);
  }
}

void beginThreads() {
  thread_create((thread_startfunc_t ) consumer, (void *) 5);
  for(int i = 0; i < filenames.size(); i++){
    thread_create((thread_startfunc_t) producer, (void *) static_cast<void*>(&filenames[i]));
    thread_yield();
  }
}


int main(int argc, char** argv) {
  max_disk_queue = stoi(argv[1], nullptr, 0); //maximum number of requests the disk can hold
  int requester_thread_count = argc - 2; //number of threads making disk requests
  for (int i = 2; i < argc; ++i) {//for every input file...
    tuple<int,string> fileInfo = make_tuple(i - 2, argv[i]);
    auto iter = filenames.insert(filenames.end(), fileInfo);

    //  string filename = argv[i];
  //ifstream file (filename.c_str());
  //string line;
  //if (!file.is_open()) return 0; //file cannot be opened
  //  cout << "creating " << filename << "\n";
  //  thread_create((thread_startfunc_t) producer, (void *) static_cast<void*>(&filename));//create thread??
      //while (getline (file, line)){ //for each line in the input file...
      // int track_number = line;
    }
  thread_libinit((thread_startfunc_t ) beginThreads, (void *) 5);

}
