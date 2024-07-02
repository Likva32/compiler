//
// Created by Likva32 on 19.05.2024.
//

#ifndef COMPILER_MEMORY_CONTROL_H
#define COMPILER_MEMORY_CONTROL_H
#include "libs\linked_list.h"
#include "malloc.h"

#define ERR_OK                 0
#define ERR_LEX                1
#define ERR_SYNTAX             2
#define ERR_SEMANT_FUNC_ARG    3
#define ERR_SEMANT_PARAM       4
#define ERR_SEMANT_UNDF_VALUE  5
#define ERR_SEMANT_RETURN      6
#define ERR_SEMANT_TYPE        7
#define ERR_SEMANT_UNDF_TYPE   8
#define ERR_SEMANT_OTHER       9
#define ERR_INTERNAL           99
#define ERR_ALLOCATION         100


void add_address_to_memory_control(void* address);

void free_all();

void exitWithError(char* messsage, int ErrCode, int linenum);

void* safe_malloc(size_t size);

#endif //COMPILER_MEMORY_CONTROL_H
