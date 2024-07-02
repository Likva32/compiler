#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "libs\ht.h"
#include "generate_code.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handler.h"
#include "asm_to_machine_code.h"


void print_tokens(Token *tokens){
    int i = 0;
    while(tokens[i].type != END_OF_TOKENS){
        print_token(tokens[i]);
        i++;
    }
    print_token(tokens[i]);
}




int main() {
    init_error_type();

    int amount_of_tokens;
    Graph **token_list = create_token_list_graphs(50, &amount_of_tokens);

    FILE *code_file = fopen("test.txt", "r");
    if (code_file == NULL) {
        perror("error");
    }
    Token *tokens = lexer(token_list, amount_of_tokens, code_file);
    fclose(code_file);

    print_tokens(tokens);

    Node **arr_parse_tree = parse_code(tokens);
    printf("\n\n");




    int i = 0;
    while(arr_parse_tree[i]->type != NULL_NODE){
        print_tree(arr_parse_tree[i], 5 , "test");
        printf("\n");
        i++;
    }

    code_generator_generate(arr_parse_tree);

    print_all_errors();

    start_assemble();






    // free_all();
    return 0;
}

