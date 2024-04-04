#include <utility>

#include "Stmt.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "LoxFunction.hpp"

LoxFunction::LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure)
  : declaration{std::move(declaration)}, closure{std::move(closure)}
{}

std::string LoxFunction::toString(){
  return "<fun " + declaration->name.lexeme + ">"; // This method is responsible for print the function value (not the function call).
}

int LoxFunction::arity(){
  return declaration->parameters.size();
}

std::any LoxFunction::call(Interpreter& interpreter, std::vector<std::any> arguments){
  auto environment = std::make_shared<Environment>(closure); // Create the current local environment of the LoxFunction.

  for(int i = 0; i < declaration->parameters.size(); i++){ // Execute the binding of the parameters of the LoxFunction to its respective arguments.
    environment->define(declaration->parameters[i].lexeme, arguments[i]);
  }

  try{
    interpreter.executeBlock(declaration->body, environment); // Execute the body of the funtion by passing its statements and its current environment.
  }catch(LoxReturn returnValue){
    return returnValue.value;
  }

  return nullptr; // Automatically deals with the case where there is no 'return' statement in the body of the function. By default, in these cases, Lox functions return nil (nullptr).
}