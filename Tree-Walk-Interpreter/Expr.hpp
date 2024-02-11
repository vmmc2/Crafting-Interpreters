#pragma once

#include <any>
#include <memory>
#include <vector>
#include <utility>

#include "Token.hpp"

struct Expr;

struct Assign;
struct Binary;
struct Unary;
struct Literal;
struct Grouping;
struct Variable;

struct ExprVisitor{
  virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr) = 0;
  virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
  virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
  virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
  virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
  virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;
  virtual ~ExprVisitor() = default;
};

struct Expr{
  virtual std::any accept(ExprVisitor& visitor) = 0;
};

struct Assign : Expr, public std::enable_shared_from_this<Assign>{
  const Token name; // L-value (Evaluates to a location in memory to which we can assign the value to).
  const std::shared_ptr<Expr> value; // R-value (Expression that evaluates to a value).

  Assign(Token name, std::shared_ptr<Expr> value)
    : name{std::move(name)}, value{std::move(value)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitAssignExpr(shared_from_this());
  }
};

struct Binary : Expr, public std::enable_shared_from_this<Binary>{
  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;

  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right) 
    : left{std::move(left)}, op{std::move(op)}, right{std::move(right)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitBinaryExpr(shared_from_this());
  }
};

struct Unary : Expr, public std::enable_shared_from_this<Unary>{
  const Token op;
  const std::shared_ptr<Expr> right;

  Unary(Token op, std::shared_ptr<Expr> right)
    : op{std::move(op)}, right{std::move(right)} 
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitUnaryExpr(shared_from_this());
  }
};

struct Variable : Expr, public std::enable_shared_from_this<Variable>{
  const Token name;

  Variable(Token name)
    : name{std::move(name)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitVariableExpr(shared_from_this());  
  }
};

struct Literal : Expr, public std::enable_shared_from_this<Literal>{
  const std::any value;

  Literal(std::any value)
    : value{std::move(value)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitLiteralExpr(shared_from_this());
  }
};

struct Grouping : Expr, std::enable_shared_from_this<Grouping>{
  const std::shared_ptr<Expr> expression;

  Grouping(std::shared_ptr<Expr> expression)
    : expression{std::move(expression)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitGroupingExpr(shared_from_this());
  }
};
