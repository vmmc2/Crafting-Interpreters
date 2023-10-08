#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <string_view>

#include "Error.hpp"
#include "Token.hpp"

class Scanner{
    public:
        int line = 1;
        int start = 0;
        int current = 0;
        std::string source;
        std::vector<Token> tokens;

        Scanner(std::string source) : source(std::move(source)) {}

        std::vector<Token> scanTokens(){
            while(!isAtEnd()){
                // We are at the beginning of the next lexeme.
                start = current;
                scanToken();
            }

            tokens.push_back(Token(line, TokenType::EOF, nullptr, ""));
            return tokens;
        }

        bool isAtEnd(){
            return current >= source.length();
        }
}