#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>

std::string file_to_str(const std::string &filepath);
std::vector<std::string> walkdir(const std::string &filepath);

#endif // UTIL_HPP
