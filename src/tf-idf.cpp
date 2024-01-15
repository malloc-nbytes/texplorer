#include <cassert>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <sqlite3.h>

#include "./include/tf-idf.hpp"
#include "./include/utils.hpp"

// const char* createTermsTable = "CREATE TABLE IF NOT EXISTS terms (term_id INTEGER PRIMARY KEY, term_text TEXT UNIQUE, term_frequency INTEGER, document_path TEXT NOT NULL, FOREIGN KEY (document_path) REFERENCES documents (document_path));";
// const char* createDocumentsTable = "CREATE TABLE IF NOT EXISTS documents (document_id INTEGER PRIMARY KEY, document_path TEXT NOT NULL UNIQUE);";

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

document_t produce_document(std::string &filepath)
{
  if (FLAGS & TF_IDF_FLAG_VERBOSE) {
    std::cout << "Indexing: " << filepath << "..." << std::endl;
  }

  std::string text = file_to_str(filepath);
  std::vector<std::string> tokens = produce_tokens(text);
  document_t document;
  freqmap_t freqmap;

  document.second = 0;

  for (std::string &token : tokens) {
    ++freqmap[token];
    ++document.second;
  }

  document.first = std::move(freqmap);

  return document;
}

corpus_t index_documents(std::vector<std::string> &filepaths)
{
  corpus_t corpus;

  for (std::string &filepath : filepaths) {
    corpus[filepath] = produce_document(filepath);
  }

  return corpus;
}

double tf(std::string &term, document_t &document)
{
  return static_cast<double>(document.first[term])/static_cast<double>(document.second);
}

double idf(std::string &term, corpus_t &corpus)
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

std::vector<std::pair<std::string, double>> produce_ranked_documents(std::string &query, corpus_t &corpus)
{
  std::vector<std::pair<std::string, double>> ranked_documents;

  for (auto &pair : corpus) {
    std::cout << "\nRanking: " << pair.first << "..." << std::endl;
    double rank = 0.0;

    for (std::string &term : produce_tokens(query)) {
      double x = tf(term, pair.second)*idf(term, corpus);
      rank += x;
      std::cout << "  " << x << " ";
    }

    ranked_documents.push_back(std::make_pair(pair.first, rank));
  }

  // std::cout << std::endl;

  return ranked_documents;
}

void corpus_to_db(corpus_t &corpus, sqlite3 *db)
{
  std::string insert_document = "INSERT INTO documents (document_path) VALUES (?);";
  std::string insert_term = "INSERT INTO terms (term_text, term_frequency, document_path) VALUES (?, ?, ?);";
  sqlite3_stmt *stmt;

  sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

  sqlite3_prepare_v2(db, insert_document.c_str(), -1, &stmt, nullptr);
  for (auto &pair : corpus) {
    sqlite3_bind_text(stmt, 1, pair.first.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }
  sqlite3_finalize(stmt);

  sqlite3_prepare_v2(db, insert_term.c_str(), -1, &stmt, nullptr);
  for (auto &pair : corpus) {
    std::cout << "Saving: " << pair.first << "..." << std::endl;

    for (auto &term : pair.second.first) {
      sqlite3_bind_text(stmt, 1, term.first.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_int(stmt, 2, term.second);
      sqlite3_bind_text(stmt, 3, pair.first.c_str(), -1, SQLITE_STATIC);
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
  }
  sqlite3_finalize(stmt);

  sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr);
}
