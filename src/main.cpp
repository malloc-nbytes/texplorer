// https://en.wikipedia.org/wiki/Tf%E2%80%93idf

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "./include/utils.hpp"
#include "./include/tf-idf.hpp"

int main(void)
{
  std::string filepath = "/home/zdh/dev/docs.gl";
  std::vector<std::string> filepaths = walkdir(filepath);

  Corpus corpus = assemble_corpus(filepaths);


  return 0;
}
