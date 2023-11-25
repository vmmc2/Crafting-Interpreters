#pragma once

#include <any>
#include <memory>
#include <vector>
#include <utility>

#include "Token.hpp"

struct Expr;
struct Binary;
struct Unary;
struct Literal;
struct Grouping;


struct Expr{
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

struct Binary : Expr, public std::enable_shared_from_this<Binary>{
    const std::shared_ptr<Expr> left;
    const Token op;
    const std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right) 
        : left{std::move(left)}, op{std::move(op)}, right{std::move(right)}
    {}

};

struct Unary : Expr, public std::enable_shared_from_this<Unary>{
    const Token op;
    const std::shared_ptr<Expr> right;

    Unary(Token op, std::shared_ptr<Expr> right)
        : op{std::move(op)}, right{std::move(right)} 
    {}
};

struct Literal : Expr, public std::enable_shared_from_this<Literal>{
    const std::any value;

    Literal(std::any value)
        : value{std::move(value)}
    {}
};

struct Grouping : Expr, std::enable_shared_from_this<Grouping>{
    const std::shared_ptr<Expr> expression;

    Grouping(std::shared_ptr<Expr> expression)
        : expression{std::move(expression)}
    {}
};