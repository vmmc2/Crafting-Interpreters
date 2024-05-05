#include <utility>

#include "LoxClass.hpp"

LoxClass::LoxClass(std::string name)
  : name{std::move(name)}
{}

int LoxClass::arity(){
  return 0;
}

std::any LoxClass::call(Interpreter& interpreter, std::vector<std::any> arguments){
  auto instance = std::make_shared<LoxInstance>(shared_from_this());
  
  return instance;
}

std::string LoxClass::toString(){
  return name;
}
