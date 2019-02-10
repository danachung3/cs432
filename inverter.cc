//to do:
// - Fix edge cases: parse words only, handle empty file, spacing...
// - read in input file correctly
// - Do you need to close the files???
// - check auto grader
// - is this the most efficient way/does it matter
// - autograder

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>


typedef std::map<std::string, std::string> invertedIndex;

void addIndex(std::string word, invertedIndex& words, std::string i){
  words[word] = words[word] + i;
}

bool isLetter(char c) {
  if(int(c) < 65 || int(c) > 122 || (int(c) > 90 && int(c) < 97)) {
    return false;
  }
  return true;
}
	  

void parseWord(std::string word, invertedIndex& w, std::string i){
  if(word.length() == 0 || int(word.at(0)) == 32) {
    return;
  }

  for(int j = 0; j < word.length(); ++j) {
    if(!isLetter(word.at(j))) {
      std::string str1 = word.substr(0, j);
      std::string str2 = word.substr(j+1);
      //std::cout << str1 + "\n";
      //std::cout << str2 + "\n";
      parseWord(str1, w, i);
      parseWord(str2, w, i);
      return;
    }
  }
  addIndex(word, w, i);
}
  
    

int main(int argc, char** argv) {
  std::string filename = argv[1];
  std::ifstream file (filename.c_str());
  if (!file.is_open()) return 0;

  invertedIndex w;
  std::string line;
  int i = 0;

  while (getline(file, line)){

    std::ostringstream ss;
    ss << " " << i;
    std::ifstream files (line.c_str());
    if (files.is_open()) {
      std::string word;
      while (files >>word)
	{
	  parseWord(word, w, ss.str());
	/**for(int j = 0; j < word.length(); ++j){
	  if(int(word.at(j)) < 65 || int(word.at(j)) > 122 || (int(word.at(j)) < 90 && int(word.at(j)) < 97)) {
	      word.erase(j, 1);
	      --j;
	  }	  
	}
	addIndex(word, w, ss.str());*/
	}
      ++i;
    }
  }

  for (invertedIndex::iterator p = w.begin();
       p!= w.end(); ++p) {
    std::cout << p->first << ":"
              << p->second << "\n";
  }
}
