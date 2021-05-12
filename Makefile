CXX ?= g++ # Set the C++ compiler to g++ iff it hasn't already been set
CPP_SRCS := $(wildcard *.cpp) 
OBJ_SRCS := parser.o lexer.o $(CPP_SRCS:.cpp=.o)
DEPS := $(OBJ_SRCS:.o=.d)

LEXER_TOOL := flex

ifeq ($(shell uname -s),Darwin)
	CXX = g++-11 -L/usr/local/opt/flex/lib -I/usr/local/opt/flex/include
	BISON = /usr/local/Cellar/bison/3.7.6/bin/bison
	ifeq ($(shell uname -m),arm64)
		BISON = /opt/homebrew/Cellar/bison/3.7.6/bin/bison
	endif
else 
	CXX ?= g++
	BISON = bison
endif
export CXX
export BISON

FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Wuninitialized -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wundef -Werror -Wno-unused -Wno-unused-parameter


TESTPROGS := $(wildcard tests/*.tnc)
TESTS := $(TESTPROGS:.tnc=)

.PHONY: all clean test cleantest

all: stdcrona.o
	make cronac

clean:
	rm -rf *.output *.o *.cc *.hh $(DEPS) cronac 
	make clean -C tests

-include $(DEPS)

cronac: $(OBJ_SRCS)
	$(CXX) $(FLAGS) -g -std=c++14 -o $@ $(OBJ_SRCS)

%.o: %.cpp 
	$(CXX) $(FLAGS) -g -std=c++14 -MMD -MP -c -o $@ $<

%.o: %.c
	$(CC) -c -std=c99 -o $@ $<

parser.o: parser.cc
	$(CXX) $(FLAGS) -Wno-sign-compare -Wno-sign-conversion -Wno-switch-default -g -std=c++14 -MMD -MP -c -o $@ $<

parser.cc: crona.yy
	$(BISON) -Werror -Wno-deprecated --defines=grammar.hh -v $<

lexer.yy.cc: crona.l
	$(LEXER_TOOL) --outfile=lexer.yy.cc $<

lexer.o: lexer.yy.cc
	$(CXX) $(FLAGS) -Wno-sign-compare -Wno-sign-conversion -Wno-old-style-cast -Wno-switch-default -g -std=c++14 -c lexer.yy.cc -o lexer.o

test: all
	make -C tests
