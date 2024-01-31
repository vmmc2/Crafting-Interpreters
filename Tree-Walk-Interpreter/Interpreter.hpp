#pragma once

#include <any>
#include <iostream>

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Error.hpp"
#include "RuntimeError.hpp"

class Interpreter : public ExprVisitor, public StmtVisitor{
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

    std::string stringify(std::any object){
      if(object.type() == typeid(nullptr)) return "nil";

      if(object.type() == typeid(double)){
        std::string text = std::to_string(std::any_cast<double>(object));
        int textLength = text.length();

        if(text[textLength - 2] == '.' && text[textLength - 1] == '0'){
          text = text.substr(0, textLength - 2);
        }

        return text;
      }

      if(object.type() == typeid(std::string)) return std::any_cast<std::string>(object);

      if(object.type() == typeid(bool)){
        return std::any_cast<bool>(object) ? "true" : "false";
      }

      return "Error in stringify: object type not recognized.";
    }

    std::any evaluate(std::shared_ptr<Expr> expr){
      return expr->accept(*this);
    }

    void execute(std::shared_ptr<Stmt> stmt){
      stmt->accept(*this);
      
      return;
    }
  
  public:
    std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override{
      evaluate(stmt->expression);

      return {};
    }

    std::any visitPrintStmt(std::shared_ptr<Print> stmt) override{
      std::any expr = evaluate(stmt->expression);
      std::cout << stringify(expr) << std::endl;
      return {};
    }

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

          throw RuntimeError{expr->op, "Operands must be either two numbers or two strings."};
        case TokenType::MINUS:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) - std::any_cast<double>(right);
        case TokenType::STAR:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) * std::any_cast<double>(right);
        case TokenType::SLASH:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) / std::any_cast<double>(right);
        case TokenType::GREATER:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) > std::any_cast<double>(right);
        case TokenType::GREATER_EQUAL:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case TokenType::LESS:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) < std::any_cast<double>(right);
        case TokenType::LESS_EQUAL:
          checkNumberOperands(expr->op, left, right);
          return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case TokenType::BANG_EQUAL:
          return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
          return isEqual(left, right);
      }
      
      // Unreachable
      return {};
    }

    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override{
      std::any right = evaluate(expr->right);

      switch(expr->op.type){
        case TokenType::BANG:
          return !isTruthy(right);
        case TokenType::MINUS:
          checkNumberOperand(expr->op, right);
          return -std::any_cast<double>(right);
      }

      // Unreachable
      return {};
    }

    void checkNumberOperand(Token op, std::any operand){
      if(operand.type() == typeid(double)) return;
      throw RuntimeError{op, "Operand must be a number."};
    }

    void checkNumberOperands(Token op, std::any left, std::any right){
      if(left.type() == typeid(double) && right.type() == typeid(double)) return;
      throw RuntimeError{op, "Operands must be both numbers"};
    }

    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override{
      return expr->value;
    }

    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override{
      return evaluate(expr->expression);
    }

    void interpret(std::vector<std::shared_ptr<Stmt>> statements){
      try{
        for(std::shared_ptr<Stmt> statement : statements){
          execute(statement);
        }
      }catch(RuntimeError error){
        runtimeError(error);
      }
    }
};