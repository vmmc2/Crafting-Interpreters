#include <utility>

#include "Error.hpp"
#include "LoxInstance.hpp"

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass)
  : klass{std::move(klass)}
{}

std::string LoxInstance::toString() {
  return klass->name + " instance";
}