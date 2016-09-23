SRCDIR = source
EXEDIR = bin
SRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard third_party_software/jsoncpp/*.cpp) $(wildcard third_party_software/googletest/gtest-all.cc)
INCLUDES = -I./include -I./third_party_software/googletest -I./third_party_software/jsoncpp
OBJS = $(SRC:.cpp = .o)
DEPENDS = $(OBJS: .o=.d)
FLAGS = -Wall -Wextra -Wconversion -pedantic -std=gnu++11 -lpthread 
CC=g++


all: vending_machine

vending_machine: $(OBJS)
	$(CC) $(FLAGS) $(INCLUDES) -o $(EXEDIR)/$@ $(OBJS)
	

.PHONY: clean
	
clean:
	$(RM) $(EXEDIR)/vending_machine 
	