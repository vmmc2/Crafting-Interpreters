#include <utility>

#include "LoxClass.hpp"

LoxClass::LoxClass(std::string name, std::map<std::string, std::shared_ptr<LoxFunction>> methods)
  : name{std::move(name)}, methods{std::move(methods)}
{}

int LoxClass::arity(){
  std::shared_ptr<LoxFunction> initializer = findMethod("init");
  if(initializer == nullptr){
    return 0;
  }

  return initializer->arity();
}

std::any LoxClass::call(Interpreter& interpreter, std::vector<std::any> arguments){
  auto instance = std::make_shared<LoxInstance>(shared_from_this());

  std::shared_ptr<LoxFunction> initializer = findMethod("init");
  if(initializer != nullptr){
    initializer->bind(instance)->call(interpreter, std::move(arguments));
  }
  
  return instance;
}

std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string& name){
  auto elem = methods.find(name);
  if(elem != methods.end()){
    return elem->second;
  }

  return nullptr;
}

std::string LoxClass::toString(){
  return name;
}
