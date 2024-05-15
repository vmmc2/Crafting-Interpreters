#pragma once

#include <any>
#include <memory>
#include <vector>
#include <utility>

#include "Token.hpp"

struct Assign;
struct Binary;
struct Call;
struct Get;
struct Grouping;
struct Literal;
struct Logical;
struct Set;
struct Super;
struct This;
struct Unary;
struct Variable;

struct ExprVisitor{
  virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr) = 0;
  virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
  virtual std::any visitCallExpr(std::shared_ptr<Call> expr) = 0;
  virtual std::any visitGetExpr(std::shared_ptr<Get> expr) = 0;
  virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
  virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
  virtual std::any visitLogicalExpr(std::shared_ptr<Logical> expr) = 0;
  virtual std::any visitSetExpr(std::shared_ptr<Set> expr) = 0;
  virtual std::any visitSuperExpr(std::shared_ptr<Super> expr) = 0;
  virtual std::any visitThisExpr(std::shared_ptr<This> expr) = 0;
  virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
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

struct Call : Expr, public std::enable_shared_from_this<Call>{
  const std::shared_ptr<Expr> callee;
  const Token paren;
  const std::vector<std::shared_ptr<Expr>> arguments;

  Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
    : callee{std::move(callee)}, paren{std::move(paren)}, arguments{std::move(arguments)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitCallExpr(shared_from_this());
  }
};

struct Get : Expr, public std::enable_shared_from_this<Get>{
  const Token name;
  const std::shared_ptr<Expr> object;

  Get(Token name, std::shared_ptr<Expr> object)
    : name{std::move(name)}, object{std::move(object)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitGetExpr(shared_from_this());
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

struct Literal : Expr, public std::enable_shared_from_this<Literal>{
  const std::any value;

  Literal(std::any value)
    : value{std::move(value)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitLiteralExpr(shared_from_this());
  }
};

struct Logical : Expr, public std::enable_shared_from_this<Logical>{
  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;

  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
    : left{std::move(left)}, op{std::move(op)}, right{std::move(right)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitLogicalExpr(shared_from_this());
  }
};

struct Set : Expr, public std::enable_shared_from_this<Set>{
  const std::shared_ptr<Expr> object;
  const Token name;
  const std::shared_ptr<Expr> value;

  Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
    : object{std::move(object)}, name{std::move(name)}, value{std::move(value)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitSetExpr(shared_from_this());
  }

};

struct Super : Expr, public std::enable_shared_from_this<Super>{
  const Token keyword;
  const Token method;

  Super(Token keyword, Token method)
    : keyword{std::move(keyword)}, method{std::move(method)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitSuperExpr(shared_from_this());
  }
};

struct This : Expr, public std::enable_shared_from_this<This>{
  const Token keyword;

  This(Token keyword)
    : keyword{std::move(keyword)}
  {}

  std::any accept(ExprVisitor& visitor) override{
    return visitor.visitThisExpr(shared_from_this());
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
