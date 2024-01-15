#ifndef TF_IDF_H
#define TF_IDF_H

#include <unordered_map>
#include <string>
#include <vector>
#include <sqlite3.h>

typedef std::unordered_map<std::string, size_t> freqmap_t;
typedef std::pair<freqmap_t, size_t> document_t;
typedef std::unordered_map<std::string, document_t> corpus_t;

void corpus_to_db(corpus_t &corpus, sqlite3 *db);
corpus_t index_documents(std::vector<std::string> &filepaths);
std::vector<std::pair<std::string, double>> produce_ranked_documents(std::string &query, corpus_t &corpus);
double tf(std::string &term, document_t &document);
double idf(std::string &term, corpus_t &corpus);

#endif // TF_IDF_H
