CXX=clang++
CXXFLAGS=-g -std=c++17 -Wall -pedantic

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o disassembler $^

%.o:	%.c
	$(CXX§) $@ -c $< $<
