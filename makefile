CC = gcc
OBJS = main.o util.o game.o solver.o game_manager.o linear_programming_solver.o board_reader.o board_writer.o stack.o list.o move.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c util.h types.h game.h game_manager.h solver.h SPBufferset.h linear_programming_solver.h board_reader.h board_writer.h stack.h list.h move.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
linear_programming_solver.o: linear_programming_solver.c game.h util.h types.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
