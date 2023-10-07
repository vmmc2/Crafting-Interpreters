#pragma once

#include <iostream>
#include <string_view>


inline bool hadError = false;


static void report(int line, std::string_view where, std::string_view message){
    std::cerr << "[Line " << line << "] Error - " << where << ": " << message << std::endl;
    hadError = true;
}


static void error(int line, std::string_view message){
    report(line, std::string_view(""), message);
}