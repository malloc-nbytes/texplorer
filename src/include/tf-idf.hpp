#ifndef TF_IDF_H
#define TF_IDF_H

#include <unordered_map>
#include <string>
#include <vector>

typedef std::unordered_map<std::string, size_t> FreqMap;
typedef std::pair<FreqMap, size_t> Document;
typedef std::unordered_map<std::string, Document> Corpus;

Corpus assemble_corpus(std::vector<std::string> &filepaths);
std::vector<std::pair<std::string, double>> produce_ranked_documents(std::string &query, Corpus &corpus);
double tf(std::string &term, Document &document);
double idf(std::string &term, Corpus &corpus);

#endif // TF_IDF_H
