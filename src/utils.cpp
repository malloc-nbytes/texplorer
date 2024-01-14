#include "./include/utils.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

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
    if (std::filesystem::is_regular_file(entry.path())) {
      filePaths.push_back(entry.path());
    } else if (std::filesystem::is_directory(entry.path())) {
      std::vector<std::string> subDir = walkdir(entry.path());
      filePaths.insert(filePaths.end(), subDir.begin(), subDir.end());
    }
  }

  return filePaths;
}

