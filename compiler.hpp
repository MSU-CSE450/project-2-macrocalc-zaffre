
#pragma once

#include <assert.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Below are some suggestions on how you might want to divide up your project.
// You may delete this and divide it up however you like.
#include "ASTNode.hpp"
#include "SymbolTable.hpp"
#include "lexer.hpp"
#include "logger.hpp"

using namespace emplex;
class Compiler {
private:
  emplex::Lexer lexer{}; // Build the lexer object
  std::vector<emplex::Token> tokens;
  std::size_t current_token;
  SymbolTable table;
  std::shared_ptr<ASTNode> root = std::make_shared<ASTNode>(ASTNode(ASTNode::Type::STATEMENT_BLOCK));

  // == HELPER ==
  emplex::Token GetNext() {
    if ((current_token + 1) < tokens.size()) {
      return tokens[++current_token];
    }

    throw Err(tokens[current_token].line_id, "Unexpected end of file"); // TODO Fix line number
  }

  emplex::Token GetCurrent() {
    if (current_token < tokens.size()) {
      return tokens[current_token];
    }

    throw Err(tokens[current_token - 1].line_id, "Unexpected end of file"); // TODO Fix line number
    return emplex::Token{};
  }

  void MoveNext() {
    ++current_token;
  }

public:
  Compiler(std::ifstream &file_stream) {
    tokens = lexer.Tokenize(file_stream);
    logger << "Hello";
  }

  void parse() {
    // Push initial scope
    table.PushScope();
    // Parse and create the AST
    parseTokens(root, table.GetScopeCount());
  }

  void parseTokens(std::shared_ptr<ASTNode> currRoot, std::size_t scopeSizeBefore) {
    logger << "Started parsing token scope. Current scope: " << scopeSizeBefore << std::endl;
    while (current_token < tokens.size() && table.GetScopeCount() >= scopeSizeBefore) {
      currRoot->AddChild(ParseStatement());
    }

    logger << "Ended parsing token scope. Current scope: " << table.GetScopeCount() << std::endl;
    // After parsing, check if there are any open brackets
    std::size_t scopeSizeAfter = table.GetScopeCount();
    if (scopeSizeBefore - scopeSizeAfter > 1) {
      std::size_t tokenPost = current_token;
      if (current_token >= tokens.size())
        --tokenPost;
      emplex::Token lastToken = tokens.at(tokenPost);
      throw Err(lastToken.line_id, "Expected }");
    }
  }

  std::shared_ptr<ASTNode> ParseStatement() {
    logger << "Parsing " << emplex::Lexer::TokenName(tokens.at(current_token)) << " : " << tokens.at(current_token).lexeme << std::endl;
    switch (tokens.at(current_token)) {
      case Lexer::ID_VAR:
        return ParseVar();
      case Lexer::ID_PRINT:
        return ParsePrint();
      case Lexer::ID_ID:
        return ParseId();
      case Lexer::ID_OPEN_SCOPE:
        return ParseOpenScope();
      case Lexer::ID_CLOSE_SCOPE:
        return ParseCloseScope();
      default:
        return ParseExpression();
    }
  }

  std::shared_ptr<ASTNode> ParseVar() {
    auto assingToken = GetCurrent();
    auto varName = GetNext();
    if (varName != Lexer::ID_ID) {
      throw Err(varName.line_id, "'var' must be proceeded by variable name");
    }

    size_t varId = table.AddVar(varName.lexeme, varName.line_id);

    auto next = GetNext();
    if (next == Lexer::ID_END_OF_LINE) {
      return std::make_shared<ASTNode>(ASTNode{});
    } else if (next == Lexer::ID_ASSIGN) {
      auto expression = ParseExpression();
      auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::ASSIGN, assingToken));
      auto var = std::make_shared<ASTNode>(ASTNode(ASTNode::VARIABLE, varName));
      var->SetId(varId);

      node->AddChild(var);
      node->AddChild(expression);
      return node;
    } else {
      throw Err(varName.line_id, "Variable must be proceded by ; or =");
    }
  }

  std::shared_ptr<ASTNode> ParseExpression() {
    auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::VALUE, emplex::Token{}));
    node->SetValue(1);
    MoveNext();
    return node;
  }

  std::shared_ptr<ASTNode> ParsePrint() {
    auto printToken = GetCurrent();
    auto next = GetNext();
    if (next != '(') {
      throw Err(next.line_id, "Expected (");
    }

    auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::PRINT, printToken));
    MoveNext();
    auto expression = ParseExpression();
    node->AddChild(expression);
    return node;
  }

  std::shared_ptr<ASTNode> ParseId() {
    auto idToken = GetCurrent();
    auto assignToken = GetNext();
    if (assignToken != Lexer::ID_ASSIGN) {
      throw Err(assignToken.line_id, "Expected expression");
    }

    auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::ASSIGN, assignToken));

    auto var = std::make_shared<ASTNode>(ASTNode(ASTNode::VARIABLE, idToken));
    var->SetId(table.GetIdByName(idToken, idToken.lexeme));
    node->AddChild(var);

    MoveNext();
    auto expression = ParseExpression();
    node->AddChild(expression);
    return node;
  }

  std::shared_ptr<ASTNode> ParseOpenScope() {
    auto openToken = GetCurrent();
    auto statementBlockToken = std::make_shared<ASTNode>(ASTNode(ASTNode::Type::STATEMENT_BLOCK, openToken));

    table.PushScope();

    MoveNext();

    parseTokens(statementBlockToken, table.GetScopeCount());

    return statementBlockToken;
  }

  std::shared_ptr<ASTNode> ParseCloseScope() {
    table.PopScope();
    return std::make_shared<ASTNode>(ASTNode());
  }

  void execute() {
    root->Run(table);
  }
};