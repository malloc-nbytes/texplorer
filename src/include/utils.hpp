#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>
#include <sqlite3.h>

#define TF_IDF_FLAG_VERBOSE (1 << 0)
#define TF_IDF_FLAG_INDEX (1 << 1)

extern uint32_t FLAGS;

std::string file_to_str(const std::string &filepath);
std::vector<std::string> walkdir(const std::string &filepath);
sqlite3* init_db(void);

#endif // UTIL_HPP
