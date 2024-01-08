#pragma once

#include <vector>

#include "Expr.hpp"
#include "Token.hpp"
#include "TokenType.hpp"

class Parser{
  private:
    const std::vector<Token>& tokens;
    int current = 0; // Points to the index of the next token waiting to be consumed.

    // Function equivalent to the "expression" rule.
    std::shared_ptr<Expr> expression(){
      return equality();
    }

    // Function equivalent to the "equality" rule.
    // Equality (and Inequality) are both Left-Associative operators.
    // It creates a left-associative nested tree of binary operator nodes.
    // Note that if the parser never encounters an equality operator, then it never enters the loop.
    // In that case, the equality() method effectively calls and returns comparison().
    // In that way, this method matches an equality operator or anything of higher precedence.
    std::shared_ptr<Expr> equality(){
      std::shared_ptr<Expr> expr = comparison();

      while(match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)){
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary>(expr, std::move(op), right);
      }

      return expr;
    }

    // Function equivalent to the "comparison" rule.
    // Comparison operators (<=, <, >, >=) are all Left-Associative operators.
    // It creates a left-associative nested tree of binary operator nodes.
    // Note that if the parser never encounters a comparison operator, then it never enters the loop.
    // In that case, the comparison() method effectively calls and returns term().
    // In that way, this method matches a comparison operator or anything of higher precedence.
    std::shared_ptr<Expr> comparison(){
      std::shared_ptr<Expr> expr = term();

      while(match(TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL)){
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<Binary>(expr, std::move(op), right);
      }

      return expr;
    }

    // Function equivalent to the "term" rule.
    // Term operators (-, +) are all Left-Associative operators.
    // It creates a left-associative nested tree of binary operator nodes.
    // Note that if the parser never encounters a term operator, then it never enters the loop.
    // In that case, the term() method effectively calls and returns factor().
    // In that way, this method matches a term operator or anything of higher precedence.
    std::shared_ptr<Expr> term(){
      std::shared_ptr<Expr> expr = factor();

      while(match(TokenType::MINUS, TokenType::PLUS)){
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<Binary>(expr, std::move(op), right);
      }

      return expr;
    }

    // Function equivalent to the "factor" rule.
    // Term operators (/, *) are all Left-Associative operators.
    // It creates a left-associative nested tree of binary operator nodes.
    // Note that if the parser never encounters a factor operator, then it never enters the loop.
    // In that case, the factor() method effectively calls and returns unary().
    // In that way, this method matches a factor operator or anything of higher precedence.
    std::shared_ptr<Expr> factor(){
      std::shared_ptr<Expr> expr = unary();

      while(match(TokenType::SLASH, TokenType::STAR)){
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary>(expr, std::move(op), right);
      }

      return expr;
    }

    // Function equivalent to the "unary" rule.
    // Unary operators (-, !) are all Right-Associative operators.
    // It creates a right-associative nested tree of unary operator nodes.
    // Note that if the parser never encounters an unary operator, then it never enters the if-clause.
    // In that case, the unary() method effectively calls and returns primary().
    // In that way, this method matches an unary operator or anything of higher precedence.
    std::shared_ptr<Expr> unary(){
      std::shared_ptr<Expr> expr;

      if(match(TokenType::MINUS, TokenType::BANG)){
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Unary>(std::move(op), right);
      }else{
        expr = primary();
      }

      return expr;
    }

    // Function equivalent to the "primary" rule.
    // It's the highest precedence rule of the Lox Context-Free Grammar.
    // Pay attention to the fact that you can nest expressions if higher precedence by using parenthesis.
    // In that way, this method matches an unary operator or anything of higher precedence.
    std::shared_ptr<Expr> primary(){

      if(match(TokenType::NIL))
        return std::make_shared<Literal>(nullptr);
      if(match(TokenType::TRUE))
        return std::make_shared<Literal>(true);
      if(match(TokenType::FALSE))
        return std::make_shared<Literal>(false);
      if(match(TokenType::NUMBER, TokenType::STRING))
        return std::make_shared<Literal>(previous().literal);
      if(match(TokenType::LEFT_PAREN)){
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return std::make_shared<Grouping>(expr);
      }
    }

    // Function that checks whether the next token to be consumed is of any the given types. 
    // If that's the case, it consumes the token and returns true. 
    // Otherwise, it leaves the token alone and return false.
    template<class... T>
    bool match(T... type){
      assert((... && std::is_same_v<T, TokenType>));

      if((... || check(type))){
        advance();
        return true;
      }

      return false;
    }

    // Function that receives a TokenType and checks whether the current token is of such type. 
    // It doesn't consume anything, only executes a check (look at the current unconsumed token).
    bool check(TokenType type){
      if(isAtEnd()) return false;
      return peek().type == type;
    }

    // Function that consumes the current token that has not been consumed yet and returns it.
    Token advance(){
      if(!isAtEnd()) current++;
      return previous();
    }

    // Function that checks whether we've reached the end of the program (end of file).  
    bool isAtEnd(){
      return peek().type == TokenType::FILE_END;
    }

    // Function that returns the current token that has not been consumed yet.
    Token peek(){
      return tokens[current];
    }

    // Function that returns the most recently consumed token by the parser.
    // Such function makes it easier to use match() and then access the just-matched token.
    Token previous(){
      return tokens[current - 1];
    }

  public:
    Parser(const std::vector<Token>& tokens)
      : tokens{tokens}
    {}


};
