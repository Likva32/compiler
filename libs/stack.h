//
// Created by Likva32 on 21.05.2024.
//

#ifndef COMPILER_STACK_H
#define COMPILER_STACK_H
#define MAX_STACK_LENGTH 64

#include "graph.h"
#include "stdio.h"
#include "ht.h"

typedef struct {
    hashMap *content[MAX_STACK_LENGTH];
    int top;
} stack_ht;


typedef struct {
    char *value;
    TokenType type;
    struct Node *right;
    struct Node *left;
    Variable *variable;
    struct FunctionNode *func_node;
    struct IfNode *if_node;
    struct WhileNode *while_node;
    struct FuncCallNode *func_call_node;
} Node;

typedef struct {
    char *func_name;
    Node **args;
    int arg_count;
} FuncCallNode;

typedef struct FunctionNode {
    char *name;
    TokenType *param_types;
    char **param_names;
    int param_count;
    Node **statements;
    int statement_count;
    stack_ht *curr_stack_sym;
} FunctionNode;

typedef struct {
    Node *condition;
    Node **then_statements;
    int then_statement_count;
    Node **else_statements;
    int else_statement_count;
    stack_ht *curr_stack_sym;
} IfNode;

typedef struct {
    Node *condition;
    Node **body_statements;
    int body_statement_count;
    stack_ht *curr_stack_sym;
} WhileNode;


stack_ht *stack_clone(stack_ht *original_stack);

hashMap *stack_get(stack_ht *stack, int index);

hashMap *peek_stack(stack_ht *stack);

void push_stack(stack_ht *stack, hashMap *element);

hashMap *pop_stack(stack_ht *stack);

stack_ht *stack_create();

void print_tree(Node *node, int indent, char *identifier);

#endif //COMPILER_STACK_H
