C++ = g++
OBJS = main.o Puzzle.o Part.o Table.o
EXEC = ex1

COMP_FLAG = -std=c++14 -Wall -Wextra \
	    -Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
		$(C++) $(OBJS) -o $@
main.o: main.cpp Puzzle.h Table.h
		$(C++) $(COMP_FLAG) -c $*.cpp
Puzzle.o: Puzzle.h Puzzle.cpp Part.h Table.h
		$(C++) $(COMP_FLAG) -c $*.cpp
Part.o: Part.h Part.cpp
		$(C++) $(COMP_FLAG) -c $*.cpp
Table.o: Table.h Table.cpp
		$(C++) $(COMP_FLAG) -c $*.cpp

clean:
		rm -f $(OBJS) $(EXEC)
