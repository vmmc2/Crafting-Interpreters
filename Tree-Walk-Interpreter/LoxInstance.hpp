#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>

class LoxClass;
class Token;

class LoxInstance: public std::enable_shared_from_this<LoxInstance> {
  std::shared_ptr<LoxClass> klass;

public:
  LoxInstance(std::shared_ptr<LoxClass> klass);
  std::string toString();
};