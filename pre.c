#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXLINE 81
#define MAX_LINE_LEN 81
#define HASHSIZE 1 
/* hashtable's currently functioning as a linked list. If machine's capable to 
	keep more arrays - besides the machine image, it's possible to improve the
	program preformance by increasing the defined hash size. */ 
#define MAXLABEL 31

extern char *strdup(const char*);

typedef struct Macro_slot { /* table entry */
    struct Macro_slot *next; /* next enrty in chain */
    char *name; /* defined macro's name */
    char *lines; /* replacement text */
} Macro_slot;

static Macro_slot *m_hashtab[HASHSIZE];
/* a pointer table to store the macros' as a linked list */

struct Macro_slot * macro_alloc(void)
{
    return (Macro_slot *)malloc(sizeof(Macro_slot));
}

/* free_macros: a function to free the memmory allocated to macro declarations*/
void free_macros(void)
{
    Macro_slot *ptr;
    int i;
        
    for (i = 0; i < HASHSIZE; i++) {
        while (m_hashtab[i] != NULL) {
            ptr = m_hashtab[i]->next;
            free(m_hashtab[i]->lines);
            free(m_hashtab[i]->name);
            free(m_hashtab[i]);
            m_hashtab[i] = ptr;
        }
    }
}


/* macro_hash: form macro_hash lines for string s */
unsigned macro_hash(char *s)
{
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}


/* mlookup: m stands for "macro". look for s in macros macro_hash tabel */
Macro_slot *mlookup(char *s)
{
    Macro_slot *np;

    for (np = m_hashtab[macro_hash(s)]; np!= NULL; np = np->next) {
        if (strcmp(s, np->name) == 0)
            return np; /* found */
    }
    return NULL; /* incase not found */
}


/* install_macro: save macro's name and content (lines) in macros' hashtable */
Macro_slot *install_macro(char *name, char *lines)
{
    Macro_slot *np;
    unsigned hashval;

    if((np = mlookup(name)) == NULL) { /* not found */
        np = (Macro_slot *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL )
            return NULL;
        hashval = macro_hash(name);
        np->lines = strdup(lines);
        np->next = m_hashtab[hashval];  
        m_hashtab[hashval] = np;
    } else { /* already there */
        free((void *) np->lines); /* free previous defenition */
    }
    np->lines = lines;
    return np;
}


/* copy_macro_lines: copy the lines between the macro name and "endm"*/
char *copy_macro_lines(FILE **read)
{
    char line[MAXLINE];
    char *all_line;
    char c;
    int i;

    all_line = (char *)malloc(100*sizeof(line));/* assumed maximum macro size */
    *all_line = '\0';
    while (strstr(line, "endm") == NULL) {
        for (i = 0; i < MAXLINE; i++) {
            line[i] = '\0';
        }

        c = getc(*read);
        
        for (i = 0; i < MAXLINE; i++) {
            line[i] = c;
            c = getc(*read);
            if (c == '\n') {
                line[i+1] = '\n';
                break;
            }
        }
        
        if (strstr(line, "endm") == NULL) {
            strcat(all_line, line);
        }
    }
    return all_line;
}

/* copy_two_fields: the function places the first two words from given "curline"
   into the given fields pointers. if only one field second ptr reamains empty*/
void copy_two_fields(char *first_field, char *second_field, char *curline)
{
    int i;
    int j;

    j = 0;

    for (i = 0; i < MAXLINE; i++) { /* copy the first field */
        if (!isspace(curline[i])) {
          first_field[j] = curline[i];
          j++;
        }
        if (isspace(curline[i]) && (j > 0)) {
            break;
        }
    }
    first_field[j] = '\0';
    j = 0;
    
    for (; i < MAXLINE && curline[i] != '\0'; i++) { /* copy the second field */
        if (!isspace(curline[i])) {
            second_field[j] = curline[i];
            j++;
        }
        if (isspace(curline[i]) && (j > 0)) {
            break;
        }
    }
    second_field[j] = 0;
    return;
}

/* expand_macros: the function gets a file name and file pointer. It creats a
	a new new-file - identical to the original file which its name passed, but
	with macro-defenitions expanded. After the file created it points the given
	file pointer to the new file, with reading permission. For its operatin, the
	function usesstwo FILE pointers: the input pointer for reading, and another
	for writing. It examines each line separately, and places macros contents
	instead of their names into the new file. */
void expand_macros(char *file_name, FILE **read)
{
    FILE *write;
    char c;
    char curline[MAXLINE]; /* I assumed the max macro size*/
    int i;
    char *first_field;
    char *second_field;
    char *new_file_name;
    int empty_line;
    
    Macro_slot *saved_macro;

    if ((new_file_name = strdup(file_name)) == NULL) {
        printf("error: file name\n");
        exit(1);
    }
    
    
    if ((*read = fopen(file_name, "r")) == NULL) {
    	printf("error: unable to find file '%s'\n", file_name);
    	exit(1);
    }
    
    new_file_name[strlen(new_file_name) -1] = 'm';/*now ".am" instead of ".as"*/
    write = fopen(new_file_name, "w"); 

    first_field = (char *)malloc(MAXLABEL*sizeof(char));
    second_field = (char *)malloc(MAXLABEL*sizeof(char));
    saved_macro = macro_alloc();
    
    while (c != EOF) {
        
        for (i = 0; i < MAXLINE; i++) { 
        /* copy each line until max-line-length, new line break, or EOF case */
            
            curline[i] = (c = getc(*read));
           
            if (!isspace(c))
            	empty_line = 0;

            if (c == EOF) {
                fclose(write);
                *read = fopen(new_file_name, "r");
                free_macros();
                return;
            }
            
            if (c == '\n') {
                curline[i] = '\n';
                curline[++i] = '\0';
                break;
            }
        }
        
        /* now check for macros declaratins and names (if not an empty line) */
        if (!empty_line)
        	copy_two_fields(first_field, second_field, curline);
        	
        if (strcmp(first_field, "macro") == 0) { /* found a macro */
            saved_macro = install_macro(second_field,copy_macro_lines(read));
        } else if (strlen(second_field) == 0 && strlen(first_field) > 0) {
			/* if the line contains only one field -> maybe a macro name */
            if ((saved_macro = mlookup(first_field)) != NULL) 
            		/* searching for the macro in the macro_hash table */
                fputs(saved_macro->lines, write);
            else { /*not a macro's name -> code's errors will be handled later*/
                fputs(curline, write);
            }
        } else {
            fputs(curline, write);
        }
        /* now get ready for next line */
        memset(first_field, '\0', MAXLABEL);
        memset(second_field, '\0', MAXLABEL);
        memset(curline, '\0', MAXLINE);
        empty_line = 1;
    }
    return;
}
