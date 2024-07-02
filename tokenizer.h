#ifndef COMPILER_TOKENIZER_H
#define COMPILER_TOKENIZER_H

#include <stdio.h>
#include "libs\graph.h"


typedef struct Data {
    char* text;
    TokenType type;
}Data;

int init_all_tokens_graphs(Graph **tokens_graph_list);

Graph *init_token_graph(Data token_data);

Graph* init_token_num();

Graph* init_token_name_variable();

Graph* init_token_string();

Graph **create_token_list_graphs(int size, int *amount_of_tokens);

Token *lexer(Graph **token_list_graph, int amount_of_tokens, FILE *code_file);

Token* create_token(char* token_value, Graph* token_graph);

void print_token(Token token);

#endif //COMPILER_TOKENIZER_H
