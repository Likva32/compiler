//
// Created by Likva32 on 21.05.2024.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "memory_control.h"
#include "libs/stack.h"
#include "string.h"
#include "stdio.h"

#define VARIABLES_SIZE 50

typedef struct {
    Variable *arr[VARIABLES_SIZE];
    int ind;
} All_Variables;

extern hashMap *global_symtable;
extern stack_ht *stack_symtable;
extern stack_ht *cur_stack;
extern All_Variables *all_variables;


Variable *search_var_in_stack(char *var_name);

Node *init_node(char *value, TokenType type);

Node *create_bin_node(Node *mid, Node *left, Node *right);

Node *parse_print(Token *tokens, int *position);

Node *parse_parentheses(Token *tokens, int *position);

Node *parse_formula(Token *tokens, int *position, int precedence);

Node *parse_variable_or_number(Token *tokens, int *position);

Node *handle_variable(Token *tokens, int *position);

Node *parse_expression(Token *tokens, int *position);

Node **parse_code(Token *tokens);

Token *match(Token *tokens, int *position, TokenType type_excepted);

bool is_variable_exist(char *var_name);

void add_variable_num(Token *variable_token, float var_value, TokenType var_type);

void add_variable_string(Token *variable_token, char *var_value, TokenType var_type);

bool check_leaf_same_type(Node *root, TokenType expected_type);

float evaluate_tree(Node *root);

bool is_variable_exist(char *var_name);

TokenType getVarType(char *var_name);

void setVarValue(char *var_name, void *value, TokenType type);

Node *handle_func(Token *tokens, int *position);

Node *handle_if(Token *tokens, int *position);

int get_precedence(TokenType token_type);

#endif //COMPILER_PARSER_H
