#include <cassert>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <sqlite3.h>

#include "./include/tf-idf.hpp"
#include "./include/utils.hpp"

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

std::optional<document_t> produce_document(std::string &filepath)
{
  if (FLAGS & TF_IDF_FLAG_VERBOSE) {
    std::cout << "Indexing: " << filepath << "..." << std::endl;
  }

  std::string text = file_to_str(filepath);
  std::vector<std::string> tokens = produce_tokens(text);

  if (tokens.size() == 0) {
    return std::nullopt;
  }

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
    std::optional<document_t> document = produce_document(filepath);
    if (document) {
      corpus[filepath] = *document;
    }
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
    if (FLAGS & TF_IDF_FLAG_VERBOSE) {
      std::cout << "Ranking: " << pair.first << "..." << std::endl;
    }
    double rank = 0.0;

    for (std::string &term : produce_tokens(query)) {
      double x = tf(term, pair.second)*idf(term, corpus);
      rank += x;
      if (FLAGS & TF_IDF_FLAG_VERBOSE) {
        std::cout << "  " << term << ": " << x << std::endl;
      }
    }

    if (FLAGS & TF_IDF_FLAG_VERBOSE) {
      std::cout << std::endl;
    }

    ranked_documents.push_back(std::make_pair(pair.first, rank));
  }

  return ranked_documents;
}

void corpus_to_db(corpus_t &corpus, sqlite3 *db)
{
  std::printf("Saving corpus to db...\n");
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

corpus_t corpus_from_db(sqlite3 *db)
{
  std::printf("Loading corpus from db...\n");
  corpus_t corpus;
  std::string select_documents = "SELECT document_path FROM documents;";
  std::string select_terms = "SELECT term_text, term_frequency FROM terms WHERE document_path = ?;";
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db, select_documents.c_str(), -1, &stmt, nullptr);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::string document_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    corpus[document_path] = document_t();
  }
  sqlite3_finalize(stmt);

  sqlite3_prepare_v2(db, select_terms.c_str(), -1, &stmt, nullptr);
  for (auto &pair : corpus) {
    sqlite3_bind_text(stmt, 1, pair.first.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      std::string term_text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
      int term_frequency = sqlite3_column_int(stmt, 1);
      pair.second.first[term_text] = term_frequency;
      pair.second.second += term_frequency;

      std::cout << "  " << term_text << " " << term_frequency << std::endl;
    }
    sqlite3_reset(stmt);
  }
  sqlite3_finalize(stmt);

  return corpus;
}
