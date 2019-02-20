#include <iostream>
#include <fstream>




int main(int c, char** argv[]) {
  int max_disk_queue = argv[1]; //maximum number of requests the disk can hold
  



  std::ifstream file (filename.c_str()); //reads requester's series of requests
  std::string line;
  if (!file.is_open()) return 0; //file cannot be opened
  while (getline (file, line)){
      std::string request;

      //for each request do something...
  }




}
