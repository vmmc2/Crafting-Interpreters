#pragma once

#include <any>
#include <string>
#include <utility>

#include "TokenType.hpp"

class Token{
  public:
    const int line;
    const TokenType type;
    const std::any literal;
    const std::string lexeme;

    Token(int line, TokenType type, std::any literal, std::string lexeme)
      : line{line}, type{type}, literal{std::move(literal)}, lexeme{std::move(lexeme)}
    {}

    std::string toString() const{
      std::string literal_text;

      switch(type){
        case (TokenType::IDENTIFIER):
          literal_text = lexeme;
          break;
        case (TokenType::STRING):
          literal_text = std::any_cast<std::string>(literal);
          break;
        case (TokenType::NUMBER):
          literal_text = std::to_string(std::any_cast<double>(literal));
          break;
        case (TokenType::TRUE):
          literal_text = "true";
          break;
        case (TokenType::FALSE):
          literal_text = "false";
          break;
        default:
          literal_text = "nil";
          break;
      }

      return ::toString(type) + " " + lexeme + " " + literal_text;
    }
};