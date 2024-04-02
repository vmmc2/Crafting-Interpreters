#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "Interpreter.hpp"
#include "LoxCallable.hpp"

class Environment;
class Function;

class LoxFunction : LoxCallable{
  private:
    std::shared_ptr<Function> declaration;
    std::shared_ptr<Environment> closure;

  public:
    LoxFunction(std::shared_ptr<Function> declaration);

    std::string toString() override;
    int arity() override;
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;
};