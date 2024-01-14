#ifndef TF_IDF_H
#define TF_IDF_H

#include <unordered_map>
#include <string>

typedef std::unordered_map<std::string, size_t> FreqMap;

struct Corpus {
  //                 doc name,   frequencies
  std::unordered_map<std::string, FreqMap> documents;
};

FreqMap tf_produce_freqs(std::string &text);
double tf(std::string &term, std::unordered_map<std::string, size_t> freqs);
double idf(std::string &term, Corpus &corpus);
double tfidf(std::string &term, Corpus &corpus);

#endif // TF_IDF_H
