#pragma once

#include <any>

#include "Expr.hpp"

class Interpreter : public ExprVisitor{
  private:
    bool isTruthy(std::any object){
      if(object.type() == typeid(nullptr)) return false;
      if(object.type() == typeid(bool)) return std::any_cast<bool>(object);
      return true;
    }

    bool isEqual(std::any left, std::any right){
      if(left.type() == typeid(nullptr) && right.type() == typeid(nullptr)){
        return true;
      }
      if((left.type() == typeid(nullptr) && right.type() != typeid(nullptr)) || (left.type() != typeid(nullptr) && right.type() == typeid(nullptr))){
        return false;
      }
      if(left.type() == typeid(bool) && right.type() == typeid(bool)){
        return std::any_cast<bool>(left) == std::any_cast<bool>(right);
      }
      if(left.type() == typeid(double) && right.type() == typeid(double)){
        return std::any_cast<double>(left) == std::any_cast<double>(right);
      }
      if(left.type() == typeid(std::string) && right.type() == typeid(std::string)){
        return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
      }

      return false;
    }

    std::any evaluate(std::shared_ptr<Expr> expr){
      return expr->accept(*this);
    }
  public:
    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override{
      std::any left = evaluate(expr->left);
      std::any right = evaluate(expr->right);

      switch(expr->op.type){
        case TokenType::PLUS:
          if(left.type() == typeid(double) && right.type() == typeid(double)){
            return std::any_cast<double>(left) + std::any_cast<double>(right);
          }
          if(left.type() == typeid(std::string) && right.type() == typeid(std::string)){
            return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
          }
          break;
        case TokenType::MINUS:
          return std::any_cast<double>(left) - std::any_cast<double>(right);
        case TokenType::STAR:
          return std::any_cast<double>(left) * std::any_cast<double>(right);
        case TokenType::SLASH:
          return std::any_cast<double>(left) / std::any_cast<double>(right);
        case TokenType::GREATER:
          return std::any_cast<double>(left) > std::any_cast<double>(right);
        case TokenType::GREATER_EQUAL:
          return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case TokenType::LESS:
          return std::any_cast<double>(left) < std::any_cast<double>(right);
        case TokenType::LESS_EQUAL:
          return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case TokenType::BANG_EQUAL:
          return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
          return isEqual(left, right);
      }
    }

    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override{
      std::any right = evaluate(expr->right);

      switch(expr->op.type){
        case TokenType::BANG:
          return !isTruthy(right);
        case TokenType::MINUS:
          return -std::any_cast<double>(right);
      }

      // Unreachable
      return {};
    }

    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override{
      return expr->value;
    }

    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override{
      return evaluate(expr->expression);
    }
};