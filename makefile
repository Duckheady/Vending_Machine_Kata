SRCDIR = source
EXEDIR = bin
DEBUGDIR = Debug
RELEASEDIR = Release
PRODUCTIONSYMBOLSDIR = DeliverableSymbols
SRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard third_party_software/jsoncpp/*.cpp) $(wildcard third_party_software/googletest/gtest-all.cc)
INCLUDES = -I./include -I./third_party_software/googletest -I./third_party_software/jsoncpp
OBJS = $(SRC:.cpp = .o)
DEPENDS = $(OBJS: .o=.d)
DEBUGFLAGS = -Og
RELEASEFLAGS =
PRODUCTIONFLAGS = -O3
FLAGS = -g -Wall -Wextra -Wconversion -pedantic -std=gnu++11 -lpthread 
CC=g++

all: vending_machine_debug vending_machine_release vending_machine_deliverable

debug: vending_machine_debug

release: vending_machine_release

deliverable: vending_machine_deliverable

vending_machine_debug: $(OBJS)
	$(CC) $(DEBUGFLAGS) $(FLAGS) $(INCLUDES) -o $(EXEDIR)/$(DEBUGDIR)/$@ $(OBJS)
	
vending_machine_release: $(OBJS)
	$(CC) $(RELEASEFLAGS) $(FLAGS) $(INCLUDES) -o $(EXEDIR)/$(RELEASEDIR)/$@ $(OBJS)

vending_machine_deliverable: $(OBJS)
	$(CC) $(PRODUCTIONFLAGS) $(FLAGS) $(INCLUDES) -o $(EXEDIR)/$@ $(OBJS)
	#------------------------------------------------------------------------
	#Breakup symbols in the executable to a different folder.
	objcopy --only-keep-debug $(EXEDIR)/$@.exe $(EXEDIR)/$(PRODUCTIONSYMBOLSDIR)/$@.debug
	strip -g $(EXEDIR)/$@
	#Done
	
.PHONY: clean

clean:
	$(RM) $(EXEDIR)/$(DEBUGDIR)/vending_machine_debug.exe
	$(RM) $(EXEDIR)/$(RELEASEDIR)/vending_machine_release.exe
	$(RM) $(EXEDIR)/vending_machine_deliverable.exe
	$(RM) $(EXEDIR)/$(PRODUCTIONSYMBOLSDIR)/vending_machine_deliverable.debug
