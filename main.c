/*
	assembler: The program transfers assembly language files into a machine code
	it gets  files' names from the command line (without the ".as" extension),
	if the file "name.as" includes valid code statments and macros, it creates
	the following new files:
    "name.am"        for the source without macros abbrevetions
    "name.object"    for the machine code
    "name.externals" for external data words information
    "name.enrties"   for the "entry" type sympols in the file
*/

#include "syntax.h"
#include "first_pass.h"
#include "second_pass.h"
#include "print_output.h"
#include "pre.h"

#define MAX_CODE_ARR 600 /* maximum array size for code and data images */

extern char *strdup(const char*);

/* free_machine_img: a function to free the memmory allocated for machine words
	during the previous file's assembling */
void free_machine_img(machine_word **img, int start, int counter)
{
    int i;
    
    for (i = start; (i < counter) && (img[i] != NULL);) {
        free(img[i]);
        i++;
    }
}

/* assemble: the function gets a file name and assembles its machine code as
	described above. First it breakes MACRO lines into their content, written as
	a new ".am" file, then it reads the file twice while assembling the code and
	data images, using the functions from the first and second passes' files.
	If no errors found it prints the results using the funcions from the "print
	output" file. */ 
void assemble(char *argv)
{
    machine_word *code_img[MAX_CODE_ARR]; 
    machine_word *data_img[MAX_CODE_ARR];
    /* code and data images made of poiners to "machine word" structure */
    int ic, dc; /* an Instructions-Counter and a Data-Counter variables */
    FILE *am_file; /* a file pointer for the file after macros expanding */
    char *as_filename; /* for the ".as" file extension */
    int *error_flag;

    ic  = 100; /* first 100 places saved for the machine */
    dc  = 0;         
    as_filename = (char *)malloc(strlen(argv) + 4); /* +4 for file extention and null terminator*/
    error_flag = (int *)malloc(sizeof(int));
    strcpy(as_filename, argv);
    strcat(as_filename, ".as");

    expand_macros(as_filename, &(am_file));
    
    as_filename[strlen(as_filename) - 1] = 'm';/*now has ".am" at string's end*/
    /* run first pass */
    dc = get_first_img(ic, dc, code_img, data_img, am_file, as_filename,
    				   error_flag);     
    
    fseek(am_file, 0, SEEK_SET); /* reset the pointer and start second pass */
    ic = get_second_img(code_img, data_img, am_file, as_filename, error_flag);
    fclose(am_file);
    if (*error_flag == EXIT_SUCCESS) {
        make_output(code_img, data_img, ic, dc, as_filename);
    }
    free(as_filename);
    free(error_flag);
    free_machine_img(code_img, 100, ic);
    free_machine_img(data_img, 0, dc);
    return;
}

/*  main: the main function gets ".as" file names from the command line 
	(withot	file's extentions ".as"), and assemble each file - if valid. */
int main(int argc, char *argv[])
{
	int files_to_assamble;
    int i;
	
	if ((files_to_assamble = argc) <= 1) {/* if no file names are given */
		printf("error: program needs at least one file to assemble.\n");
	}
	
    for (i = 1; i < files_to_assamble; i++) {
       assemble(*(argv +i));
    }

    return EXIT_SUCCESS;
}
