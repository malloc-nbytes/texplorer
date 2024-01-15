#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sqlite3.h>

#include "./include/utils.hpp"

std::string file_to_str(const std::string &filepath)
{
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << filepath << std::endl;
    return {};
  }
  std::stringstream buf;
  buf << file.rdbuf();
  std::string contents = buf.str();
  file.close();
  return contents;
}

std::vector<std::string> walkdir(const std::string& path)
{
  std::vector<std::string> filePaths;

  if (std::filesystem::is_regular_file(path)) {
    filePaths.push_back(path);
    return filePaths;
  }

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (std::filesystem::is_regular_file(entry.path()) && entry.path().filename().string()[0] != '.') {
      filePaths.push_back(entry.path());
    } else if (std::filesystem::is_directory(entry.path()) && entry.path().filename().string()[0] != '.') {
      std::vector<std::string> subDir = walkdir(entry.path());
      filePaths.insert(filePaths.end(), subDir.begin(), subDir.end());
    }
  }

  return filePaths;
}

sqlite3* init_db(void) {
  sqlite3* db;
  int rc;

  rc = sqlite3_open("test.db", &db);

  if (rc) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return nullptr;
  }

  const char* create_terms_table = "CREATE TABLE IF NOT EXISTS terms (term_id INTEGER PRIMARY KEY, term_text TEXT UNIQUE, term_frequency INTEGER, document_path TEXT NOT NULL, FOREIGN KEY (document_path) REFERENCES documents (document_path));";
  const char* create_documents_table = "CREATE TABLE IF NOT EXISTS documents (document_id INTEGER PRIMARY KEY, document_path TEXT NOT NULL UNIQUE);";

  rc = sqlite3_exec(db, create_terms_table, nullptr, nullptr, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return nullptr;
  }

  rc = sqlite3_exec(db, create_documents_table, nullptr, nullptr, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return nullptr;
  }

  return db;
}