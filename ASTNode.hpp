#pragma once

#include <cmath>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "SymbolTable.hpp"
#include "lexer.hpp"
#include "logger.hpp"

class ASTNode {
public:
  enum Type {
    EMPTY = 0,
    STATEMENT_BLOCK,
    EXPRESSION,
    ASSIGN,
    VARIABLE,
    VALUE,
    PRINT,
  };

private:
  Type type = EMPTY;
  std::size_t id;
  double value;
  std::vector<std::shared_ptr<ASTNode>> child{};
  emplex::Token token;

  void RunAssign(SymbolTable &symbols) {
    logger << "Running assign" << std::endl;
    // Run expression
    child.at(1)->Run(symbols);
    // Set the value
    symbols.SetValue(0, child.at(0)->GetId(), child.at(1)->GetValue());
  }

  void RunPrint(SymbolTable &symbols) {
    logger << "Running print with children: " << child.size() << std::endl;
    for (auto children : child) {
      children->Run(symbols);
      children->PrintNode(std::cout);
    }
    std::cout << std::endl;
  }

  void RunExpression(SymbolTable &symbols) {
    logger << "Running expression" << std::endl;
    child.at(0)->Run(symbols);
    SetValue(child.at(0)->GetValue());
  }

  void RunVariable(SymbolTable &symbols) {
    logger << "Running variable" << std::endl;
    SetValue(symbols.GetValue(0, GetId()));
  }

  void PrintNode(std::ostream &stream) {
    stream << GetValue();
  };

public:
  ASTNode() {};
  ASTNode(Type type)
      : type{type} {};
  ASTNode(Type type, emplex::Token token)
      : type{type}, token(token) {};

  void AddChild(std::shared_ptr<ASTNode> node) {
    child.push_back(node);
  }

  double Run(SymbolTable &symbols) {
    if (GetType() == Type::EMPTY || GetType() == Type::STATEMENT_BLOCK) {
      logger << "Running type: " << GetType() << std::endl;
      for (auto children : child) {
        if (children != nullptr) {
          children->Run(symbols);
        }
      }
      return 1;
    }

    logger << "Running line: " << token.line_id << std::endl;

    switch (type) {
      case Type::PRINT:
        RunPrint(symbols);
        break;
      case Type::ASSIGN:
        RunAssign(symbols);
        break;
      case Type::EXPRESSION:
        RunExpression(symbols);
        break;
      case Type::VARIABLE:
        RunVariable(symbols);
        break;
      default:
        break;
    }

    return 1;
  }

  std::size_t GetId() const { return id; };
  double GetValue() const { return value; }
  ASTNode::Type GetType() const {
    return type;
  }
  void SetId(size_t newId) { id = newId; };
  void SetValue(double newValue) { value = newValue; };
};
