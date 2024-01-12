# Simple Makefile to compile a C++ file

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17

# Source file
SRCS = Lox.cpp

# Executable name
TARGET = test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
