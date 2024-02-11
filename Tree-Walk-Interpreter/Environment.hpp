#pragma once

#include <any>
#include <map>
#include <string>
#include <memory>

#include "Error.hpp"
#include "Token.hpp"

class Environment : public std::enable_shared_from_this<Environment>{
  private:
    std::shared_ptr<Environment> enclosing;
    std::map<std::string, std::any> values;
  public:
    Environment() // Constructor for the Global Environment (There's no enclosing environment).
    : enclosing{nullptr}
    {}

    Environment(std::shared_ptr<Environment> enclosing) // Constructor for any non-global Environment that might receive an enclosing environment.
    : enclosing{enclosing}
    {}

    void define(std::string name, std::any value){ // A new variable is always declared in the current innermost scope.
      values[name] = std::move(value);

      return;
    }

    void assign(Token name, std::any value){
      auto elem = values.find(name.lexeme);
      if(elem != values.end()){
        elem->second = std::move(value);
        return;
      }

      if(enclosing != nullptr){ // If a name was not found in the current scope. Try looking for it in the enclosing/outer scope it we reach the Global scope.
        enclosing->assign(name, value);
        return;
      }

      // If the variable hasn't already been declared (does not exist inside the environment map), then we cannot assign a new value to it.
      throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'."); 
    }

    std::any get(Token name){
      if(values.find(name.lexeme) != values.end()){
        return values[name.lexeme];
      }

      if(enclosing != nullptr){ // If a name was not found in the current scope. Try looking for it in the enclosing/outer scope it we reach the Global scope.
        return enclosing->get(name);
      }

      // If the variable hasn't already been declared (does not exist inside the environment map), then we cannot get its value.
      throw RuntimeError(name,"Undefined variable: '" + name.lexeme + "'.");
    }
};