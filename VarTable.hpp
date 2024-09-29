#pragma once

#include <assert.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "error.hpp"

struct Var {
  std::size_t id;
  std::string name;
  int declared_line;
  double value;
};

class VarTable {
private:
  std::unordered_map<std::string, std::shared_ptr<Var>> variables{};

public:
  bool HasVar(std::string name) const {
    return variables.find(name) != variables.end();
  }

  std::shared_ptr<Var> AddVar(std::size_t id, int lineNumber, std::string name, double value = 0) {
    if (HasVar(name)) {
      Err(lineNumber, "Variable" + name + " already exists");
    }

    variables[name] = std::make_shared<Var>(Var{
        id, name, lineNumber, value});

    return std::shared_ptr<Var>(variables[name]);
  }

  int SetVar(int lineNumber, std::string name, double value) {
    if (!HasVar(name)) {
      Err(lineNumber, "Variable" + name + " was not declared in this scope");
    }

    variables[name]->value = value;
    return variables[name]->id;
  }

  const Var &GetVar(int lineNumber, std::string name) const {
    if (!HasVar(name)) {
      Err(lineNumber, "Variable" + name + " was not declared in this scope");
    }

    return *(variables.at(name));
  }
};