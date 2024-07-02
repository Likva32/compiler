
#ifndef COMPILER_HT_H
#define COMPILER_HT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

typedef struct {
    Token* token;
    TokenType var_type;
    char *string_value;
    int int_value;
    float float_value;
    struct FunctionNode *func_value;
    int scope;
    int is_func;
} Variable;

typedef struct  {
    char* key;
    Variable* value;
    struct node* next;
}node;

Variable *create_variable_num(Token *token, float value_var, TokenType var_type);

Variable *create_variable_string(Token *token, char *value_var, TokenType var_type);

void setNode(node* node, char* key, Variable* value);

typedef struct  {
    int numOfElements, capacity;
    struct node** arr;
}hashMap;

hashMap* initializeHashMap();

int hashFunction(hashMap* mp, char* key);

void insert(hashMap* mp, char* key,Variable* value);

void delete (hashMap* mp, char* key);
Variable* search(hashMap* mp, char* key);

#endif //COMPILER_HT_H