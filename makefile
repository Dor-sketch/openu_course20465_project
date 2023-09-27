assembler: main.o pre.o first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o
	gcc -ansi -Wall -pedantic main.o pre.o first_pass.o second_pass.o print_output.o op_functions.o symbol_table.o data_functions.o util.o -o assembler
        
main.o: main.c syntax.h first_pass.h 
	gcc -c -ansi -Wall -pedantic main.c  -o main.o -lm

first_pass.o:  first_pass.c op_functions.h symbol_table.h data_functions.h util.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o -lm

op_functions.o: syntax.h
	gcc -c -ansi -Wall -pedantic op_functions.c -o op_functions.o -lm

symbol_table.o: symbol_table.c symbol_table.h 
	gcc -c -ansi -Wall -pedantic symbol_table.c -o symbol_table.o -lm

data_functions.o: data_functions.c syntax.h symbol_table.h data_functions.h 
	gcc -c -ansi -Wall -pedantic data_functions.c -o data_functions.o -lm

util.o: util.c syntax.h 
	gcc -c -ansi -Wall -pedantic util.c -o util.o -lm

second_pass.o: second_pass.c syntax.h symbol_table.h util.h data_functions.h
	gcc -c -ansi -Wall -pedantic second_pass.c -o second_pass.o -lm

print_output.o:
	gcc -c -ansi -Wall -pedantic print_output.c -o print_output.o -lm

pre.o:
	gcc -c -ansi -Wall -pedantic pre.c -o pre.o -lm