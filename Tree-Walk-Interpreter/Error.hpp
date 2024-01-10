#pragma once

#include <iostream>
#include <string_view>

#include "TokenType.hpp"

inline bool hadError = false;

static void report(int line, std::string_view where, std::string_view message){
  std::cerr << "[Line " << line << "] Error - " << where << ": " << message << std::endl;
  hadError = true;
}

static void error(int line, std::string_view message){
  report(line, std::string_view(""), message);
}

static void error(Token token, std::string_view message){
  if(token.type == TokenType::FILE_END){
    report(token.line, " at end", message);
  }else{
    report(token.line, "at '" + token.lexeme + "'", message);
  }
}