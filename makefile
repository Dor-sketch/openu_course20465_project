CC = gcc
CXX = g++
CFLAGS = -g -ansi -Wall -pedantic
CPPFLAGS = -g -Wall -pedantic -std=c++17
CXXFLAGS = $(CPPFLAGS) -std=c++17
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
GTKLIBS = `pkg-config --libs gtk+-3.0`

all: assembler gui

assembler: main.o pre.o first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o gui.o
	$(CXX) $(CFLAGS) -o $@ $^ $(GTKLIBS)

gui: gui.o main.o pre.o first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(GTKLIBS)

main.o: main.c syntax.h first_pass.h
	$(CC) $(CFLAGS) -c main.c  -o main.o

gui.o: gui.cpp
	g++ -c $(CPPFLAGS) $(GTKCFLAGS) gui.cpp -o gui.o

first_pass.o:  first_pass.c op_functions.h symbol_table.h data_functions.h util.h
	$(CC) $(CFLAGS) -c first_pass.c -o first_pass.o

op_functions.o: op_functions.c syntax.h
	$(CC) $(CFLAGS) -c op_functions.c -o op_functions.o

symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c -o symbol_table.o

data_functions.o: data_functions.c syntax.h symbol_table.h data_functions.h
	$(CC) $(CFLAGS) -c data_functions.c -o data_functions.o

util.o: util.c syntax.h
	$(CC) $(CFLAGS) -c util.c -o util.o

second_pass.o: second_pass.c syntax.h symbol_table.h util.h data_functions.h
	$(CC) $(CFLAGS) -c second_pass.c -o second_pass.o

print_output.o: print_output.c
	$(CC) $(CFLAGS) -c print_output.c -o print_output.o

pre.o: pre.c
	$(CC) $(CFLAGS) -c pre.c -o pre.o

clean:
	rm -f *.o assembler gui *.ob *.ent *.ext *.am