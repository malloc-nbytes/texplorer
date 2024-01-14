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

std::vector<std::pair<std::string, Document>> corpus_to_sorted_vec(Corpus &corpus)
{
  std::vector<std::pair<std::string, Document>> sorted_corpus(corpus.begin(), corpus.end());
  std::sort(sorted_corpus.begin(), sorted_corpus.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.second.second > rhs.second.second;
  });
  return sorted_corpus;
}

int main(void)
{
  std::string filepath = "/home/zdh/dev/docs.gl/es2";
  std::vector<std::string> filepaths = walkdir(filepath);

  Corpus corpus = assemble_corpus(filepaths);

  // /home/zdh/dev/docs.gl/es2/glActiveTexture.xhtml
  std::string query = "selects which texture unit subsequent";

  std::vector<std::pair<Document, size_t>> ranked_documents = search_query(query, corpus);

  return 0;
}
