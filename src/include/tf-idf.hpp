#ifndef TF_IDF_H
#define TF_IDF_H

#include <unordered_map>
#include <string>
#include <vector>

struct FreqMap {
  std::unordered_map<std::string, size_t> freqs;
  size_t total_terms;
};

struct Document {
  FreqMap freqmap;
};

struct Corpus {
  std::unordered_map<std::string, Document> documents;
};

Corpus assemble_corpus(std::vector<std::string> &filepaths);
double tf(std::string &term, Document &document);
double idf(std::string &term, Corpus &corpus);
double tfidf(std::string &term, Document &document, Corpus &corpus);

#endif // TF_IDF_H
