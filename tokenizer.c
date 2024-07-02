#include "tokenizer.h"
#include "libs\graph.h"
#include "string.h"
#include "malloc.h"
#include "memory_control.h"

int line_number = 1;

Graph *init_token_num() {
    Graph *token = (Graph *) safe_malloc(sizeof(Graph));
    add_address_to_memory_control(token);
    token->START = ' ';
    token->END = ' ';
    initGraph(token);
    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
            token->vertex[i + '0'][j + '0'] = 1;
        }
        token->vertex[token->START][i + '0'] = 1;


        token->vertex['\n'][i + '0'] = 1;


        token->vertex[i + '0'][token->END] = 1;
    }
    set_visited_false(token);
    return token;
}

Graph *init_token_name_variable() {
    Graph *token = (Graph *) safe_malloc(sizeof(Graph));
    add_address_to_memory_control(token);
    token->START = ' ';
    token->END = ' ';
    initGraph(token);
    for (int i = 0; i <= ('z' - 'a'); i++) {
        for (int j = 0; j <= ('z' - 'a'); j++) {
            token->vertex[i + 'a'][j + 'a'] = 1;

            token->vertex[i + 'a'][j + '0'] = 1;
            token->vertex[i + '0'][j + 'a'] = 1;
        }
        token->vertex[token->START][i + 'a'] = 1;
        token->vertex[i + 'a'][token->END] = 1;
        token->vertex[i + '0'][token->END] = 1;
    }
    set_visited_false(token);
    return token;
}

Graph *init_token_string() {
    Graph *token = (Graph *) safe_malloc(sizeof(Graph));
    add_address_to_memory_control(token);
    token->START = ' ';
    token->END = ' ';
    initGraph(token);

    for (int i = 0; i <= ('z' - 'a'); i++) {
        for (int j = 0; j <= ('z' - 'a'); j++) {
            token->vertex[i + 'a'][j + 'a'] = 1;
            token->vertex[i + '0'][j + '0'] = 1;
            token->vertex[i + 'a'][j + '0'] = 1;
            token->vertex[i + '0'][j + 'a'] = 1;
        }


        token->vertex['"']['0' + i] = 1;
        token->vertex['0' + i]['"' + ASCII_LEN] = 1;

        token->vertex['"'][i + 'a'] = 1;
        token->vertex[i + 'a']['"'] = 1;

        token->vertex[token->START]['"'] = 1;
        token->vertex['"'][token->END] = 1;
    }
    set_visited_false(token);
    return token;
}

Graph *init_token_get_value() {
    Graph *token = (Graph *) safe_malloc(sizeof(Graph));
    add_address_to_memory_control(token);
    token->START = ' ';
    token->END = ' ';
    initGraph(token);
    join(token, token->START, token->START);

    for (int i = 0; i <= ('z' - 'a'); i++) {
        token->vertex['*'][i + 'a'] = 1;
    }
    for (int i = 0; i <= ('z' - 'a'); i++) {
        for (int j = 0; j <= ('z' - 'a'); j++) {
            token->vertex[i + 'a'][i + 'a'] = 1;
        }
    }
    set_visited_false(token);
    return token;
}

Graph *init_token_get_address() {
    Graph *token = (Graph *) safe_malloc(sizeof(Graph));
    add_address_to_memory_control(token);
    token->START = ' ';
    token->END = ' ';
    initGraph(token);
    join(token, token->START, token->START);
    for (int i = 0; i <= ('z' - 'a'); i++) {
        join(token, '&', i + 'a');
    }
    for (int i = 0; i <= ('z' - 'a'); i++) {
        for (int j = 0; j <= ('z' - 'a'); j++) {
            join(token, i + 'a', j + 'a');
        }
    }
    set_visited_false(token);
    return token;
}


Graph *init_token_graph(Data token_data) {
    Graph *token = (Graph *) safe_malloc(sizeof(Graph));
    add_address_to_memory_control(token);

    token->START = ' ';
    token->END = ' ';
    if ( token_data.type == SEMICOLON ||  token_data.type == RBRACE ||  token_data.type == LBRACE)
        token->END = '\n';

    int i;
    initGraph(token);
    join(token, token->START, token_data.text[0]);

    token->vertex['\n'][token_data.text[0]] = 1;


    for (i = 1; i < strlen(token_data.text); i++) {
        join(token, token_data.text[i - 1], token_data.text[i]);
    }
    join(token, token_data.text[strlen(token_data.text) - 1], token->END);
    set_visited_false(token);
    token->type_token = token_data.type;
    return token;
}


