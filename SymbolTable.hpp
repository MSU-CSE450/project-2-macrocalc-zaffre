#pragma once

#include <algorithm>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "VarTable.hpp"
#include "error.hpp"

class SymbolTable {
private:
  std::vector<VarTable> scopes{};
  std::vector<std::shared_ptr<Var>> variables{};

  std::vector<VarTable>::const_reverse_iterator GetScope(std::string varName) const {
    return std::find_if(
        scopes.rbegin(),
        scopes.rend(),
        [&varName](auto &scope) { return scope.HasVar(varName); });
  }

  bool IsValidScope(std::vector<VarTable>::const_reverse_iterator scope) const {
    return scope != scopes.rend();
  }

public:
  void PushScope() {
    scopes.push_back(VarTable{});
  }

  void PopScope() {
    scopes.pop_back();
  }

  bool HasVar(std::string name) const {
    return IsValidScope(GetScope(name));
  }

  size_t AddVar(std::string name, size_t lineNumber) {
    if (scopes.empty()) {
      throw Err(lineNumber, "Scope not initialized");
    }

    std::size_t id = variables.size();
    variables.push_back(scopes.back().AddVar(id, lineNumber, name));
    return id;
  }

  /// Set/Get by id
  double GetValue(int lineNumber, size_t id) const {
    if (id >= variables.size()) {
      throw Err(lineNumber, "Getting variable that does not exist");
    }
    return variables.at(id)->value;
  }

  void SetValue(int lineNumber, size_t id, double value) {
    if (id >= variables.size()) {
      throw Err(lineNumber, "Setting variable that does not exist");
    }
    variables.at(id)->value = value;
  }

  /// Set/Get by name
  double GetValue(int lineNumber, std::string name) const {
    auto scope = GetScope(name);
    if (!IsValidScope(scope)) {
      throw Err(lineNumber, "Variable " + name + "does not exist");
    }

    return scope->GetVar(lineNumber, name).value;
  }

  std::size_t GetIdByName(int lineNumber, std::string name) {
    auto scope = GetScope(name);
    if (!IsValidScope(scope)) {
      throw Err(lineNumber, "Variable " + name + "does not exist");
    }

    return scope->GetVar(lineNumber, name).id;
  }

  void SetValue(int lineNumber, std::string name, double value) {
    scopes.back().SetVar(lineNumber, name, value);
  }

  std::size_t GetScopeCount() {
    return scopes.size();
  }
};
