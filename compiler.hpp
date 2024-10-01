
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
  std::string TokenName(int id) const {
    if (id > 0 && id < 128) {
      return std::string("'") + static_cast<char>(id) + "'";
    }
    return emplex::Lexer::TokenName(id);
  }

  emplex::Token GetCurrent() {
    if (current_token < tokens.size()) {
      return tokens[current_token];
    }

    throw Err(tokens[current_token - 1].line_id, "Unexpected end of file"); // TODO Fix line number
    return emplex::Token{};
  }

  emplex::Token GetCurrent(int required_id, std::string err_message = "") {
    if (GetCurrent() != required_id) {
      if (err_message.size())
        throw Err(GetCurrent(), err_message);
      else {
        throw Err(GetCurrent(),
                  "Expected token type ", TokenName(required_id),
                  ", but found ", TokenName(GetCurrent()));
      }
    }
    return GetCurrent();
  }

  emplex::Token UseToken() { return tokens[current_token++]; }

  emplex::Token UseToken(int required_id, std::string err_message = "") {
    if (GetCurrent() != required_id) {
      if (err_message.size())
        throw Err(GetCurrent(), err_message);
      else {
        throw Err(GetCurrent(),
                  "Expected token type ", TokenName(required_id),
                  ", but found ", TokenName(GetCurrent()));
      }
    }
    return UseToken();
  }

  bool UseTokenIf(int test_id) {
    if (GetCurrent() == test_id) {
      current_token++;
      return true;
    }
    return false;
  }

  bool UseNextTokenIf(int test_id) {
    if (current_token + 1 < tokens.size() && tokens[current_token + 1] == test_id) {
      current_token++;
      return true;
    }
    return false;
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
      case Lexer::ID_END_OF_LINE:
      default:
        MoveNext();
        return std::make_shared<ASTNode>(ASTNode());
    }
  }

  std::shared_ptr<ASTNode> ParseVar() {
    auto assingToken = UseToken();
    auto varName = GetCurrent(Lexer::ID_ID, "'var' must be proceeded by variable name");

    size_t varId = table.AddVar(varName.lexeme, varName.line_id);

    if (UseNextTokenIf(Lexer::ID_END_OF_LINE)) {
      return std::make_shared<ASTNode>(ASTNode{});
    }

    return ParseId();
  }

  /**
   * Expect to be at the start of the expression
   */
  std::shared_ptr<ASTNode> ParseExpression() {
    logger << "Parsing expression" << std::endl;
    std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>(ASTNode(ASTNode::Type::EXPRESSION));

    auto term = ParseTerm();
    node->AddChild(term);

    return node;
  }

  std::shared_ptr<ASTNode> ParseTerm() {
    auto term = UseToken();
    logger << "Parsing term " << term.lexeme << std::endl;

    switch (term) {
      case Lexer::ID_ID: {
        auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::VARIABLE, term));
        node->SetId(table.GetIdByName(term.line_id, term.lexeme));
        return node;
      }
      case Lexer::ID_NUMBER: {
        auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::VALUE, term));
        node->SetValue(std::stod(term.lexeme));
        return node;
      }
      default:
        return std::make_shared<ASTNode>(ASTNode());
    }
  }

  std::shared_ptr<ASTNode> ParsePrint() {
    auto printToken = UseToken(Lexer::ID_PRINT);
    auto next = UseToken('(');
    auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::PRINT, printToken));
    do {
      auto expression = ParseExpression();
      node->AddChild(expression);
    } while (UseTokenIf(','));

    return node;
  }

  std::shared_ptr<ASTNode> ParseId() {
    auto idToken = UseToken(Lexer::ID_ID);
    auto assignToken = UseToken(Lexer::ID_ASSIGN);

    auto node = std::make_shared<ASTNode>(ASTNode(ASTNode::ASSIGN, assignToken));

    auto var = std::make_shared<ASTNode>(ASTNode(ASTNode::VARIABLE, idToken));
    var->SetId(table.GetIdByName(idToken, idToken.lexeme));
    node->AddChild(var);

    auto expression = ParseExpression();
    node->AddChild(expression);

    UseToken(Lexer::ID_END_OF_LINE);
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