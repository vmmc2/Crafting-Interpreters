#include <utility>

#include "LoxClass.hpp"

LoxClass::LoxClass(std::string name)
  : name{std::move(name)}
{}

std::string LoxClass::toString(){
  return name;
}
