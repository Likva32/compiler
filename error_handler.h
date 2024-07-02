//
// Created by likva32 on 6/14/2024.
//

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "error_handler.h"
#include "memory_control.h"
#include <stdlib.h>
#include "parser.h"
#include "stdio.h"

typedef struct {
    char *error_type;
    size_t line_number;
} Error;


void init_error_type();

Error* create_error(char *error_type, const size_t line_number);

void add_error(char *error_type, const size_t line_number);

void print_all_errors();


#endif //ERROR_HANDLER_H
