CC = gcc
CXX = g++
CFLAGS = -g -ansi -Wall -pedantic `pkg-config --cflags gtk+-3.0`
CPPFLAGS = -g -ansi -Wall -pedantic -std=c++17 `pkg-config --libs gtk+-3.0`
CXXFLAGS = $(CPPFLAGS) -std=c++17
GTKFLAGS = `pkg-config --cflags --libs gtk+-3.0`


all: assembler gui

assembler: main.o pre.o first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o gui.o
	$(CXX) $(CFLAGS) -o $@ $^ $(GTKFLAGS) `pkg-config --cflags --libs gtk+-3.0`

gui: gui.o main.o pre.o first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(GTKFLAGS) `pkg-config --cflags --libs gtk+-3.0`

gui.o: gui.cpp
	g++ -c $(CPPFLAGS) gui.cpp -o gui.o `pkg-config --cflags --libs gtk+-3.0`


main.o: main.c syntax.h first_pass.h
	$(CC) $(CFLAGS) -c main.c  -o main.o

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