#include <utility>

#include "Environment.hpp"
#include "Interpreter.hpp"
#include "LoxFunction.hpp"
#include "Stmt.hpp"

LoxFunction::LoxFunction(std::shared_ptr<Function> declaration)
  : declaration{std::move(declaration)}
{}

std::string LoxFunction::toString(){
  return "<fun " + declaration->name.lexeme + ">";
}

int LoxFunction::arity(){
  return declaration->parameters.size();
}

std::any LoxFunction::call(Interpreter& interpreter, std::vector<std::any> arguments){
  auto environment = std::make_shared<Environment>(interpreter.globals); // Create the current local environment of the LoxFunction.

  for(int i = 0; i < declaration->parameters.size(); i++){ // Execute the binding of the parameters of the LoxFunction to its respective arguments.
    environment->define(declaration->parameters[i].lexeme, arguments[i]);
  }
  interpreter.executeBlock(declaration->body, environment); // Execute the body of the funtion by passing its statements and its current environment.

  return nullptr;
}