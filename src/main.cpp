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
  std::string filepath = "/home/zdh/school/";
  std::string query = "virus";

  std::vector<std::string> filepaths = walkdir(filepath);

  Corpus corpus = assemble_corpus(filepaths);
  std::cout << "Files: " << corpus.size() << std::endl;

  std::vector<std::pair<std::string, double>> ranked_documents = produce_ranked_documents(query, corpus);

  std::sort(ranked_documents.begin(), ranked_documents.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.second > rhs.second;
  });

  for (size_t i = 0; (i < 10) & (i < ranked_documents.size()); ++i) {
    std::cout << 10-i << ": " << ranked_documents[i].first << " ::: " << ranked_documents[i].second << std::endl;
  }

  return 0;
}
