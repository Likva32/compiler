//
// Created by likva32 on 6/14/2024.
//

#include "error_handler.h"
#define ERROR_SIZE 50
#define ERROR_MSG_SIZE 150
Error** error_arr;
int error_ind;

void init_error_type() {
    error_arr = (Error**)calloc(ERROR_SIZE,sizeof(Error*));
    error_ind = 0;
    global_symtable = NULL;
}

Error* create_error(char *error_type, const size_t line_number) {
    Error* error = (Error*)safe_malloc(sizeof(Error));
    error->error_type = error_type;
    error->line_number = line_number;
    return error;
}

void add_error(char *error_type, const size_t line_number) {
    error_arr[error_ind++] = create_error(error_type, line_number);
}

void print_all_errors() {
    int i =0;
    while(error_arr[i] != NULL) {
        printf("\nERROR: %s on line number: %zu\n", error_arr[i]->error_type, error_arr[i]->line_number);
        i++;
    }
}