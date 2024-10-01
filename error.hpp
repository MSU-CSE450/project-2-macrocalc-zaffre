#pragma once
#include <iostream>
#include <string>

#include "lexer.hpp"

class Err : public std::exception {
public:
  template <typename... Ts>
  explicit Err(size_t line_num, Ts... message) {
    std::cerr << "ERROR (line " << line_num << "): ";
    (std::cerr << ... << message);
    std::cerr << std::endl;
    exit(1);
  }

  template <typename... Ts>
  explicit Err(emplex::Token token, Ts... message) {
    Err(token.line_id, message...);
  }

  // Override the what() function to provide the error message
  virtual const char *what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_;
};