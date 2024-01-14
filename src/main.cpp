// https://en.wikipedia.org/wiki/Tf%E2%80%93idf

#include <cassert>
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "./include/utils.hpp"
#include "./include/tf-idf.hpp"

int main(void)
{
  std::string filepath = "/home/zdh/dev/docs.gl/es2";
  std::vector<std::string> filepaths = walkdir(filepath);

  Corpus corpus = assemble_corpus(filepaths);
  std::vector<std::pair<std::string, Document>> sorted_corpus(corpus.begin(), corpus.end());

  std::sort(sorted_corpus.begin(), sorted_corpus.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.second.second > rhs.second.second;
  });

  for (std::pair<std::string, Document> &pair : sorted_corpus) {
    std::cout << "Document: " << pair.first << " => " << pair.second.second << std::endl;
  }

  return 0;
}
