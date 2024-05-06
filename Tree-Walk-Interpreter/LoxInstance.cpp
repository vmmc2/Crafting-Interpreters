#include <utility>

#include "Error.hpp"
#include "LoxInstance.hpp"

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass)
  : klass{std::move(klass)}
{}

std::any LoxInstance::get(const Token& name){
  auto elem = fields.find(name.lexeme);
  if(elem != fields.end()){
    return elem->second;
  }

  throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

std::string LoxInstance::toString(){
  return klass->name + " instance";
}