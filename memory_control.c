//
// Created by Likva32 on 19.05.2024.
//
#include "memory_control.h"
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdio.h>

linked_list* Manager = NULL;

void add_address_to_memory_control(void* address){
    push_linked_list(&Manager, address);
}

void free_all(){
    while(!is_empty_linked_list(Manager)){
        free(pop_linked_list(Manager));
    }
}

void* safe_malloc(size_t size){
    void* address = malloc(size);
    if(address == NULL)
        exitWithError("Error at malloc",ERR_ALLOCATION, 0);
    add_address_to_memory_control(address);
    return address;
}

void exitWithError(char* masssage, int ErrCode, int linenum) {
    switch (ErrCode)
    {
        case ERR_LEX:
            fprintf(stderr,"\033[1m\033[31mError in the program within the lexical analysis (incorrect structure of the current lexeme) line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SYNTAX:
            fprintf(stderr,"\033[1m\033[31mError in the parsing program line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_FUNC_ARG:
            fprintf(stderr,"\033[1m\033[31mSemantic error in the program - undefined function, variable redefinition line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_PARAM:
            fprintf(stderr,"\033[1m\033[31mWrong number/type of parameters when calling the function or wrong type of return value from function line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_UNDF_VALUE:
            fprintf(stderr,"\033[1m\033[31mSemantic error in program - use of undefined or uninitialized for variable line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_RETURN:
            fprintf(stderr,"\033[1m\033[31mMissing/overrun expression in the return statement from the function line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_TYPE:
            fprintf(stderr,"\033[1m\033[31mType compatibility semantic error in arithmetic, string, and relational expressions line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_UNDF_TYPE:
            fprintf(stderr,"\033[1m\033[31mThe variable or parameter type is not specified and cannot be inferred from the expression used line:\033[0m \033[1m\033[33m%d\n",linenum);
            break;
        case ERR_SEMANT_OTHER:
            fprintf(stderr,"\033[1m\033[31mOther semantic errors:\033[0m \033[33m%d\n",linenum);
            break;
        case ERR_INTERNAL:
            fprintf(stderr,"\033[1m\033[31mInternal compiler error, i.e. not affected by the input program line:\033[0m \033\033[1m%d\n",linenum);
            break;
        case ERR_ALLOCATION:
            fprintf(stderr,"\033[1m\033[31mALLOCATION ERROR\033[0m \033\033[1m\n");
            break;
    }
    fprintf(stderr, "\033[35m\033[1m%s\n\033[0m", masssage);
    free_all();
    exit(ErrCode);
}