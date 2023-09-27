#include "syntax.h"
#include "symbol_table.h"
#include <ctype.h>

#define LABEL (srcline->label)
#define LABEL_LENGTH (strlen(LABEL))

typedef enum data_type { DATA, STRING, ENTRY, EXTERN, INVALID_DATA } data_type;

void install_numbers(src_op_line *srcline, machine_word **data_img);
void install_extern(src_op_line *srcline, machine_word **data_img);
void install_string(src_op_line *srcline, machine_word **data_img);
void install_entry(src_op_line *srcline, machine_word **data_img);

/* a structure used to find data instruction and activiate their machine
   installiation function. It's used by the following function "get_datatype"
   to compare a given string to the valid data instructions names. */
static struct {
    char *name;
    void (*func)(src_op_line *src_line_ptr, machine_word **data_img);
    data_type type;
} data[] = {
    { ".data",   install_numbers,    DATA },
    { ".string", install_string,     STRING },
    { ".entry",  install_entry,      ENTRY },
    { ".extern", install_extern,     EXTERN },
    { ".error",  NULL,               INVALID_DATA }
};

/* copy_datatype: copy a string (array of characters) from special sign "." 
   until reaching token (space, new line etc.), returns a pointer to string 
   address*/
char *copy_datatype(char *aligned_instruction)
{
    char *temp_string;
    int i;
    
    temp_string = (char*) malloc(strlen(".extern") + 1); 
    /* size of ".extern" = size of ".string" > size of ".data" and ".entry" */

    for (i = 0; i<=strlen(temp_string) && aligned_instruction[i] != ' ' 
        	&& aligned_instruction[i] != '\0' 
        	&& aligned_instruction[i] != '\n' ; i++) {
        	
        	temp_string[i] = aligned_instruction[i];
    }
    temp_string[i] = '\0';
    return temp_string;
}

/* get_datatype: get a string and identify if its a valid data instruction
   aka .data .string .extern .entry. returns the "data_type" definiation
   of the input by searching for its name throgh the diffrent alternatives. */
data_type get_datatype(char *aligned_instruction)
{  
    char *temp_string;
    int i;
    int total_options;

    temp_string = copy_datatype(aligned_instruction);
    total_options = (sizeof(data)/sizeof(data[0])) - 1; 
    		    /* for the multi structure of data[] */

    for (i = 0; i < total_options; i++) { 
        if (strcmp(temp_string, data[i].name) == 0)
            break;
    }
    free(temp_string);
    return data[i].type;
}

/* install_entry: the function only prints warnning. entries are handled 
	separatly from the second pass file */
void install_entry(src_op_line *srcline, machine_word **data_img)
{
    if (strlen(srcline->label) > 0)
        printf("%s:%d: warrning: label '%s' before an enrty\n",
        	srcline->as_filename, srcline->line_num, srcline->label);                   
}

/* install_string: the function creates a binay-word for every character
   using the "machine word" structer describes in the syntax header, and
   saves it in the data image. The binary word is made froe the ASCII value
   of the character. An ending character '\0' is also created and saved at
   the data image array after reading the last character of the string. */ 
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
        if (install(LABEL, srcline->cur_dc+1, ".data", srcline) == NULL) { 
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

/* install_numbers: the function creates a binary-word representation for every
   number after the special word ".data". It converts every number from a string
   to an integer, by reading it digit by digit, and multiplies by 10 evey time.
   each number's saved as a binary-word in the data image after a comma,
   or at the end of the line. The function also updates the sign of the number
   and handles errors regarding invalid numbers. */ 
void install_numbers(src_op_line *srcline, machine_word **data_img)
{
    int pos = strlen(".data "); /* set position on the first operand */
    int inside_number = 0;
    int is_negative = 1;
    int temp_num = 0;
    int was_number = 0; /* flags to deal with the last number */
    int after_comma = 0;    
    int has_changed = srcline->cur_dc;       
	
    if ((data_img[srcline->cur_dc] = (machine_word *)malloc(sizeof(machine_word)))
    		== NULL) { /* malloc failure */
        printf("%s:%d: error: data image assress '%d' unavailable\n",
            srcline->as_filename, srcline->line_num, srcline->cur_dc);
        return;
    }
	
    if (strlen(srcline->label) > 0) { /* if ther's a label before the data */
        if (install(LABEL, srcline->cur_dc+1, ".data", srcline) == NULL) {
            printf("error: symbol aleady defined...\n");               
        }
        pos += LABEL_LENGTH + 2; /* +1 for ':', +2 for anather space */
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
        printf("exit from external symbol statmetn\n");
    }
}

/* proccess_data: install the given input from the source line into the data
	image as binary words. If it finds a relavant label also saves it in the
	Symbols-Table. The function also handles locally data-regarding input errors
	(undefind use of speciel "." symbol / missing string or numburs etc.) and
	update the error flag saved inside the source line structure accordingly. */
void process_data(src_op_line *srcline, machine_word **data_img)
{
    data_type type;
    int data_pos;

    data_pos = strlen(srcline->label) == 0 ? 0 : strlen(srcline->label) + 2;
    
    if ((type = get_datatype(&srcline->alignedsrc[data_pos])) == INVALID_DATA) {
        printf("%s:%d: error: undefined data instruction '",
        	srcline->as_filename, srcline->line_num);
        	/* if nor ".data" ".string" ".extern" ".entry" */
        while (!isspace(srcline->alignedsrc[data_pos])) { 
            putchar(srcline->alignedsrc[data_pos]);
            data_pos++;
        }/* prints the instruction */
        
        printf("'\n");
        
        if (strlen(srcline->label) != 0) { /* if ther's a label before "." */
            printf("%s:%d: note: symbol '%s' won't be installed due to an undefined statement\n",
                srcline->as_filename, srcline->line_num, srcline->label);
        }
        srcline->error_flag = EXIT_FAILURE;
    } else {
        (*(data[type].func))(srcline, data_img);
    }
}
