#include <string>
#include <vector>
#include <cstring> // std::strerror
#include <fstream> 
#include <iostream> // std::getline

#include "Error.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "Resolver.hpp"
#include "AstPrinter.hpp"
#include "Interpreter.hpp"

// It's not good practice to include .cpp files, but in our case it
// allows us to lay out the files similarly to the Java code while
// avoiding circular dependencies.
#include "LoxFunction.cpp" // Chapter 10 - Functions
#include "LoxClass.cpp"    // Chapter 12 - Classes
#include "LoxInstance.cpp" // Chapter 12 - Classes

Interpreter interpreter{}; 

std::string readFile(std::string_view path) {
  std::ifstream file{path.data(), std::ios::in | std::ios::binary | std::ios::ate};
  if(!file){
    std::cerr << "Failed to open file " << path << ": " << std::strerror(errno) << "\n";
    std::exit(74);
  }

  std::string contents;
  contents.resize(file.tellg());

  file.seekg(0, std::ios::beg);
  file.read(contents.data(), contents.size());

  return contents;
}

void run(std::string_view source){
  Scanner scanner{source};
  std::vector<Token> tokens = scanner.scanTokens();

  // for(const Token& token : tokens){
  //   std::cout << token.toString() << std::endl;
  // }

  Parser parser{tokens};
  std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

  if(hadError) return;

  // std::cout << AstPrinter{}.print(expression) << std::endl;

  Resolver resolver{interpreter};
  resolver.resolve(statements);

  // Stop if there was a resolution error.
  if (hadError) return;

  interpreter.interpret(statements);

  return;
}

void runFile(std::string_view path){
  std::string contents = readFile(path);
  run(contents);
    
  if(hadError){
    std::exit(65);
  }

  if(hadRuntimeError){
    std::exit(70);
  }

  return;
}

void runPrompt(){
  for(;;){
    std::cout << "> ";
    std::string line_of_code;
    if(!std::getline(std::cin, line_of_code)){
      break;
    }
    run(line_of_code);
        
    hadError = false;
  }

  return;
}

int main(int argc, char* argv[]){ 
  if(argc == 1){
    runPrompt();
  }else if(argc == 2){
    runFile(std::string_view(argv[1]));
  }else{
    std::cout << "Error! Wrong number of arguments. Should be 0 or 1." << std::endl;
    std::exit(64);
  }
  return 0;
}