//
// Created by Likva32 on 30.05.2024.
//

#ifndef COMPILER_GENERATE_CODE_H
#define COMPILER_GENERATE_CODE_H

#include "stdio.h"
#include "libs/stack.h"
#include "parser.h"
#include <stdint.h>

#define NO_REGISTER -1

#define NUM_OF_REGISTERS 6

#define REGISTER_NAME_LENGTH 8

#define R10 "r10"
#define R11 "r11"
#define R12 "r12"
#define R13 "r13"
#define R14 "r14"
#define R15 "r15"

#define RAX "rax"
#define RDX "rdx"


#define LABEL_FORMAT "L%d"
#define LABEL_NAME_LENGTH 8


#define MOV "\tmov %s, %s\n"
#define MOVSS "\tmovss %s, %s\n"

#define CMP "\tcmp %s, %s\n"
#define JE "\tje %s\n"

#define ADD "\tadd %s, %s\n"
#define SUB "\tsub %s, %s\n"
#define SBB "\tsbb %s, %s\n"

#define IMUL "\timul %s, %s\n"
#define IDIV "\tidiv %s\n"
#define NEG "\tneg %s\n"


#define DB " db 0\n"
#define DQ " dq 0\n"


typedef struct Register {
    char name[REGISTER_NAME_LENGTH];
    bool used;
} Register;

void generate_program(Node *root, unsigned char *code, int *code_offset, int *data_offset);

extern Register registers[NUM_OF_REGISTERS];
extern FILE *dest_file;

void code_generator_init();

int code_generator_register_alloc();

char *code_generator_label_create();

void code_generator_output_data_segment();

void code_generator_generate(Node **arr_parse_tree);

void code_generator_stmt(Node *parse_tree);

void code_generator_declare(Node *decl);

int code_generator_expr(Node *node);

void code_generator_if(Node *if_node);

void code_generator_while(Node *while_node);

void code_generator_write(Node *node);

void code_generator_output(FILE *file, char *string);

#endif //COMPILER_GENERATE_CODE_H
