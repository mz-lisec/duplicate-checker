// Duplicate code checker based on a simple Levenshtein distance.
//
// Author: Sifan Weng(szefany@gmail.com)

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void GetFilesInDir(const std::string& dirname,
                   std::vector<std::string>* files) {
  files->clear();
  DIR* dir = opendir(dirname.c_str());;
  if (dir == nullptr) {
    std::cerr << "Error directory name: " << dirname << std::endl;
    exit(1);
  }
  std::string fullname;
  dirent* file;
  while (file = readdir(dir)) {
    if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
      fullname = dirname;
      if (*fullname.rbegin() != '/') {
        fullname += "/";
      }
      fullname += file->d_name;
      files->push_back(fullname);
    }
  }
}

std::string Slurp(std::ifstream& in) {
  std::stringstream sstr;
  sstr << in.rdbuf();
  return sstr.str();
}

std::string ReadFile(const std::string& filename) {
  std::ifstream in(filename);
  std::string content;
  if (in.is_open()) {
    content = Slurp(in);
  } else {
    std::cerr << "Error in open file: " << filename << std::endl;
    exit(1);
  }
  in.close();

  std::remove_if(content.begin(), content.end(), [](char c) {
    return c == '\n' || c == '\r';
  });
  auto it = std::unique(content.begin(), content.end(), [](char l, char r) {
    return l == r && l == ' ';
  });
  content.resize(it - content.begin() + 1);
  return content;
}

double CalculateScore(const std::string& a, const std::string& b) {
  int len_a = a.length();
  int len_b = b.length();
  int** dp = new int*[2];
  for (int i = 0; i < 2; ++ i) {
    dp[i] = new int[len_b + 1];
  }
  for (int i = 0; i <= len_b; ++ i) {
    dp[0][i] = i;
  }
  for (int i = 1; i <= len_a; ++ i) {
    dp[i & 1][0] = i;
    for (int j = 1; j <= len_b; ++ j) {
      dp[i & 1][j] = std::min(dp[(i - 1) & 1][j], dp[i & 1][j - 1]) + 1;
      dp[i & 1][j] = std::min(dp[i & 1][j],
                              dp[(i - 1) & 1][j - 1] + (a[i - 1] != b[j - 1]));
    }
  }
  double result = 1 - (double)dp[len_a & 1][len_b] / std::max(len_a, len_b);
  for (int i = 0; i < 2; ++ i) {
    delete [] dp[i];
  }
  delete [] dp;
  return result;
}

void DupCheck(const std::vector<std::string>& files,
              std::vector<std::vector<double>>* matrix) {
  int size = files.size();
  std::string a, b;
  for (int i = 0; i < size; ++ i) {
    (*matrix)[i][i] = 0;
    for (int j = i + 1; j < size; ++ j) {
      a = ReadFile(files[i]);
      b = ReadFile(files[j]);
      (*matrix)[i][j] = (*matrix)[j][i] = CalculateScore(a, b);
    }
  }
}

void PrintName(const std::string& filename,
               const std::vector<std::string>& files) {
  std::ofstream out(filename);
  for (int i = 0; i < files.size(); ++ i) {
    out << i << ": " << files[i] << std::endl;
  }
  out.close();
}

void PrintResult(const std::string& filename,
                 const std::vector<std::vector<double>>& matrix) {
  std::ofstream out(filename);
  for (int i = 0; i < matrix.size(); ++ i) {
    for (int j = 0; j < matrix.size(); ++ j) {
      out << matrix[i][j] << " ";
    }
    out << std::endl;
  }
  out.close();
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./dupCheck [dir]\n" << std::endl;
    return 1;
  }

  std::cerr << "Fetching files..." << std::endl;

  std::string dir = argv[1];
  std::vector<std::string> files;
  GetFilesInDir(dir, &files);

  std::cerr << files.size() << " files fetched." << std::endl;

  std::sort(files.begin(), files.end());
  std::vector<std::vector<double>> matrix;
  int size = files.size();
  matrix.resize(size);
  for (auto& v : matrix) {
    v.resize(size);
  }

  std::cerr << "Duplicate checking..." << std::endl;
  DupCheck(files, &matrix);

  std::cerr << "Done." << std::endl;
  std::cerr << "Generating outputs to name.txt and result.txt" << std::endl;

  PrintName("name.txt", files);
  PrintResult("result.txt", matrix);

  std::cerr << "Done." << std::endl;
  return 0;
}
