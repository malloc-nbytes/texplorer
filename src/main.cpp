// Resources:
//   https://en.wikipedia.org/wiki/Tf%E2%80%93idf

// Requirements
//   libsqlite3-dev

#include <cassert>
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sqlite3.h>

#include "./include/utils.hpp"
#include "./include/tf-idf.hpp"

void usage(const char *progname)
{
  std::cerr << "Usage: " << progname << " [OPTIONS...] <query>" << std::endl;
  std::cerr << "Options:" << std::endl;
  std::cerr << "  -h, --help\t\tShow this help message" << std::endl;
  std::cerr << "  -v, --verbose\t\tEnable verbose output" << std::endl;
  exit(1);
}

sqlite3 *init_db(void)
{
  sqlite3 *db;
  int rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    exit(1);
  }

  char *zErrMsg = 0;

  const char *sql = "CREATE TABLE IF NOT EXISTS Documents ("
    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "Path TEXT NOT NULL,"
    "Content TEXT NOT NULL"
    ");";

  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    exit(1);
  }

  sql = "CREATE TABLE IF NOT EXISTS Terms ("
    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "Term TEXT NOT NULL,"
    "DocumentID INTEGER NOT NULL,"
    "Frequency INTEGER NOT NULL,"
    "FOREIGN KEY(DocumentID) REFERENCES Documents(ID)"
    ");";

  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << zErrMsg << std::endl;
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    exit(1);
  }

  return db;
}

int main(void)
{
  sqlite3 *db = init_db();

  std::string filepath = "/home/zdh/dev/docs.gl/";
  std::string query = "gl bind vertex buffer";
  std::vector<std::string> filepaths = walkdir(filepath);
  corpus_t corpus = index_documents(filepaths);

  std::vector<std::pair<std::string, double>> ranked_documents = produce_ranked_documents(query, corpus);

  std::sort(ranked_documents.begin(), ranked_documents.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.second > rhs.second;
  });

  for (size_t i = 0; (i < 10) & (i < ranked_documents.size()); ++i) {
    std::cout << 10-i << ": " << ranked_documents[i].first << " ::: " << ranked_documents[i].second << std::endl;
  }

  sqlite3_close(db);
  return 0;
}
