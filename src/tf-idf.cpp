#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "./include/utils.hpp"
#include "./include/tf-idf.hpp"

std::vector<std::string> produce_tokens(std::string &text)
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

Document produce_document(std::string &filepath)
{
  std::cout << "Lexing: " << filepath << std::endl;
  std::string text = file_to_str(filepath);
  std::vector<std::string> tokens = produce_tokens(text);
  FreqMap freqmap;
  freqmap.total_terms = 0;

  for (std::string &token : tokens) {
    ++freqmap.freqs[token];
    ++freqmap.total_terms;
  }

  return Document {
    .freqmap = std::move(freqmap),
  };
}

Corpus assemble_corpus(std::vector<std::string> &filepaths)
{
  Corpus corpus;
  for (std::string &filepath : filepaths) {
    corpus.documents[filepath] = produce_document(filepath);
  }
  return corpus;
}

void dump_corpus(Corpus &corpus)
{
  for (auto &doc : corpus.documents) {
    std::cout << "Document: " << doc.first << std::endl;
    FreqMap &fm = doc.second.freqmap;
    std::cout << "Total terms: " << fm.total_terms << std::endl;
    for (auto &freq : fm.freqs) {
      std::cout << "  " << freq.first << " " << freq.second << std::endl;
    }
  }
}

size_t docs_term_is_in(std::string &term, Corpus &corpus)
{
  size_t d = 0;
  for (auto &doc : corpus.documents) {
    if (doc.second.freqmap.freqs[term]) {
      ++d;
    }
  }
  return d;
}

double tf(std::string &term, Document &document)
{
  size_t F_td = document.freqmap.freqs[term];
  return static_cast<double>(F_td)/document.freqmap.total_terms;
}

double idf(std::string &term, Corpus &corpus)
{
  double N = static_cast<double>(corpus.documents.size());
  return std::log((N+1)/(docs_term_is_in(term, corpus)+1));
}

double tfidf(std::string &term, Document &document, Corpus &corpus)
{
  return tf(term, document)*idf(term, corpus);
}
