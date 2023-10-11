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
                // In each iteration/turn of the loop, a single token is scanned.
                start = current;
                scanToken();
            }

            tokens.push_back(Token(line, TokenType::EOF, nullptr, ""));
            return tokens;
        }

        // Method that checks whether the Scanner has reached the end of the raw source code.
        bool isAtEnd(){
            return current >= source.length();
        }

        // Method that consumes (returns it and advances the cursor) the next character in the source file and returns it.
        char advance(){
            return source[current++];
        }

        // Auxiliar method to add a token to the list of tokens produced by the Scanner.
        void addToken(TokenType type){
            addToken(type, nullptr);
        }

        // Method that creates and adds the current token (generated from the current lexeme) to the list of tokens produced by the Scanner.
        void addToken(TokenType type, std::any literal){
            std::string lexeme = source.substr(start, current - start);
            tokens.push_back(Token(line, type, literal, lexeme));

            return;
        }

        void scanToken(){
            char c = advance();
            switch(c){
                case '(':
                    addToken(TokenType::LEFT_PAREN);
                    break;
                case ')':
                    addToken(TokenType::RIGHT_PAREN);
                    break;
                case '{':
                    addToken(TokenType::LEFT_BRACE);
                    break;
                case '}':
                    addToken(TokenType::RIGHT_BRACE);
                    break;
                case ',':
                    addToken(TokenType::COMMA);
                    break;
                case '.':
                    addToken(TokenType::DOT);
                    break;
                case '-':
                    addToken(TokenType::MINUS);
                    break;
                case '+':
                    addToken(TokenType::PLUS);
                    break;
                case ';':
                    addToken(TokenType::SEMICOLON);
                    break;
                case '*':
                    addToken(TokenType::STAR);
                    break;
                case '!':
                    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                    break;
                case '=':
                    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                    break;
                case '<':
                    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                    break;
                case '>':
                    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                    break;
                case '/':
                    if(match('/')){
                        // It's a comment.
                        while(peek() != '\n' && !isAtEnd()) advance();
                    }else{
                        // It's the division operator '/'.
                        addToken(TokenType::SLASH);
                    }
                    break;
                case ' ':
                    break;
                case '\r':
                    break;
                case '\t':
                    break;
                case '\n':
                    line++;
                    break;
                case '"':
                    string();
                    break;
                default:
                    error(line, "Unexpected character.");
                    break;
            }
        }

        // Method that adds a string literal token.
        void string(){
            while(peek() != '"' && !isAtEnd()){
                if(peek() == '\n') line++;
                advance();
            }

            if(isAtEnd()){
                error(line, "Unterminated string.");
                return;
            }

            std::string literal = source.substr(start + 1, current - start - 2);
            addToken(TokenType::STRING, literal);

            return;
        }

        // Method that checks if the next character (after the current one) matches the expected character.
        // If not, returns false. Otherwise, consumes the next character and returns true.
        bool match(char expected){
            if(current >= source.length()) return false;
            if(source[current] != expected) return false;

            current++;
            return true;
        }

        // Method that works very similar to the "advance" method. However, it does not consume the character.
        // Such method works as what's called a lookahead.
        // Since it only looks at the current unconsumed character, we have 1 character of lookahead.
        char peek(){
            if(isAtEnd()) return '\0';
            return source[current];
        }
}