int init_all_tokens_graphs(Graph **tokens_graph_list) {
    int i;
    Data all_tokens_str[] = {
            {"(", LPAR},{")", RPAR},
            {"{", LBRACE},{"}", RBRACE},
            {",", COMMA},{"<3", SEMICOLON},
            {":", COLON},{"shave", EQUAL},
            {"loshave", NOT_EQUAL},{"<=", LESS_EQUAL},
            {"<", LESS},{">=", GREATER_EQUAL},
            {">", GREATER},{"lo", NOT},
            {"or", OR},{"and", AND},
            {"+", PLUS},{"-", MINUS},
            {"*", MULTIPLY},{"/", DIVIDE},
            {"=", ASSIGN},{"if", IF},
            {"exit", EXIT},{"write", WRITE},
            {"else", ELSE},{"func", FUNC},
            {"giveback", GIVEBACK},{"declare", DECLARE},
            {"shalem", INT_T},{"loshalem", FLOAT_T},
            {"bool", BOOL_T},{"mishpat", STRING_T},
            {"while", WHILE},{"call", FUNC_CALL},
            {NULL, END_OF_TOKENS}
    };

    for (i = 0; all_tokens_str[i].type != END_OF_TOKENS; i++) {
        tokens_graph_list[i] = init_token_graph(all_tokens_str[i]);
    }
    int j = i;

    tokens_graph_list[i] = init_token_num();
    tokens_graph_list[++i] = init_token_name_variable();
    tokens_graph_list[++i] = init_token_string();

     tokens_graph_list[j]->type_token = INT;
     tokens_graph_list[++j]->type_token= VARIABLE;
     tokens_graph_list[++j]->type_token= STRING;

    return i;
}

void print_token(Token token){
    printf("\nTOKEN VALUE: ");
    printf("%s",token.value);
    printf("\nline number: %d\n", token.line_num);

    switch(token.type) {
        case FLOAT:
            printf("TOKEN TYPE: FLOAT\n");
            break;
        case INT:
            printf("TOKEN TYPE: INT\n");
            break;
        case VARIABLE:
            printf("TOKEN TYPE: VARIABLE\n");
            break;
        case STRING:
            printf("TOKEN TYPE: STRING\n");
            break;
        case COMP:
            printf("TOKEN TYPE: COMPARATOR\n");
            break;
        case END_OF_TOKENS:
            printf("TOKEN TYPE: END OF TOKENS\n");
            break;
        case BEGINNING:
            printf("TOKEN TYPE: BEGINNING\n");
            break;
        case LPAR:
            printf("TOKEN TYPE: LPAR\n");
            break;
        case RPAR:
            printf("TOKEN TYPE: RPAR\n");
            break;
        case LBRACE:
            printf("TOKEN TYPE: LBRACE\n");
            break;
        case RBRACE:
            printf("TOKEN TYPE: RBRACE\n");
            break;
        case PLUS:
            printf("TOKEN TYPE: PLUS\n");
            break;
        case MINUS:
            printf("TOKEN TYPE: MINUS\n");
            break;
        case MULTIPLY:
            printf("TOKEN TYPE: MULTIPLY\n");
            break;
        case DIVIDE:
            printf("TOKEN TYPE: DIVIDE\n");
            break;
        case SEMICOLON:
            printf("TOKEN TYPE: SEMICOLON\n");
            break;
        case COMMA:
            printf("TOKEN TYPE: COMMA\n");
            break;
        case COLON:
            printf("TOKEN TYPE: COLON\n");
            break;
        case LESS:
            printf("TOKEN TYPE: LESS\n");
            break;
        case GREATER:
            printf("TOKEN TYPE: GREATER\n");
            break;
        case LESS_EQUAL:
            printf("TOKEN TYPE: LESS_EQUAL\n");
            break;
        case GREATER_EQUAL:
            printf("TOKEN TYPE: GREATER_EQUAL\n");
            break;
        case NOT_EQUAL:
            printf("TOKEN TYPE: NOT_EQUAL\n");
            break;
        case ASSIGN:
            printf("TOKEN TYPE: ASSIGN\n");
            break;
        case EQUAL:
            printf("TOKEN TYPE: EQUAL\n");
            break;
        case NOT:
            printf("TOKEN TYPE: NOT\n");
            break;
        case WRITE:
            printf("TOKEN TYPE: WRITE\n");
            break;
        case OR:
            printf("TOKEN TYPE: OR\n");
            break;
        case AND:
            printf("TOKEN TYPE: AND\n");
            break;
        case DECLARE:
            printf("TOKEN TYPE: DECLARE\n");
            break;
        case IF:
            printf("TOKEN TYPE: IF\n");
            break;
        case ELSE:
            printf("TOKEN TYPE: ELSE\n");
            break;
        case FUNC:
            printf("TOKEN TYPE: FUNC\n");
            break;
        case GIVEBACK:
            printf("TOKEN TYPE: GIVEBACK\n");
            break;
        case EXIT:
            printf("TOKEN TYPE: EXIT\n");
            break;
        case NULL_NODE:
            break;
        case STRING_T:
            printf("TOKEN TYPE: STRING_T\n");
            break;
        case INT_T:
            printf("TOKEN TYPE: INT_T\n");
            break;
        case FLOAT_T:
            printf("TOKEN TYPE: FLOAT_T\n");
            break;
        case BOOL_T:
            printf("TOKEN TYPE: BOOL_T\n");
            break;
        case FUNC_CALL:
            printf("TOKEN TYPE: FUNC_CALL\n");
            break;

    }

}

