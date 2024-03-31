#pragma once

#include <any>
#include <chrono>
#include <iostream>

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Error.hpp"
#include "Environment.hpp"
#include "LoxCallable.hpp"
#include "LoxFunction.hpp"
#include "RuntimeError.hpp"

class NativeClock : public LoxCallable {
  int arity() override{
    return 0;
  }

  std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override{
    auto ticks = std::chrono::system_clock::now().time_since_epoch();
    auto timeElapsedInMili = std::chrono::duration<double>{ticks}.count() / 1000.0;

    return timeElapsedInMili;
  }

  std::string toString() override{
    return "<native fun>";
  }
};

class Interpreter : public ExprVisitor, public StmtVisitor{
  private:
    // The variables stay in memory as long as the interpreter is still running.
    std::shared_ptr<Environment> environment = globals;

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
    std::shared_ptr<Environment> globals{ new Environment };

    Interpreter(){
      globals->define("clock", std::shared_ptr<NativeClock>{});
    }

    std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override{
      evaluate(stmt->expression);

      return {};
    }

    std::any visitFunctionStmt(std::shared_ptr<Function> stmt) override{
      std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(stmt);
      environment->define(stmt->name.lexeme, function);

      return nullptr;
    }

    void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment){
      std::shared_ptr<Environment> previous = this->environment;

      try{
        this->environment = environment;
        for(const std::shared_ptr<Stmt>& statement : statements){
          execute(statement);
        }
      }catch(...){
        this->environment = previous;
        throw;
      }

      this->environment = previous;

      return;
    }

    std::any visitIfStmt(std::shared_ptr<If> stmt) override{
      if(isTruthy(evaluate(stmt->condition))){
        execute(stmt->ifBranch);
      }else if(stmt->elseBranch != nullptr){
        execute(stmt->elseBranch);
      }

      return {};
    }

    std::any visitPrintStmt(std::shared_ptr<Print> stmt) override{
      std::any expr = evaluate(stmt->expression);
      std::cout << stringify(expr) << std::endl;
      return {};
    }

    std::any visitBlockStmt(std::shared_ptr<Block> stmt) override{
      executeBlock(stmt->statements, std::make_shared<Environment>(environment));

      return {};
    }

    std::any visitVarStmt(std::shared_ptr<Var> stmt) override{
      // We assume that the variable declaration statement doesn't assign any value to the variable: "var a;"
      // In this approach, the default declared variable without an initializer has the "nil" value.
      std::any value = nullptr;
      if(stmt->initializer != nullptr){ // We have a value to assign to the variable that's being declared: "var a = 5;"
        value = evaluate(stmt->initializer);
      }

      environment->define(stmt->name.lexeme, std::move(value));

      return {};
    }

    std::any visitWhileStmt(std::shared_ptr<While> stmt) override{
      while(isTruthy(evaluate(stmt->condition))){
        execute(stmt->body);
      }

      return {};
    }

    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override{
      std::any value = evaluate(expr->value);
      environment->assign(expr->name, value);

      return value;
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

    std::any visitCallExpr(std::shared_ptr<Call> expr) override{
      // We need to verify whether the callee is valid or not (This is done through evaluation).
      std::any callee = evaluate(expr->callee);

      std::vector<std::any> arguments;
      for(const std::shared_ptr<Expr>& argument : expr->arguments){
        arguments.push_back(evaluate(argument));
      }

      std::shared_ptr<LoxCallable> function;

      if(callee.type() == typeid(std::shared_ptr<LoxFunction>)){
        function = std::any_cast<std::shared_ptr<LoxFunction>>(callee); // Isso eh msm um erro??
      }else{
        throw RuntimeError{expr->paren, "Can only call functions and classes."};
      }

      if(arguments.size() != function->arity()){
        throw RuntimeError{expr->paren, "Expected " + std::to_string(function->arity()) + " arguments, but received " + std::to_string(arguments.size()) + "."};
      }

      return function->call(*this, std::move(arguments));
    }

    std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override{
      std::any left = evaluate(expr->left);

      if(expr->op.type == TokenType::OR){
        if(isTruthy(left)) return left; // Short-Circuit from left to right (left-associative).
      }else if(expr->op.type == TokenType::AND){
        if(!isTruthy(left)) return left; // Short-Circuit from left to right (left-associative).
      }

      return evaluate(expr->right);
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

    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override{
      return environment->get(expr->name);
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