#ifndef TF_IDF_H
#define TF_IDF_H

#include <unordered_map>
#include <string>

typedef std::unordered_map<std::string, size_t> FreqMap;

struct Corpus {
  std::unordered_map<std::string, FreqMap> info;

  inline size_t documents() {
    return this->info.size();
  }

  inline FreqMap &freqmap(std::string &document_name) {
    return this->info[document_name];
  }
};

FreqMap tf_produce_freqs(std::string &text);
double tf(std::string &term, std::unordered_map<std::string, size_t> freqs);

#endif // TF_IDF_H
