// Resources:
//   https://en.wikipedia.org/wiki/Tf%E2%80%93idf

// Requirements
//   libsqlite3-dev

#include <iterator>
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
size_t ranked_lim = 10;

void usage(const char *progname)
{
  std::cerr << "Usage: " << progname << " [OPTIONS...] -i=<path>... <query1> <query2> ..." << std::endl;
  std::cerr << "Options:" << std::endl;
  std::cerr << "  -h, --help.....................Show this help message" << std::endl;
  std::cerr << "  -v, --verbose..................Enable verbose output" << std::endl;
  std::cerr << "  -i <path>, --index <path>......Index <path>" << std::endl;
  std::cerr << "  -s <path>, --save <path>.......Save indexed files to <path>" << std::endl;
  std::cerr << "  -db <path>, --database <path>..Used indexed files from <path>" << std::endl;
  std::cerr << "  -lim <N>, --limit <N>..........Limit to <N> files shown (def. 10)" << std::endl;
  exit(1);
}

void tfidf(std::string &index_path,
           std::string &query,
           std::string &db_save_path,
           std::string &db_from_path)
{
  sqlite3 *db = nullptr;

  if (FLAGS & TF_IDF_FLAG_SAVE) {
    db = init_db(db_save_path);
  }

  if (FLAGS & TF_IDF_FLAG_LOAD) {
    db = init_db(db_from_path);
  }

  corpus_t corpus;

  if (db && (FLAGS & TF_IDF_FLAG_LOAD)) {
    corpus = corpus_from_db(db);
  }
  else {
    if (index_path == "") {
      std::cerr << "Error: -i=<path>... must be specified if not loading from a db." << std::endl;
      exit(1);
    }
    std::vector<std::string> filepaths = walkdir(index_path);
    corpus = index_documents(filepaths);
  }

  if (db && (FLAGS & TF_IDF_FLAG_SAVE)) {
    corpus_to_db(corpus, db);
  }

  std::cout << "Ranking documents..." << std::endl;
  std::vector<std::pair<std::string, double>> ranked_documents = produce_ranked_documents(query, corpus);

  std::cout << "Sorting documents" << std::endl;
  std::sort(ranked_documents.begin(), ranked_documents.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.second > rhs.second;
  });

  std::cout << "Results" << std::endl;
  for (size_t i = 0; (i < ranked_lim) && (i < ranked_documents.size()); ++i) {
    std::cout << i << ": " << ranked_documents[i].first << " ::: " << ranked_documents[i].second << std::endl;
  }

  if (db) {
    sqlite3_close(db);
  }
}

int main(int argc, char **argv)
{
  (void)ap_eat(&argc, &argv);
  if (argc < 2) {
    usage(ap_prog_name());
  }

  std::string query = "";
  std::string index_path = "";
  std::string db_save_path = "";
  std::string db_from_path = "";

  char *inp;
  while ((inp = ap_eat(&argc, &argv)) != NULL) {
    struct Arg arg = ap_parse(inp, '-');

    if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "v")
        || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "verbose")) {
      FLAGS |= TF_IDF_FLAG_VERBOSE;
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "h")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "help")) {
      usage(ap_prog_name());
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "i")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "index")) {
      inp = ap_eat(&argc, &argv);
      index_path = std::string(inp);
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "s")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "save")) {
      FLAGS |= TF_IDF_FLAG_SAVE;
      db_save_path = std::string(ap_eat(&argc, &argv));
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "db")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "database")) {
      FLAGS |= TF_IDF_FLAG_LOAD;
      db_from_path = std::string(ap_eat(&argc, &argv));
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "lim")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "limit")) {
      ranked_lim = std::stol(ap_eat(&argc, &argv));
    }
    else if (AP_CHECK_0HYPH_OK(arg)) {
      query.append(std::string(arg.value) + " ");
    }
  }

  tfidf(index_path, query, db_save_path, db_from_path);

  return 0;
}

