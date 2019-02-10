//to do:
// - Fix edge cases: parse words only, handle empty file, spacin...
// - read in input file correctly
// - Do you need to close the files???
// - check auto grader
// - is this the most efficient way/does it matter

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>


typedef std::map<std::string, std::string> invertedIndex;

void addIndex(std::string word, invertedIndex& words, std::string i){
  words[word] = words[word] + i;
}

int main() {

  std::ifstream file ("inputs.txt");
  if (!file.is_open()) return 0;

  invertedIndex w;
  std::string line;
  int i = 0;

  while (getline(file, line)){

    std::ostringstream ss;
    ss << i << " ";
    std::ifstream files (line.c_str());
    if (!files.is_open()) return 0;
    std::string word;
    while (files >>word)
      {
        addIndex(word, w, ss.str());
      }
    ++i;
  }


  for (invertedIndex::iterator p = w.begin();
       p!= w.end(); ++p) {
    std::cout << p->first << ": "
              << p->second << " \n";
  }
}
