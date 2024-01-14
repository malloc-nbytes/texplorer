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

  if (std::filesystem::is_directory(path)) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
      const auto& filename = entry.path().filename().string();
      if (std::filesystem::is_regular_file(entry) && filename[0] != '.' && filename.find('/') == std::string::npos) {
        filePaths.push_back(entry.path().string());
      }
    }
  } else if (std::filesystem::is_regular_file(path) && path[0] != '.' && path.find('/') == std::string::npos) {
    filePaths.push_back(path);
  } else {
    std::cerr << "Invalid path: " << path << std::endl;
  }

  return filePaths;
}

