#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include "Expr.hpp"

class AstPrinter: public ExprVisitor{
  public:
    std::string print(std::shared_ptr<Expr> expr){
      return std::any_cast<std::string>(expr->accept(*this));
    }
};
