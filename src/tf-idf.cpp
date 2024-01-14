#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include "./include/tf-idf.hpp"

std::vector<std::string> tokenize_str(std::string &text)
{
  std::vector<std::string> tokens;
  std::string buf = "";
  const char *it = text.c_str();

  while (*it) {
    if (!std::isalnum(*it) && *it != '\'') {
      if (buf.length() != 0) {
        tokens.push_back(buf);
      }
      if (*it != ' ') {
        tokens.push_back(std::string(1, *it));
      }
      buf.clear();
    } else {
      buf.push_back(std::tolower(*it));
    }
    ++it;
  }

  return tokens;
}

FreqMap produce_freqs(std::string &text)
{
  FreqMap freqs;
  std::vector<std::string> tokens = tokenize_str(text);

  for (std::string &token : tokens) {
    freqs[token]++;
  }

  return freqs;
}

size_t total_terms_in_freqmap(FreqMap &freqmap) {
  size_t s = 0;
  for (auto &pair : freqmap) {
    s += pair.second;
  }
  return s;
}

size_t docs_term_is_in(std::string &term, Corpus &corpus)
{
  size_t d = 0;
  for (auto &document : corpus.documents) {
    if (document.second[term]) {
      ++d;
    }
  }
  return d;
}

double tf(std::string &term, std::string &document, Corpus &corpus)
{
  FreqMap &freqmap = corpus.documents[document];
  return
    static_cast<double>(freqmap[term])/total_terms_in_freqmap(freqmap);
}

double idf(std::string &term, Corpus &corpus)
{
  size_t
    n = corpus.documents.size(),
    dt = docs_term_is_in(term, corpus);
  return std::log((1+n)/(1+dt));
}

double tfidf(std::string &term, std::string &document, Corpus &corpus)
{
  return tf(term, document, corpus)*idf(term, corpus);
}
