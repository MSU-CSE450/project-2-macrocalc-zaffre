#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "compiler.hpp"
#include "logger.hpp"

extern bool shouldLog;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Format: " << argv[0] << " [filename]" << std::endl;
    exit(1);
  }

  shouldLog = false;

  if (argc == 3 && argv[2] == std::string("-v")) {
    shouldLog = true;
  }

  std::string filename = argv[1];

  std::ifstream in_file(filename); // Load the input file
  if (in_file.fail()) {
    std::cout << "ERROR: Unable to open file '" << filename << "'." << std::endl;
    exit(1);
  }

  auto compiler = Compiler(in_file);

  compiler.parse();
  compiler.execute();
}
