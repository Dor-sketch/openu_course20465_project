#include "syntax.h"
#include "first_pass.h"
#include "pre.h"
#include "print_output.h"
#include "second_pass.h"

#define MAX_CODE_ARR 600 /* Maximum array size for code and data images */

extern char *strdup(const char *);

/* Frees memory allocated for machine words in an image array */
void free_machine_img(machine_word **img, int start, int counter) {
    for (int i = start; i < counter && img[i] != NULL; i++) {
        if (img[i] != NULL) {
            free(img[i]);
        }
    }
}

/* Frees various resources allocated during the assembly process */
void free_resources(char *as_filename, int *error_flag, machine_word **code_img,
                    machine_word **data_img, int ic, int dc) {
    if (as_filename != NULL) {
        free(as_filename);
    }
    if (code_img != NULL) {
        free_machine_img(code_img, 100, ic);
    }
    if (data_img != NULL) {
        free_machine_img(data_img, 0, dc);
    }
}

/* Updates and returns a new filename with '.as' extension */
char *update_input_filename(char *filename) {
    char *new_filename = malloc(strlen(filename) + 4);
    if (new_filename == NULL) {
        return NULL;
    }
    strcpy(new_filename, filename);
    strcat(new_filename, ".as");
    return new_filename;
}

/* Handles the first pass of assembly process */
int generate_first_pass(char *as_filename, FILE **ex_src_fl, int *error_flag,
                        int *ic, int *dc, machine_word **code_img, machine_word **data_img) {
    as_filename[strlen(as_filename) - 1] = 'm'; /* Change to ".am" */
    printf("Running first pass with ic = %d, dc = %d\n", *ic, *dc);
    int updated_dc = get_first_img(*ic, *dc, code_img, data_img, *ex_src_fl, as_filename, error_flag);
    return updated_dc;
}

/* Checks and reports if an error occurred during assembly */
int check_error_flag(int *error_flag) {
    if (*error_flag == EXIT_FAILURE) {
        printf("Error: File has errors, no output files created.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/* Processes a single assembly file */
void process_assembly_file(char *as_filename, int *ic, int *dc,
                           machine_word **code_img, machine_word **data_img,
                           int *error_flag) {
    FILE *am_file = NULL;

    expand_macros(as_filename, &am_file);
    if (!am_file) {
        *error_flag = EXIT_FAILURE;
        return;
    }

    *dc = get_first_img(*ic, *dc, code_img, data_img, am_file, as_filename, error_flag);
    if (*error_flag != EXIT_SUCCESS) {
        fclose(am_file);
        return;
    }

    fseek(am_file, 0, SEEK_SET); // Reset for second pass
    *ic = get_second_img(code_img, data_img, am_file, as_filename, error_flag);
    fclose(am_file);
}

/* Handles the assembly process */
void assemble(char *argv) {
    machine_word *code_img[MAX_CODE_ARR] = {0};
    machine_word *data_img[MAX_CODE_ARR] = {0};
    int ic = 100, dc = 0;
    char *as_filename = update_input_filename(argv);


    if (!as_filename) {
        fprintf(stderr, "Error: Failed to update input filename.\n");
        return;
    }

    int error_flag = EXIT_SUCCESS;
    process_assembly_file(as_filename, &ic, &dc, code_img, data_img, &error_flag);

    if (check_error_flag(&error_flag) == EXIT_SUCCESS) {
        make_output(code_img, data_img, ic, dc, as_filename);
    }

    free_resources(as_filename, &error_flag, code_img, data_img, ic, dc);
    printf("Finished assembling %s\n", argv);
}

/* Main function: processes each file passed as a command-line argument */
int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Error: Program needs at least one file to assemble.\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        assemble(argv[i]);
    }

    return EXIT_SUCCESS;
}
