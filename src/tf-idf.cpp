#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
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
      if (*it != ' ' && *it != '\n' && *it != '\t') {
        tokens.push_back(std::string(1, *it));
      }
      buf.clear();
    } else {
      buf.push_back(std::tolower(*it));
    }
    ++it;
  }
  if (buf.length() != 0) {
    tokens.push_back(buf);
  }
  return tokens;
}

Document produce_document(std::string &filepath)
{
  std::cout << "Indexing: " << filepath << "..." << std::endl;

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

double tf(std::string &term, Document &document)
{
  return static_cast<double>(document.first[term])/static_cast<double>(document.second);
}

double idf(std::string &term, Corpus &corpus)
{
  double N = static_cast<double>(corpus.size())+1.0;
  double n_t = 0.0+1.0;

  for (auto &pair : corpus) {
    if (pair.second.first[term] > 0) {
      ++n_t;
    }
  }

  return std::log10(N/n_t);
}

std::vector<std::pair<std::string, double>> produce_ranked_documents(std::string &query, Corpus &corpus)
{
  std::vector<std::pair<std::string, double>> ranked_documents;

  for (auto &pair : corpus) {
    double rank = 0.0;

    for (std::string &term : produce_tokens(query)) {
      rank += tf(term, pair.second)*idf(term, corpus);
    }

    ranked_documents.push_back(std::make_pair(pair.first, rank));
  }

  return ranked_documents;
}
