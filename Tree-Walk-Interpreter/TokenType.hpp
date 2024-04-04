#pragma once

#include <string>

enum TokenType{
  // Single-Character Tokens
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, PLUS, MINUS, STAR, SLASH, SEMICOLON,

  // One or Two Character Tokens
  BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

  // Literals
  IDENTIFIER, STRING, NUMBER,

  // Keywords + End of File
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
  FILE_END
};

std::string toString(TokenType type){
  static const std::string strings[] = {
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
    "COMMA", "DOT", "PLUS", "MINUS", "STAR", "SLASH", "SEMICOLON",
    "BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL",
    "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL",
    "IDENTIFIER", "STRING", "NUMBER",
    "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
    "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
    "FILE_END"
  };

  return strings[static_cast<int>(type)];
}

