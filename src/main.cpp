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
  std::cerr << "  -s, --save.....................Save indexed <path(s)>" << std::endl;
  std::cerr << "  -db <path>, --database <path>..Used indexed files from <path>" << std::endl;
  std::cerr << "  -lim <N>, --limit <N>..........Limit to <N> files shown (def. 10)" << std::endl;
  exit(1);
}

void tfidf(std::vector<std::string> &paths, std::string &query)
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
  std::vector<std::string> paths;

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
      paths.push_back(std::string(inp));
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "s")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "save")) {
      FLAGS |= TF_IDF_FLAG_INDEX;
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "db")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "database")) {
      assert(false && "loading from database is unimplemented");
    }
    else if ((AP_CHECK_1HYPH_OK(arg) && std::string(arg.value) == "lim")
             || (AP_CHECK_2HYPH_OK(arg) && std::string(arg.value) == "limit")) {
      ranked_lim = std::stol(ap_eat(&argc, &argv));
    }
    else if (AP_CHECK_0HYPH_OK(arg)) {
      query.append(std::string(arg.value) + " ");
    }
  }

  tfidf(paths, query);

  return 0;
}

