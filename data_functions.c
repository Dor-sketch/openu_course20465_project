#include "symbol_table.h"
#include "syntax.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum data_type { DATA,
                         STRING,
                         ENTRY,
                         EXTERN,
                         INVALID_DATA } data_type;

/* max: returns the maximum value between two integers */
int max(int a, int b) {
    return a > b ? a : b;
}

/* get_label: returns the label of the source line */
char *get_label(src_op_line *srcline) {
    return srcline->label;
}

/* get_label_length: returns the length of the label */
size_t get_label_length(src_op_line *srcline) {
    return strlen(srcline->label);
}

/* copy_datatype: returns the data type as a string */
char *copy_datatype(const char *aligned_instruction) {
    size_t start = 0, end= 0;
    char *temp_string = NULL;

    start = strcspn(aligned_instruction, ".");
    if (start == strlen(aligned_instruction)) {
        return NULL;
    }

    end = start + strcspn(aligned_instruction + start, " \n");

    temp_string = (char *)malloc(end - start + 1);
    if (!temp_string) {
        perror("Memory allocation failed for datatype string");
        return NULL;
    }

    strncpy(temp_string, aligned_instruction + start, end - start);
    temp_string[end - start] = '\0';

    return temp_string;
}

/* get_datatype: returns the data type of the source line */
data_type get_datatype(const char *aligned_instruction) {
    int i;
    data_type type = INVALID_DATA;
    const char *data_instructions[] = {".data", ".string", ".entry", ".extern", ".error"};
    char *temp_string = copy_datatype(aligned_instruction);
    if (!temp_string) {
        return INVALID_DATA;
    }

    for (i = 0; i < sizeof(data_instructions) / sizeof(data_instructions[0]); i++) {
        if (strcmp(temp_string, data_instructions[i]) == 0) {
            type = (data_type)i;
            break;
        }
    }
    free(temp_string);
    return type;
}

/* install_entry: the function only prints warnning. entries are handled
	separatly from the second pass file */
void install_entry(src_op_line *srcline, machine_word **data_img)
{
    if (strlen(srcline->label) > 0)
        printf("%s:%d: warrning: label '%s' before an enrty\n",
        	srcline->as_filename, srcline->line_num, srcline->label);
}

/* install_string: creates a binary-word representation for every character
   in the string after the special word ".string". The function also updates
   the sign of the number and handles errors regarding invalid numbers. */
void install_string(src_op_line *srcline, machine_word **data_img)
{
    int ending, start;

    ending = 0; /* a variable to represent the end of a string (closin "") */
    start = 0; /* same for the beginning of the string */

    if ((data_img[srcline->cur_dc] =
    	(machine_word *)malloc(sizeof(machine_word))) == NULL) {/* malloc fail*/
        printf("%s:%d: error: data image assress '%d' unavailable\n",
            srcline->as_filename, srcline->line_num, srcline->cur_dc);
        return;
    }

    start = strlen("  .string \"");
    ending = strlen(srcline->alignedsrc) - 2; /* -1 for '\"' and -2 for '\n' */

    if (strlen(srcline->label) > 0) { /* true if ther's a label to install */
        if (install(get_label(srcline), srcline->cur_dc + 1, ".data", srcline) == NULL) {
            printf("error: symbol is aleady defined\n");
        }
        start += strlen(srcline->label);
    }

    if (srcline->alignedsrc[ending] != '"') { /* miss quotes at string's end */
        printf("%s:%d: error: missing '\"' at the end of sting '%.*s'\n",
            srcline->as_filename, srcline->line_num, ending - start +1,
            &srcline->alignedsrc[start]); /* print only the string content */
        srcline->error_flag = EXIT_FAILURE;
    }

    for (; start < ending; start++) {
        data_img[srcline->cur_dc]->funct_nd_ops = (int)srcline->alignedsrc[start];
        srcline->cur_dc++;

        if ((data_img[srcline->cur_dc] =
        	(machine_word*) malloc(sizeof(machine_word))) == NULL) {
            printf("%s:%d: error: data image assress '%d' unavailable\n",
                srcline->as_filename, srcline->line_num, srcline->cur_dc);
            return;
        } /* allocate memory to the data image for the next character */
    } /* end of string to install */

    data_img[srcline->cur_dc]->funct_nd_ops = (int) '\0';
    srcline->cur_dc++;
    return;
}

/* install_numbers: creates a binary-word representation for every number
   after the special word ".data". The function also updates the sign of the
   number and handles errors regarding invalid numbers. */
