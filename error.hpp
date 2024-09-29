#pragma once
#include <iostream>
#include <string>

class Err {
public:
  Err(std::size_t lineNumber, std::string errorMessage) {
    std::cerr << "ERROR (line " << lineNumber << "): " << errorMessage << std::endl;
    exit(1);
  }
};