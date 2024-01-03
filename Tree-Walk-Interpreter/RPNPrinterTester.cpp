#include "RPNPrinter.hpp"

int main(){
  std::shared_ptr<Expr> expression = std::make_shared<Binary>(
    std::make_shared<Grouping>(
      std::make_shared<Binary>(
        std::make_shared<Literal>(1.0),
        Token(1, TokenType::PLUS, nullptr, "+"),
        std::make_shared<Literal>(2.0)
      )
    ),
    Token(1, TokenType::STAR, nullptr, "*"),
    std::make_shared<Grouping>(
      std::make_shared<Binary>(
        std::make_shared<Literal>(4.0),
        Token(1, TokenType::MINUS, nullptr, "-"),
        std::make_shared<Literal>(3.0)
      )
    )
  );

  std::cout << RPNPrinter{}.print(expression) << std::endl;

  return 0;
}