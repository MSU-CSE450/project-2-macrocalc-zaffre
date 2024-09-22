#pragma once

#include <assert.h>
#include <string>
#include <unordered_map>
#include <vector>

class SymbolTable {
private:
  // CODE TO STORE SCOPES AND VARIABLES HERE.
  
  // HINT: YOU CAN CONVERT EACH VARIABLE NAME TO A UNIQUE ID TO CLEANLY DEAL
  //       WITH SHADOWING AND LOOKING UP VARIABLES LATER.

public:
  // CONSTRUCTOR, ETC HERE

  // FUNCTIONS TO MANAGE SCOPES (NEED BODIES FOR THESE IF YOU WANT TO USE THEM)
  void PushScope() { ; }
  void PopScope() { ; }

  // FUNCTIONS TO MANAGE VARIABLES - LOTS MORE NEEDED
  // (NEED REAL FUNCTION BODIES FOR THESE IF YOU WANT TO USE THEM)
  bool HasVar(std::string name) const { return false; }
  size_t AddVar(std::string name, size_t line_num) { return 0; }
  double GetValue(std::string name) const {
    assert(HasVar(name));
    return 0.0;
  }
  void SetValue(std::string name, double value) { ; }
};
