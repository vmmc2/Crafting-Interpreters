#pragma once

#include <any>
#include <map>
#include <string>
#include <memory>

class Environment : public std::enable_shared_from_this<Environment>{
  private:
    std::map<std::string, std::any> values;
  public:
    void define(std::string name, std::any value){
      values[name] = std::move(value);

      return;
    }
};