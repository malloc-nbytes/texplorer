#ifndef TF_IDF_H
#define TF_IDF_H

#include <unordered_map>
#include <string>
#include <vector>

typedef std::unordered_map<std::string, size_t> FreqMap;
typedef std::pair<FreqMap, size_t> Document;
typedef std::unordered_map<std::string, Document> Corpus;

Corpus assemble_corpus(std::vector<std::string> &filepaths);
void dump_corpus(Corpus &corpus);
std::vector<std::pair<Document, size_t>> search_query(std::string &query, Corpus &corpus);
double tf(std::string &term, Document &document);
double idf(std::string &term, Corpus &corpus);
double tfidf(std::string &term, Document &document, Corpus &corpus);

#endif // TF_IDF_H
