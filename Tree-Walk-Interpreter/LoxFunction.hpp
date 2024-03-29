#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "LoxCallable.hpp"

class Environment;
class Function;

class LoxFunction : LoxCallable{
  private:
    std::shared_ptr<Function> declaration;

  public:
    LoxFunction(std::shared_ptr<Function> declaration);

    std::string toString() override;
    int arity() override;
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;
};