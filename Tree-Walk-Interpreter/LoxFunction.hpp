#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "LoxCallable.hpp"

class Environment;
class Function;
class LoxInstance;

class LoxFunction : public LoxCallable{
  private:
    bool isInitializer;
    std::shared_ptr<Function> declaration;
    std::shared_ptr<Environment> closure;

  public:
    LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure, bool isInitializer);
    int arity() override;
    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;
    std::string toString() override;
};