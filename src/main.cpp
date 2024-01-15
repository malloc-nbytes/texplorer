// Resources:
//   https://en.wikipedia.org/wiki/Tf%E2%80%93idf

// Requirements
//   libsqlite3-dev

#define AP_IMPL
#include "./include/ap.h"

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

uint32_t FLAGS = 0;

void usage(const char *progname)
{
  std::cerr << "Usage: " << progname << " [OPTIONS...] -i=<path>... <query>" << std::endl;
  std::cerr << "Options:" << std::endl;
  std::cerr << "  -h, --help.................Show this help message" << std::endl;
  std::cerr << "  -v, --verbose..............Enable verbose output" << std::endl;
  std::cerr << "  -i=<path>, --index=<path>..Index <path>" << std::endl;
  std::cerr << "  -s, --save.................Save indexed <path>" << std::endl;
  exit(1);
}

void begin(std::vector<std::string> paths, std::string &query)
{
  sqlite3 *db = nullptr;

  if (FLAGS & TF_IDF_FLAG_INDEX) {
    db = init_db();
  }

  for (std::string &filepath : paths) {
    std::vector<std::string> filepaths = walkdir(filepath);

    corpus_t corpus = index_documents(filepaths);

    if (db) {
      corpus_to_db(corpus, db);
    }

    std::vector<std::pair<std::string, double>> ranked_documents = produce_ranked_documents(query, corpus);

    std::sort(ranked_documents.begin(), ranked_documents.end(), [](const auto &lhs, const auto &rhs) {
      return lhs.second > rhs.second;
    });

    for (size_t i = 0; (i < 10) & (i < ranked_documents.size()); ++i) {
      std::cout << 10-i << ": " << ranked_documents[i].first << " ::: " << ranked_documents[i].second << std::endl;
    }
  }

  if (db) {
    sqlite3_close(db);
  }
}

int main(int argc, char **argv)
{
  (void)ap_eat(&argc, &argv);
  usage(ap_prog_name());

  return 0;
}

