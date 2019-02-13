#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>


typedef std::map<std::string, std::string> invertedIndex;

//Adds file number to associated word if not already present in file
void addIndex(std::string word, invertedIndex& words, std::string i){
  if (words[word].find(i) == -1){
      words[word] = words[word] + i;
    }
}

//Determines whether a given character is a letter
bool isLetter(char c) {
  if(int(c) < 65 || int(c) > 122 || (int(c) > 90 && int(c) < 97)) {
    return false;
  }
  return true;
}

//Checks given word for correctness and extra non-letter characters
//Breaks up words that contain non-letters
void parseWord(std::string word, invertedIndex& w, std::string i){
  if(word.length() == 0 || int(word.at(0)) == 32) {
    return;
  }
  for(int j = 0; j < word.length(); ++j) {
    if(!isLetter(word.at(j))) {
      parseWord(word.substr(0,j), w, i);
      parseWord(word.substr(j+1), w, i);
      return;
    }
  }
  addIndex(word, w, i);
}

int main(int argc, char** argv) {
  std::string filename = argv[1];
  std::ifstream file (filename.c_str());
  invertedIndex w;
  std::string line;
  int i = 0;

  if (!file.is_open()) return 0;

  //Reads each filename from starter file
  while (getline(file, line)){

    std::ostringstream ss;
    ss << " " << i;
    std::ifstream files (line.c_str());
    if (files.is_open()) {
      std::string word;
      //Reads each word in file
      while (files >>word)
	{
	  parseWord(word, w, ss.str());
	}
      ++i;
    }
  }

  //Prints our map
  for (invertedIndex::iterator p = w.begin();
       p!= w.end(); ++p) {
    std::cout << p->first << ":"
              << p->second << "\n";
  }
}
