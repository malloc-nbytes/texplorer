#include <string>
#include <vector>
#include "./include/tf-idf.hpp"

std::vector<std::string> tokenize_str(std::string &text)
{
  std::vector<std::string> tokens;
  std::string buf = "";
  const char *it = text.c_str();

  while (*it) {
    if (!std::isalnum(*it) && *it != '\'') {
      if (buf.length() != 0) {
        tokens.push_back(buf);
      }
      if (*it != ' ') {
        tokens.push_back(std::string(1, *it));
      }
      buf.clear();
    } else {
      buf.push_back(std::tolower(*it));
    }
    ++it;
  }

  return tokens;
}

FreqMap tf_produce_freqs(std::string &text)
{
  FreqMap freqs;
  std::vector<std::string> tokens = tokenize_str(text);

  for (std::string &token : tokens) {
    freqs[token]++;
  }

  return freqs;
}

double tf(std::string &term, std::unordered_map<std::string, size_t> freqs)
{
  size_t s = 0;
  for (auto &item : freqs) {
    s += item.second;
  }
  return static_cast<double>(freqs[term])/s;
}
