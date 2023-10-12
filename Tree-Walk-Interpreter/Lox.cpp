#include <cstring> // std::strerror
#include <fstream> 
#include <iostream> // std::getline
#include <string>
#include <vector>

#include "Scanner.hpp"

void run(std::string source){
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    for(const Token& token : tokens){
        std::cout << token.toString() << std::endl;
    }

    return;
}


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


void runFile(std::string_view path){
    std::string contents = readFile(path);
    run(contents);
    
    if(hadError){
        std::exit(65);
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
        run(line);
        
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