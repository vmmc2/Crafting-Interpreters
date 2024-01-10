#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include "Expr.hpp"

class RPNPrinter : public ExprVisitor{
  private:
    template <class... E>
    std::string rpn(std::string_view name, E... expr){
      assert((... && std::is_same_v<E, std::shared_ptr<Expr>>));

      std::ostringstream builder;

      builder << "(";
      ((builder << print(expr) << " "), ...);
      builder << name << ")";

      return builder.str();
    }
  public:
    std::string print(std::shared_ptr<Expr> expr){
      return std::any_cast<std::string>(expr->accept(*this));
    }

    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override{
      return rpn(expr->op.lexeme, expr->left, expr->right);
    }

    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override{
      return rpn(expr->op.lexeme, expr->right);
    }

    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override{
      auto& expr_type = expr->value.type();

      if(expr_type == typeid(nullptr)){
        return "nil";
      }else if(expr_type == typeid(std::string)){
        return std::any_cast<std::string>(expr->value);
      }else if(expr_type == typeid(double)){
        return std::to_string(std::any_cast<double>(expr->value));
      }else if(expr_type == typeid(bool)){
        return std::any_cast<bool>(expr->value) ? "true" : "false";
      }

      return "Error in visitLiteralExpr: Literal type not recognized.";
    }

    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override{
      return rpn("grouping", expr->expression);
    }
};