void install_numbers(src_op_line *srcline, machine_word **data_img)
{
    int pos = strlen(".data "); /* set position on the first operand */
    int inside_number = 0;
    int is_negative = 1;
    int temp_num = 0;
    int was_number = 0; /* flags to deal with the last number */
    int after_comma = 0;
    int has_changed = srcline->cur_dc;
    size_t label_length = get_label_length(srcline);

    if ((data_img[srcline->cur_dc] = (machine_word *)malloc(sizeof(machine_word)))
    		== NULL) { /* malloc failure */
        printf("%s:%d: error: data image assress '%d' unavailable\n",
            srcline->as_filename, srcline->line_num, srcline->cur_dc);
        return;
    }

    if (strlen(srcline->label) > 0) { /* if ther's a label before the data */
        if (install(get_label(srcline), srcline->cur_dc + 1, ".data", srcline) == NULL) {
            printf("error: symbol aleady defined...\n");
        }
        pos += label_length + 2; /* +1 for ':', +2 for anather space */
    }

    for (; pos < strlen(srcline->alignedsrc); pos++) { /* until end of line */

        if (inside_number == 0) { /* update the sign at the begining */
            if (srcline->alignedsrc[pos] == '+' ||
            	srcline->alignedsrc[pos] == '-') {
                if ('-' == (char)srcline->alignedsrc[pos])
                    is_negative = -1;
                pos++;
        	}
        }
        inside_number = 1;

        if (!isdigit(srcline->alignedsrc[pos])) {
            if (srcline->alignedsrc[pos] == ',' ||
            	srcline->alignedsrc[pos] == '\n' ||
            	srcline->alignedsrc[pos] == ' ') { /* if valid number ending */

                if ((after_comma == 1) && (was_number == 0)) {/*missing number*/
                	printf("%s:%d: error: missing a number after ','\n",
            			srcline->as_filename, srcline->line_num);
                    srcline->error_flag = EXIT_FAILURE;
                    return;
                }

                data_img[srcline->cur_dc]->funct_nd_ops = temp_num*is_negative;
                srcline->cur_dc++; /*done intall data, now update data counter*/

                if ((data_img[srcline->cur_dc]
                	= (machine_word *)malloc(sizeof(machine_word))) == NULL) {
        	    	printf("%s:%d: error: data image assress '%d' unavailable\n",
            			srcline->as_filename, srcline->line_num, srcline->cur_dc);
                    return;
                } /* allocated space now get ready for next number */

                while ((srcline->alignedsrc[pos] != ',') &&
                		(pos < strlen(srcline->alignedsrc))) /* before '\n' */
                			pos ++;

                after_comma = (srcline->alignedsrc[pos] == ',') ? 1 : 0;

                if (srcline->alignedsrc[pos] == ' ')
                	pos++;

                inside_number = 0;
                was_number = 0;
                temp_num = 0;
                is_negative = 1;

            } else { /* if reads an invald character (nor a digit or a comma) */
                printf("%s:%d: error: invalid digit '%c'\n",
                    srcline->as_filename, srcline->line_num,
                    srcline->alignedsrc[pos]);
                srcline->error_flag = EXIT_FAILURE;
        	}
        } else if (inside_number) { /* update temp_num */
            	temp_num = temp_num*10 + (srcline->alignedsrc[pos] - '0');
            	was_number = 1;
        }
    } /* finished reading the numbers and updatind the Data-Counter */
    if (!(srcline->cur_dc - has_changed)) { /* if data image hasn't changed */
    	printf("%s:%d: error: data instruction must contains at least one number\n",
        	srcline->as_filename, srcline->line_num);
        srcline->error_flag = EXIT_FAILURE;
    }
}


/* install_extern: prints a warning if finds a label before an exteranl data
   saves the external sympol in the symbol table with external attributes. */
void install_extern(src_op_line * srcline, machine_word **data_img)
{
    int i;

    i = 0;

    if (strlen(srcline->label) > 0) { /* a label before an external data */
        printf("%s:%d: warrning: label '%s' before an external declaration\n",
        	srcline->as_filename, srcline->line_num, srcline->label);
    }

    for (i = strlen(".extern "); srcline->alignedsrc[i] != '\0'; i++) {
    	/* copy the symbol after ".extern" */
        srcline->label[i - strlen(".extern ")] = srcline->alignedsrc[i];
    }
    srcline->label[i - strlen(".extern ")-1] = '\0';

    if (install(srcline->label, 0, ".extern", srcline) == NULL) {
        printf("exit from external symbol statment\n");
    }
}

/* handle_unknown_data: prints an error message for an unknown data instruction */
void handle_unknown_data(src_op_line *srcline) {
    /* Find the start of the instruction (after the '.') */
    const char *start = strchr(srcline->alignedsrc, '.') + 1;

    /* If no '.' found, or it's the last character, print a default error message */
    if (!start || *start == '\0') {
        fprintf(stderr, "%s:%d: error: undefined data instruction\n",
                srcline->as_filename, srcline->line_num);
    } else {
        size_t len = strcspn(start, " \t\n");

        fprintf(stderr, "%s:%d: error: undefined data instruction: '.%.*s'\n",
                srcline->as_filename, srcline->line_num, (int)len, start);
    }
    srcline->error_flag = EXIT_FAILURE;
}

/* process_data: identifies the data type of the source line
   and calls the appropriate function to handle it. */
void process_data(src_op_line *srcline, machine_word **data_img) {
    data_type type = get_datatype(srcline->alignedsrc);

    switch (type) {
    case DATA:
        install_numbers(srcline, data_img);
        break;
    case STRING:
        install_string(srcline, data_img);
        break;
    case ENTRY:
        install_entry(srcline, data_img);
        break;
    case EXTERN:
        install_extern(srcline, data_img);
        break;
    case INVALID_DATA:
        handle_unknown_data(srcline);
        break;
    }
}
