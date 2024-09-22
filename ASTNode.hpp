#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "SymbolTable.hpp"

class ASTNode {
private:
  // PLACE AST NODE INFO HERE.
  
public:
  // CONSTRUCTORS, ETC HERE.
  // CAN SPECIFY NODE TYPE AND ANY NEEDED VALUES HERE OR USING OTHER FUNCTIONS.

  // CODE TO ADD CHILDREN AND SETUP AST NODE HERE.
  void AddChild(ASTNode node) { ; }
  
  // CODE TO EXECUTE THIS NODE (AND ITS CHILDREN, AS NEEDED).
  double Run(SymbolTable & symbols) { ; }

};
