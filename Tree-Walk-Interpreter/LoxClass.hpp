#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "LoxCallable.hpp"

class Interpreter;
class LoxFunction;

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass>{
  private:
    friend class LoxInstance;
    const std::string name;
  public:
    LoxClass(std::string name);
    int arity() override;
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;
    std::string toString() override;
};