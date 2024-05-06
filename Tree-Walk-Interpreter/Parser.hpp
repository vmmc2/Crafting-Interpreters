#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Error.hpp"
#include "Token.hpp"
#include "TokenType.hpp"

class Parser{
  private:
    // This is a simple sentinel class we use to unwind the parser.
    struct ParseError: public std::runtime_error {
      using std::runtime_error::runtime_error;
    };
    const std::vector<Token>& tokens;
    int current = 0; // Points to the index of the next token waiting to be consumed.

    // Function equivalent to the "declaration" rule.
    std::shared_ptr<Stmt> declaration(){
      try{
        if(match(TokenType::CLASS)){
          return classDeclaration();
        }
        if(match(TokenType::FUN)){
          return function("function"); // From here on, we go to the "function" rule of our grammar, since we've consumed the keyword/token "fun".
        }
        if(match(TokenType::VAR)){
          return varDeclaration();
        }
        return statement();
      }catch(ParseError error){
        synchronize();
        return nullptr;
      }
    }

    // Function equivalent to the "classDecl" rule.
    std::shared_ptr<Stmt> classDeclaration(){
      Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
      consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

      std::vector<std::shared_ptr<Function>> methods;
      while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()){
        methods.push_back(function("method"));
      }

      consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

      return std::make_shared<Class>(std::move(name), std::move(methods));
    }

    // Function equivalent to the "varDecl" rule.
    std::shared_ptr<Stmt> varDeclaration(){
      Token name = consume(TokenType::IDENTIFIER, "Expected variable name after keyword 'var'.");
      std::shared_ptr<Expr> initializer = nullptr;

      if(match(TokenType::EQUAL)){
        initializer = expression();
      }

      consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
      return std::make_shared<Var>(std::move(name), initializer);
    }

    // Function equivalent to the "statement" rule.
    std::shared_ptr<Stmt> statement(){
      if(match(TokenType::FOR)){
        return forStatement();
      }
      if(match(TokenType::IF)){
        return ifStatement();
      }
      if(match(TokenType::PRINT)){
        return printStatement();
      }
      if(match(TokenType::LEFT_BRACE)){
        return std::make_shared<Block>(block());
      }
      if(match(TokenType::RETURN)){
        return returnStatement();
      }
      if(match(TokenType::WHILE)){
        return whileStatement();
      }

      return expressionStatement();
    }

    // Function equivalent to the "forStatement" rule.
    std::shared_ptr<Stmt> forStatement(){
      consume(TokenType::LEFT_PAREN, "Expect a '(' after 'for'.");

      // Checking for the "initializer" clause of the "for" loop.
      std::shared_ptr<Stmt> initializer;
      if(match(TokenType::SEMICOLON)){
        initializer = nullptr;
      }else if(match(TokenType::VAR)){
        initializer = varDeclaration();
      }else{
        initializer = expressionStatement();
      }

      // Checking for the "condition" clause of the "for" loop.
      std::shared_ptr<Expr> condition = nullptr;
      if(!check(TokenType::SEMICOLON)){
        condition = expression();
      }
      consume(TokenType::SEMICOLON, "Expected a ';' after the 'for' condition.");

      // Checking for the "increment" clause of the "for" loop.
      std::shared_ptr<Expr> increment = nullptr;
      if(!check(TokenType::RIGHT_PAREN)){
        increment = expression();
      }
      consume(TokenType::RIGHT_PAREN, "Expect ')' after 'for' clauses.");

      // Checking for the body of the 'for' loop.
      std::shared_ptr<Stmt> body = statement();

      if(increment != nullptr){
        body = std::make_shared<Block>(
          std::vector<std::shared_ptr<Stmt>>{
            body,
            std::make_shared<Expression>(increment)
          }
        );
      }

      if(condition == nullptr){
        condition = std::make_shared<Literal>(true);
      }
      body = std::make_shared<While>(condition, body);

      if(initializer != nullptr){
        body = std::make_shared<Block>(
          std::vector<std::shared_ptr<Stmt>>{
            initializer,
            body
          }
        );
      }

      return body;
    }

    // Function equivalent to the "ifStatement" rule.
    std::shared_ptr<Stmt> ifStatement(){
      consume(TokenType::LEFT_PAREN, "Expected a '(' after 'if'.");
      std::shared_ptr<Expr> condition = expression();
      consume(TokenType::RIGHT_PAREN, "Expected a ')' after the condition of an 'if'.");

      std::shared_ptr<Stmt> ifBranch = statement();
      std::shared_ptr<Stmt> elseBranch = nullptr;

      if(match(TokenType::ELSE)){
        elseBranch = statement();
      }

      return std::make_shared<If>(condition, ifBranch, elseBranch);
    }

    // Function equivalent to the "printStatement" rule.
    std::shared_ptr<Stmt> printStatement(){
      std::shared_ptr<Expr> value = expression();
      consume(TokenType::SEMICOLON, "Expected a ';' at the end of a PRINT statement.");

      return std::make_shared<Print>(value);
    }

    // Function equivalent to the "returnStatement" rule.
    std::shared_ptr<Stmt> returnStatement(){
      Token keyword = previous();
      std::shared_ptr<Expr> value = nullptr;

      if(!check(TokenType::SEMICOLON)){ // If the there is no ';' token after the 'return' token, then we expect an expression.
        value = expression();
      }

      consume(TokenType::SEMICOLON, "Expect a ';' after a return value"); // In both cases (where we have and where we don't have a return value) we expect a ';' at the end of the return statement.

      return std::make_shared<Return>(keyword, value);
    }

    // Function equivalent to the "expressionStatement" rule.
    std::shared_ptr<Stmt> expressionStatement(){
      std::shared_ptr<Expr> expr = expression();
      consume(TokenType::SEMICOLON, "Expected a ';' at the end of an expression statement");

      return std::make_shared<Expression>(expr);
    }

    // Function equivalent to the "function" rule.
    std::shared_ptr<Function> function(std::string kind){
      Token name = consume(TokenType::IDENTIFIER, "Expect a " + kind + " name."); // Stores the token with the name of the function.

      consume(TokenType::LEFT_PAREN, "Expect '(' after a " + kind + " name."); // Consume the left parenthesis after a function name in a function declaration.
      std::vector<Token> parameters;

      if(!check(TokenType::RIGHT_PAREN)){ // This if handles the case of zero parameters.
        do{ // This do-while loop parses the parameters as long as we find commas to separate them.
          if(parameters.size() >= 255){ // Here, we validate at parse time that you don’t exceed the maximum number of parameters a function is allowed to have.
            error(peek(), "Can't have more than 255 parameters.");
          }

          parameters.push_back(consume(TokenType::IDENTIFIER, "Expect a parameter name."));
        }while(match(TokenType::COMMA));
      }
      consume(TokenType::RIGHT_PAREN, "Expect a ')' after parameters."); // Finally, whether the function has 0 or more parameters, we should expect a ')' character.
    
      consume(TokenType::LEFT_BRACE, "Expect a '{' before a " + kind + " body.");
      std::vector<std::shared_ptr<Stmt>> body = block();

      return std::make_shared<Function>(std::move(name), std::move(parameters), std::move(body));
    }

    // Function equivalent to the "block" rule.
    std::vector<std::shared_ptr<Stmt>> block(){
      std::vector<std::shared_ptr<Stmt>> statements;

      while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()){
        statements.push_back(declaration());
      }

      consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

      return statements;
    }

    // Function equivalent to the "while" rule.
    std::shared_ptr<Stmt> whileStatement(){
      consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
      std::shared_ptr<Expr> condition = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after 'while' condition.");
      std::shared_ptr<Stmt> body = statement();

      return std::make_shared<While>(condition, body);
    }

    // Function equivalent to the "expression" rule.
    std::shared_ptr<Expr> expression(){
      return assignment();
    }

    // Function equivalent to the "assignment" rule.
    // Remember that an assignment is also an expression whose resulting value is the R-value of it.
    std::shared_ptr<Expr> assignment(){
      std::shared_ptr<Expr> expr = orExpression(); // This can either evaluate to a L-value or a R-value.

      if(match(TokenType::EQUAL)){
        Token equals = previous(); // This contains the token of TokenType::EQUAL
        std::shared_ptr<Expr> value = assignment();

        if(Variable* e = dynamic_cast<Variable*>(expr.get())){
          Token name = e->name;
          return std::make_shared<Assign>(std::move(name), value);
        }

        error(std::move(equals), "Invalid assignment target.");
      }

      return expr;
    }

    // Function equivalent to the "or" rule.
    std::shared_ptr<Expr> orExpression(){
      std::shared_ptr<Expr> expr = andExpression();

      while(match(TokenType::OR)){
        Token op = previous();
        std::shared_ptr<Expr> right = andExpression();
        expr = std::make_shared<Logical>(expr, std::move(op), right);
      }

      return expr;
    }

    // Function equivalent to the "and" rule.
    std::shared_ptr<Expr> andExpression(){
      std::shared_ptr<Expr> expr = equality();

      while(match(TokenType::AND)){
        Token op = previous();
        std::shared_ptr<Expr> right = equality();
        expr = std::make_shared<Logical>(expr, std::move(op), right);
      }

      return expr;
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
    // In that case, the unary() method will enter the else-clause and effectively calls and returns call().
    // In that way, this method matches an unary operator or anything of higher precedence.
    std::shared_ptr<Expr> unary(){
      if(match(TokenType::MINUS, TokenType::BANG)){
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary>(std::move(op), right);
      }

      return call();
    }

    // Auxiliar function to the one that implements the "call" rule.
    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee){
      std::vector<std::shared_ptr<Expr>> arguments;

      if(!check(TokenType::RIGHT_PAREN)){
        do{
          if(arguments.size() >= 255){
            error(peek(), "Can't have more than 255 arguments.");
          }
          arguments.push_back(expression());
        }while(match(TokenType::COMMA));
      }

      Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments of a function/method.");

      return std::make_shared<Call>(callee, std::move(paren), std::move(arguments));
    }

    // Function equivalent to the "call" rule.
    // This works in a left-associative way. Think of the following chaining of functions: f(1)(2)(3)
    std::shared_ptr<Expr> call(){
      std::shared_ptr<Expr> expr = primary();

      while(true){
        if(match(TokenType::LEFT_PAREN)){
          expr = finishCall(expr);
        }else if(match(TokenType::DOT)){
          Token name = consume(TokenType::IDENTIFIER, "After '.' expect a property name.");
          expr = std::make_shared<Get>(std::move(name), expr);
        }else{
          break;
        }
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
      if(match(TokenType::IDENTIFIER)){
        return std::make_shared<Variable>(previous());
      }

      throw error(peek(), "Expect an expression.");
    }

    // Function that checks to see if the next token is of the expected type.
    // If so, it consumes the token, returns it by calling the 'advance' method and everything is groovy.
    // Otherwise, an error is reported with a message.
    Token consume(TokenType type, std::string_view message){
      if(check(type)) return advance();

      throw error(peek(), message);
    }

    // Function that reports an error, show is to the user and throws a 'ParseError' instance.
    // Such function will report an error by calling the 'error' function that reports an error at a given token.
    // The 'error' function shows the token’s location and the token itself.
    // The 'error' method below returns the error instead of throwing it because we want to let the calling
    // method inside the parser decide whether to unwind or not.
    // For example: The 'consume' method unwinds the parser. However, other methods might not want to do it.
    ParseError error(Token token, std::string_view message){
      ::error(token, message);
      return ParseError{""};
    }

    // Function that synchronizes the forthcoming sequence of tokens after the parser got into a correct state.
    // This function will throw unwanted tokens until it find a token that represents the beginning of the next statement.
    // It discards tokens until it thinks it has found a statement boundary. 
    // After catching a ParseError, we’ll call this and then we are hopefully back in sync.
    // When it works well, we have discarded tokens that would have likely caused cascaded errors anyway,
    // and now we can parse the rest of the file starting at the next statement.
    void synchronize(){
      advance();

      while(!isAtEnd()){
        if(previous().type == TokenType::SEMICOLON) return;

        switch (peek().type){
          case (TokenType::CLASS):
          case (TokenType::FUN):
          case (TokenType::VAR):
          case (TokenType::FOR):
          case (TokenType::IF):
          case (TokenType::WHILE):
          case (TokenType::PRINT):
          case (TokenType::RETURN):
            return;
        }

        advance();
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

    std::vector<std::shared_ptr<Stmt>> parse(){
      std::vector<std::shared_ptr<Stmt>> statements;

      while(!isAtEnd()){
        statements.push_back(declaration());
      }

      return statements;
    }
};
