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
  std::string text = file_to_str(filepath);
  std::vector<std::string> tokens = produce_tokens(text);
  Document document;
  FreqMap freqmap;

  document.second = 0;

  for (std::string &token : tokens) {
    ++freqmap[token];
    ++document.second;
  }

  document.first = std::move(freqmap);
  return document;
}

Corpus assemble_corpus(std::vector<std::string> &filepaths)
{
  Corpus corpus;

  for (std::string &filepath : filepaths) {
    corpus[filepath] = produce_document(filepath);
  }

  return corpus;
}

void dump_corpus(Corpus &corpus)
{
  (void)corpus;
  assert(false && "unimplemented");
}

double tf(std::string &term, Document &document)
{
  size_t f_td = document.first[term];
  return static_cast<double>(f_td)/document.second;
}

double idf(std::string &term, Corpus &corpus)
{
  size_t n = corpus.size();
  size_t dtd = 0;
  for (auto &doc : corpus) {
    if (doc.second.first[term] != 0) {
      ++dtd;
    }
  }
  return std::log(static_cast<double>((n+1)) / (dtd+1));
}

double tfidf(std::string &term, Document &document, Corpus &corpus)
{
  return tf(term, document)*idf(term, corpus);
}
