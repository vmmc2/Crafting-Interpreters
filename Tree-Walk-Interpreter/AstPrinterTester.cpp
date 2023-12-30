#include "AstPrinter.hpp"

int main(){
  std::shared_ptr<Expr> expression = std::make_shared<Binary>(
    std::make_shared<Unary>(
      Token(1, TokenType::MINUS, nullptr, "-"),
      std::make_shared<Literal>(123.00)
    ),
    Token(1, TokenType::STAR, nullptr, "*"),
    std::make_shared<Grouping>(
      std::make_shared<Literal>(45.67)
    )
  );

  std::cout << AstPrinter{}.print(expression) << "\n";

  return 0;
}