Graph **create_token_list_graphs(int size, int *amount_of_tokens) {
    Graph **token_list = (Graph **) calloc(size, sizeof(Graph *));
    add_address_to_memory_control(token_list);
    *amount_of_tokens = init_all_tokens_graphs(token_list);
    printf("%d", *amount_of_tokens);
    return token_list;
}

Token *lexer(Graph **token_list_graph, int amount_of_tokens, FILE *code_file) {
    Token *token;

    int number_of_tokens = 12;
    int tokens_size = 0;

    Token *tokens = safe_malloc(sizeof(Token) * number_of_tokens);
    int tokens_index = 0;

    char ID[100];
    int i = 0;
    int buffer_size = 1;
    int backup_buffer_size = 1;
    char state_in_graph = ' ';
    char cur_char = (char) getc(code_file);
    bool read_from_buffer = false;
    bool exit_inner_loop = false;
    int iteration = 0;

    while (cur_char != EOF) {
        for (int j = 0; (j <=  amount_of_tokens)  && ((cur_char != EOF) || (read_from_buffer) ); j++)  {
            if(iteration > amount_of_tokens *2){
                printf("%d %d\n",EOF,cur_char);
                read_from_buffer = false;
                i = 0;
                buffer_size = 1;
                backup_buffer_size = 1;
                cur_char = (char) getc(code_file);
            }


            exit_inner_loop = false;
            while ((cur_char != EOF || read_from_buffer) && !exit_inner_loop) {
                if (cur_char == EOF)
                    exit_inner_loop = true;
                if (read_from_buffer) {
                    cur_char = ID[i];
                }
                while (state_in_graph == ' ' && cur_char == ' ' ) {
                    cur_char = (char) getc(code_file);
                }
                while ((state_in_graph == ' ' && cur_char == '\n') || (state_in_graph == '\n' && cur_char == '\n')){
                    cur_char = (char) getc(code_file);
                    line_number++;
                }
                if (!read_from_buffer) {
                    ID[i] = cur_char;
                }

                int test = (int) state_in_graph;
                while (token_list_graph[j]->visited[test] && test != token_list_graph[j]->START ) {
                    test = test + ASCII_LEN;
                }
                if (test != token_list_graph[j]->START )
                    token_list_graph[j]->visited[test] = true;


                if ((j >= 34 && isAdjacent(token_list_graph[j], state_in_graph, cur_char)) ||
                    (j < 34 && isAdjacentUniqe(token_list_graph[j], state_in_graph, cur_char)) ||
                        (token_list_graph[j]->type_token== STRING && (cur_char == '"' && isAdjacentUniqe(token_list_graph[j], state_in_graph, cur_char)))) {

                    state_in_graph = cur_char;

                    if (state_in_graph == token_list_graph[j]->END) {
                        iteration = 0;
                        printf("\nfinal state reached \n");
                        ID[i] = '\0';

                        token = create_token(ID, token_list_graph[j]);
                        tokens_size++;

                        if (tokens_size > number_of_tokens) {
                            number_of_tokens *= 1.5;
                            tokens = realloc(tokens, sizeof(Token) * number_of_tokens);
                            add_address_to_memory_control(tokens);
                        }

                        tokens[tokens_index] = *token;
                        tokens_index++;

                        if (state_in_graph == '\n')
                            line_number++;

                        state_in_graph = token_list_graph[j]->START;
                        read_from_buffer = false;
                        i = 0;
                        buffer_size = 1;
                        backup_buffer_size = 1;
                        cur_char = (char) getc(code_file);
                        j = 0;
                        exit_inner_loop = true;
                    } else {
                        if (read_from_buffer) {
                            buffer_size--;
                            if (buffer_size == 0) {
                                read_from_buffer = false;
                                cur_char = (char) getc(code_file);
                                backup_buffer_size++;
                                buffer_size++;
                            }
                        } else {
                            cur_char = (char) getc(code_file);
                            backup_buffer_size++;
                            buffer_size++;
                        }
                        i++;
                    }
                } else {
                    read_from_buffer = true;
                    buffer_size = backup_buffer_size;
                    i = 0;
                    state_in_graph = token_list_graph[j]->START;
                    exit_inner_loop = true;
                }
            }
            set_visited_false(token_list_graph[j]);
            iteration++;
        }

    }
    tokens[tokens_index].value = 0;
    tokens[tokens_index].type = END_OF_TOKENS;
    tokens[tokens_index].line_num = line_number;
    fclose(code_file);

    return tokens;
}



Token* create_token(char* token_value, Graph* token_graph){
    Token *token = safe_malloc(sizeof(Token));
    add_address_to_memory_control(token);
    token->line_num = line_number;
    token->type = token_graph->type_token;
    token->value = safe_malloc(sizeof(char) * strlen(token_value));
    add_address_to_memory_control(token->value);
    strcpy(token->value, token_value);

    return token;
}
