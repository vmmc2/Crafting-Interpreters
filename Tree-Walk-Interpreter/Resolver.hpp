#pragma once

#include <map>
#include <memory>
#include <vector>
#include <functional>

#include "Interpreter.hpp"

class Resolver : public ExprVisitor, public StmtVisitor{
  private:
    Interpreter& interpreter;
    std::vector<std::map<std::string, bool>> scopes;

    void resolve(std::shared_ptr<Stmt> stmt){
      // Very similar to the "execute" method from the Interpreter class.
      stmt->accept(*this);

      return;
    }

    void resolve(std::shared_ptr<Expr> expr){
      // Very similar to the "evaluate" method from the Interpreter class.
      expr->accept(*this);

      return;
    }

    void beginScope(){
      scopes.push_back(std::map<std::string, bool>{});

      return;
    }

    void endScope(){
      scopes.pop_back();

      return;
    }

  public:
    Resolver(Interpreter& interpreter)
      : interpreter{interpreter}
    {}

    void resolve(const std::vector<std::shared_ptr<Stmt>>& statements){
      for(const std::shared_ptr<Stmt>& statement : statements){
        resolve(statement);
      }
      return;
    }

    std::any visitBlockStmt(std::shared_ptr<Block> stmt) override{
      // This begins a new scope, 
      // traverses into the statements inside the block, 
      // and then discards the scope.
      beginScope();
      resolve(stmt->statements);
      endScope();

      return {};
    }

    std::any visitVarStmt(std::shared_ptr<Var> stmt) override{
      declare(stmt->name);
      if(stmt->initializer != nullptr){
        resolve(stmt->initializer);
      }
      define(stmt->name);

      return {};
    }
};