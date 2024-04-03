#pragma once

#include <iostream>
#include <string_view>

#include "Token.hpp"
#include "RuntimeError.hpp"

inline bool hadError = false;
inline bool hadRuntimeError = false;

static void report(int line, std::string_view where, std::string_view message){
  std::cerr << "[Line " << line << "] Error - " << where << ": " << message << std::endl;
  hadError = true;

  return;
}

static void error(const Token& token, std::string_view message){
  if(token.type == TokenType::FILE_END){
    report(token.line, " at end ", message);
  }else{
    report(token.line, " at '" + token.lexeme + "'", message);
  }

  return;
}

static void error(int line, std::string_view message){
  report(line, std::string_view(""), message);

  return;
}

static void runtimeError(const RuntimeError& error){
  std::cerr << "[Line " << error.token.line << "]: " << error.what() << "\n";
  hadRuntimeError = true;

  return;
}