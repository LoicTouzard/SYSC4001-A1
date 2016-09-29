EXE=PartA
INT=BarberRoom.hpp CustomerGenerator.hpp WaitingRoom.hpp 

SRC=$(INT:.hpp=.cpp)
OBJ=$(INT:.hpp=.o)
	
MainSRC=PartA.cpp
MainOBJ=$(MainSRC:.cpp=.o)
	
EXEPATH=./

ECHO=@echo

CLEAN=clean
RM=rm
RMFlags=-f

Compilator=@g++
CompilationsOptions=-Wall -pedantic -std=c++11 
Linker=@g++
LinkerOptions=

all: $(EXE)

$(EXE): $(MainOBJ) $(OBJ)
	$(ECHO) "Link Edition for $(EXE)"
	$(Linker) -o $(EXEPATH)$(EXE) $(MainOBJ) $(OBJ) $(LinkerOptions)
	
$(MainOBJ): $(MainSRC) $(INT)
	$(ECHO) "Compilation of $(MainSRC)"
	$(Compilator) -o $(MainOBJ) -c $(MainSRC) $(CompilationsOptions)

%.o: %.cpp
	$(ECHO) "Compilation of $<"
	$(Compilator) -c $< $(CompilationsOptions)
	
# ***** CLEAN ***** #

.PHONY: $(CLEAN)
$(CLEAN):
	$(RM) $(RMFlags) $(EXEPATH)$(EXE) $(OBJ) $(MainOBJ)
