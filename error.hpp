#pragma once
#include <iostream>
#include <string>


class Err : public std::exception {
public:
  explicit Err(std::size_t lineNumber, std::string errorMessage) {
    std::stringstream ss;
    ss << "ERROR";
    if (lineNumber){
       ss << "(line " << lineNumber << ")";
    }
    ss << ": " << errorMessage;
    std::cerr << ss.str() << std::endl;
  }

  // Override the what() function to provide the error message
  virtual const char* what() const noexcept override {
      return message_.c_str();
  }

private:
    std::string message_;
};