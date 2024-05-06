#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>

class LoxClass;
class Token;

class LoxInstance: public std::enable_shared_from_this<LoxInstance> {
  private:
    std::shared_ptr<LoxClass> klass;
    std::map<std::string, std::any> fields;

  public:
    LoxInstance(std::shared_ptr<LoxClass> klass);
    std::any get(const Token& name);
    std::string toString();
};