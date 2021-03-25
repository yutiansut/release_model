#ifndef _HUSKY_FILE_UTILS_H_
#define _HUSKY_FILE_UTILS_H_

#include <vector>
#include <string>

namespace husky {

extern bool file_exists(const std::string &name);

extern bool dir_exists(const std::string &path);

extern std::string extract_directory(const std::string &filePath);

extern void mkdir_recursive(const std::string &dir);

extern size_t file_size(const std::string &filename);

extern std::vector<char> f2c(char const *filename);

extern std::vector<std::string> f2str_vec(const std::string &filename);

}

#endif