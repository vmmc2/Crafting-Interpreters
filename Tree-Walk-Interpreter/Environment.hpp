#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <functional>

#include "Error.hpp"
#include "Token.hpp"

class Environment : public std::enable_shared_from_this<Environment>{
  private:
    friend class Interpreter;
    
    std::shared_ptr<Environment> enclosing;
    std::map<std::string, std::any> values;

  public:
    Environment() // Constructor for the Global Environment (There's no enclosing environment).
      : enclosing{nullptr}
    {}

    Environment(std::shared_ptr<Environment> enclosing) // Constructor for any non-global Environment that might receive an enclosing environment.
      : enclosing{std::move(enclosing)}
    {}

    void define(const std::string& name, std::any value){ // A new variable is always declared in the current innermost scope.
      values[name] = std::move(value);

      return;
    }

    std::shared_ptr<Environment> ancestor(int distance){
      std::shared_ptr<Environment> environment = shared_from_this();
      for(int i = 0; i < distance; i++){
        environment = environment->enclosing;
      }

      return environment;
    }

    void assign(const Token& name, std::any value){
      auto elem = values.find(name.lexeme);
      if(elem != values.end()){
        elem->second = std::move(value);
        return;
      }

      if(enclosing != nullptr){ // If a name was not found in the current scope. Try looking for it in the enclosing/outer scope it we reach the Global scope.
        enclosing->assign(name, std::move(value));
        return;
      }

      // If the variable hasn't already been declared (does not exist inside the environment map), then we cannot assign a new value to it.
      throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'."); 
    }

    std::any get(const Token& name){
      if(values.find(name.lexeme) != values.end()){
        return values[name.lexeme];
      }

      if(enclosing != nullptr){ // If a name was not found in the current scope. Try looking for it in the enclosing/outer scope it we reach the Global scope.
        return enclosing->get(name);
      }

      // If the variable hasn't already been declared (does not exist inside the environment map), then we cannot get its value.
      throw RuntimeError(name, "Undefined variable: '" + name.lexeme + "'.");
    }

    void assignAt(int distance, const Token& name, std::any value){
      ancestor(distance)->values[name.lexeme] = std::move(value);

      return;
    }

    std::any getAt(int distance, const std::string& name){
      return ancestor(distance)->values[name];
    }
};