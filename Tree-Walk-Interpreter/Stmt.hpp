#pragma once

#include <any>
#include <memory>
#include <utility>
#include <vector>

#include "Expr.hpp"
#include "Token.hpp"

struct Block;
struct Class;
struct Expression;
struct Function;
struct If;
struct Print;
struct Return;
struct Var;
struct While;

struct StmtVisitor{
  virtual std::any visitBlockStmt(std::shared_ptr<Block> stmt) = 0;
  virtual std::any visitClassStmt(std::shared_ptr<Class> stmt) = 0;
  virtual std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) = 0;
  virtual std::any visitFunctionStmt(std::shared_ptr<Function> stmt) = 0;
  virtual std::any visitIfStmt(std::shared_ptr<If> stmt) = 0;
  virtual std::any visitPrintStmt(std::shared_ptr<Print> stmt) = 0;
  virtual std::any visitReturnStmt(std::shared_ptr<Return> stmt) = 0;
  virtual std::any visitVarStmt(std::shared_ptr<Var> stmt) = 0;
  virtual std::any visitWhileStmt(std::shared_ptr<While> stmt) = 0;
  virtual ~StmtVisitor() = default;
};

struct Stmt{
  virtual std::any accept(StmtVisitor& visitor) = 0;
};

struct Block : Stmt, public std::enable_shared_from_this<Block>{
  const std::vector<std::shared_ptr<Stmt>> statements;

  Block(std::vector<std::shared_ptr<Stmt>> statements)
    : statements{std::move(statements)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitBlockStmt(shared_from_this());
  }
};

struct Class : Stmt, public std::enable_shared_from_this<Class>{
  const Token name;
  const std::vector<std::shared_ptr<Function>> methods;

  Class(Token name, std::vector<std::shared_ptr<Function>> methods)
    : name{std::move(name)}, methods{std::move(methods)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitClassStmt(shared_from_this());
  }
};

struct Expression : Stmt, public std::enable_shared_from_this<Expression>{
  const std::shared_ptr<Expr> expression;

  Expression(std::shared_ptr<Expr> expression)
    : expression{std::move(expression)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitExpressionStmt(shared_from_this());
  }
};

struct Function : Stmt, public std::enable_shared_from_this<Function>{
  const Token name;
  const std::vector<Token> parameters;
  const std::vector<std::shared_ptr<Stmt>> body;

  Function(Token name, std::vector<Token> parameters, std::vector<std::shared_ptr<Stmt>> body)
    : name{std::move(name)}, parameters{std::move(parameters)}, body{std::move(body)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitFunctionStmt(shared_from_this());
  }
};

struct If : Stmt, public std::enable_shared_from_this<If>{
  std::shared_ptr<Expr> condition;
  std::shared_ptr<Stmt> ifBranch;
  std::shared_ptr<Stmt> elseBranch;

  If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> ifBranch, std::shared_ptr<Stmt> elseBranch)
    : condition{std::move(condition)}, ifBranch{std::move(ifBranch)}, elseBranch{std::move(elseBranch)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitIfStmt(shared_from_this());
  }
};

struct Print : Stmt, public std::enable_shared_from_this<Print>{
  const std::shared_ptr<Expr> expression;

  Print(std::shared_ptr<Expr> expression)
    : expression{std::move(expression)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitPrintStmt(shared_from_this());
  }
};

struct Return : Stmt, public std::enable_shared_from_this<Return>{
  const Token keyword;
  const std::shared_ptr<Expr> value;

  Return(Token keyword, std::shared_ptr<Expr> value)
    : keyword{std::move(keyword)}, value{std::move(value)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitReturnStmt(shared_from_this());
  }
};

struct Var : Stmt, public std::enable_shared_from_this<Var>{
  const Token name;
  const std::shared_ptr<Expr> initializer;

  Var(Token name, std::shared_ptr<Expr> initializer)
    : name{std::move(name)}, initializer{std::move(initializer)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitVarStmt(shared_from_this());
  }
};

struct While : Stmt, public std::enable_shared_from_this<While>{
  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Stmt> body;

  While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
    : condition{std::move(condition)}, body{std::move(body)}
  {}

  std::any accept(StmtVisitor& visitor) override{
    return visitor.visitWhileStmt(shared_from_this());
  }
};