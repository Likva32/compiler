//
// Created by Likva32 on 21.05.2024.
//

#include "parser.h"
#include "libs/stack.h"
#include "string.h"
#include "stdio.h"
#include "libs\ht.h"
#include "error_handler.h"

hashMap *global_symtable;
stack_ht *stack_symtable;
stack_ht *cur_stack;
All_Variables *all_variables;

Node *init_node(char *value, TokenType type) {
    Node *node = malloc(sizeof(Node));
    node->value = malloc(sizeof(char) * 2);
    node->type = (int) type;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->func_node = NULL;
    return node;
}

Node *create_bin_node(Node *mid, Node *left, Node *right) {
    mid->left = (struct Node *) left;
    mid->right = (struct Node *) right;
    return mid;
}

Node *parse_print(Token *tokens, int *position) {
    Token *write_token = match(tokens, position, WRITE);
    if (write_token != NULL) {
        Node *write_node = init_node(write_token->value, write_token->type);


        if (tokens[*position].type == STRING) {
            Node *string_node = init_node(tokens[*position].value, STRING);
            write_node->left = (struct Node *) string_node;
            (*position)++;
        } else {
            write_node->left = (struct Node *) parse_formula(tokens, position, 0);
        }

        return write_node;
    }
    add_error("EXPECTED WRITE", tokens[*position].line_num);
    return NULL;
}

Node *parse_parentheses(Token *tokens, int *position) {
    if (match(tokens, position, LPAR)) {
        Node *node = parse_formula(tokens, position, 0 );

        if (match(tokens, position, RPAR) == NULL) {
            add_error("EXPECTED )", tokens[*position].line_num);
            return NULL;
        }
        return node;

    } else if (match(tokens, position, NOT)) {
        Token *not_token = &tokens[*position - 1];
        Node *operand = parse_parentheses(tokens, position);
        Node *not_node = init_node(not_token->value, not_token->type);
        return create_bin_node(not_node, operand, NULL);
    }
    else {
        return parse_variable_or_number(tokens, position);
    }
}

int get_precedence(TokenType type) {
    switch (type) {
        case NOT:
            return 6;
        case MULTIPLY:
        case DIVIDE:
            return 5;
        case PLUS:
        case MINUS:
            return 4;
        case LESS:
        case LESS_EQUAL:
        case GREATER:
        case GREATER_EQUAL:
        case EQUAL:
        case NOT_EQUAL:
            return 3;
        case AND:
            return 2;
        case OR:
            return 1;
        default:
            return -1;
    }
}

Node *parse_formula(Token *tokens, int *position, int precedence) {
    Node *left_node = parse_parentheses(tokens, position);

    while (true) {
        Token *operator_token = NULL;
        TokenType type_tokens[] = {MULTIPLY, DIVIDE, PLUS, MINUS, LESS, LESS_EQUAL,
                                   GREATER, GREATER_EQUAL, EQUAL, NOT_EQUAL, AND, OR};
        int n = sizeof(type_tokens) / sizeof(type_tokens[0]);

        for (int i = 0; i < n && operator_token == NULL ; i++) {
            operator_token = match(tokens, position, type_tokens[i]);
        }

        if (operator_token == NULL || get_precedence(operator_token->type) < precedence)
            break;

        int operator_precedence = get_precedence(operator_token->type);
        Node *right_node = parse_formula(tokens, position, operator_precedence + 1);
        Node *operator_node = init_node(operator_token->value, operator_token->type);
        left_node = create_bin_node(operator_node, left_node, right_node);
    }
    if (match(tokens, position, NOT) != NULL) {
        Token *not_token = &tokens[*position - 1];
        Node *not_node = init_node(not_token->value, not_token->type);
        not_node->left = (struct Node *) left_node;
        left_node = not_node;
    }

    return left_node;
}

Node *parse_variable_or_number(Token *tokens, int *position) {
    Token *number = match(tokens, position, INT);
    if (number != NULL) {
        return init_node(number->value, number->type);
    }
    number = match(tokens, position, FLOAT);
    if (number != NULL) {
        return init_node(number->value, number->type);
    }
    number = match(tokens, position, STRING);
    if (number != NULL) {
        return init_node(number->value, number->type);
    }
    Token *variable = match(tokens, position, VARIABLE);
    if (variable == NULL) {
        add_error("EXPECTED VARIABLE OR NUMBER", tokens[*position].line_num);
        return NULL;
    }
    if (!is_variable_exist(variable->value)) {
        add_error("VARIABLE IS NOT DECLARED", tokens[*position].line_num);
        return NULL;
    }


    return init_node(variable->value, variable->type);
}

Node *handle_variable(Token *tokens, int *position) {
    Node *variable_node = parse_variable_or_number(tokens, position);

    if (!is_variable_exist(variable_node->value)) {
        add_error("VARIABLE IS NOT DECLARED", tokens[*position].line_num);
        return NULL;
    }

    Token *assign_operator = match(tokens, position, ASSIGN);
    if (assign_operator != NULL) {
        Node *assign_node = init_node(assign_operator->value, assign_operator->type);
        Node *right_formula_node = parse_formula(tokens, position,0);


        if (!check_leaf_same_type(right_formula_node, getVarType(variable_node->value))) {
            add_error("EXPECTED CORRECT TYPE", tokens[*position].line_num);
            return NULL;
        }

        variable_node->variable = search_var_in_stack(variable_node->value);
        Node *bin_node = create_bin_node(assign_node, variable_node, right_formula_node);
        return bin_node;
    } else {
        Token *lpar_token = match(tokens, position, LPAR);
        if (lpar_token != NULL) {
            handle_func(tokens, position);
        }
        add_error("EXPECTED ASSIGN", tokens[*position].line_num);
        return NULL;
    }
}

Token *handle_type(Token *tokens, int *position, TokenType *var_type) {
    switch (tokens[*position].type) {
        case INT_T:
            *var_type = INT;
            break;
        case FLOAT_T:
            *var_type = FLOAT;
            break;
        case BOOL_T:
            *var_type = BOOL;
            break;
        case STRING_T:
            *var_type = STRING;
            break;
        default:
            add_error("EXPECTED TYPE", tokens[*position].line_num);
            return NULL;
    }
    (*position)++;
    return &tokens[(*position)-1];
}

bool is_variable_exist(char *var_name) {
    if (search_var_in_stack(var_name)== NULL)
        return false;
    return true;
}

Variable* search_var_in_stack(char *var_name){
    hashMap *symtable;
    Variable *var = NULL;
    for (int i = cur_stack->top ; i >= 0 && var == NULL; i--) {
        symtable = stack_get(cur_stack, i);
        var = search(symtable, var_name);
    }
    return var;
}

TokenType getVarType(char *var_name) {
    Variable *var = search_var_in_stack(var_name);

    if (var != NULL) {
        switch (var->var_type) {
            case INT_T:
                return INT;
            case FLOAT_T:
                return FLOAT;
            case BOOL_T:
                return BOOL;
            case STRING_T:
                return STRING;
            case INT:
                return INT;
            case FLOAT:
                return FLOAT;
            case BOOL:
                return BOOL;
            case STRING:
                return STRING;
        }
    }
}

void setVarValue(char *var_name, void *value, TokenType type) {
    Variable *var = search_var_in_stack(var_name);

    if (type == INT) {
        printf("old value %d\n", var->int_value);
        var->int_value = (int) (*((float *) value));
        printf("new value %d\n", var->int_value);
    } else if (type == FLOAT) {
        printf("old value %f\n", var->float_value);
        var->float_value = *((float *) value);
        printf("new value %f\n", var->float_value);
    }else
    {
        printf("old value %s\n", var->string_value);
        var->string_value = (char *) value;
        printf("new value %s\n", var->string_value);
    }
}

void add_variable_num(Token *variable_token, float var_value, TokenType var_type) {
    Variable *var = create_variable_num(variable_token, var_value, var_type);
    insert(peek_stack(stack_symtable), variable_token->value, var);
    all_variables->arr[all_variables->ind++]= var;
}

void add_variable_string(Token *variable_token, char *var_value, TokenType var_type) {
    Variable *var = create_variable_string(variable_token, var_value, var_type);
    insert(peek_stack(cur_stack), variable_token->value, var);
    all_variables->arr[all_variables->ind++]= var;
}

bool check_leaf_same_type(Node *root, TokenType expected_type) {
    if (root == NULL) {
        return true;
    }
    if (root->left == NULL && root->right == NULL) {
        if (root->type != expected_type && (root->type == VARIABLE && getVarType(root->value) != INT && expected_type != INT)) {
            return false;
        }
    }
    return check_leaf_same_type((Node *) root->left, expected_type) &&
           check_leaf_same_type((Node *) root->right, expected_type);
}

float evaluate_tree(Node *root) {
    if(root ==NULL){
        return 0;
    }
    if (root->left == NULL && root->right == NULL) {
        TokenType key = root->type;
        switch (key) {
            case VARIABLE:{
                switch (getVarType(root->value)) {
                    case INT:
                        return search_var_in_stack( root->value)->int_value;
                    case FLOAT:
                        return search_var_in_stack( root->value)->float_value;
                }
            }
            case INT:
                return atoi(root->value);
            case FLOAT:
                return atof(root->value);
            default:
                printf("Error: Unsupported leaf node type.\n");
                exit(EXIT_FAILURE);
        }
    }
    float left_value = evaluate_tree((Node *) root->left);
    float right_value = evaluate_tree((Node *) root->right);
    switch (root->type) {
        case PLUS:
            return left_value + right_value;
        case MINUS:
            return left_value - right_value;
        case MULTIPLY:
            return left_value * right_value;
        case DIVIDE:
            return left_value / right_value;
        case AND:
            return left_value && right_value;
        case OR:
            return left_value || right_value;
        case NOT:
            return !left_value;
        case EQUAL:
            return left_value == right_value;
        case NOT_EQUAL:
            return left_value != right_value;
        case LESS:
            return left_value < right_value;
        case LESS_EQUAL:
            return left_value <= right_value;
        case GREATER:
            return left_value > right_value;
        case GREATER_EQUAL:
            return left_value >= right_value;
        default:
            printf("Error: Unsupported operator node type.\n");
            exit(EXIT_FAILURE);
    }
}


Node *handle_declare(Token *tokens, int *position) {
    TokenType var_type;
    Token *declare_token = match(tokens, position, DECLARE);
    if (declare_token == NULL) {
        add_error("EXPECTED DECLARE", tokens[*position].line_num);
        return NULL;
    }
    Token *variable_token = match(tokens, position, VARIABLE);
    if (variable_token == NULL) {
        add_error("EXPECTED VARIABLE", tokens[*position].line_num);
        return NULL;
    }
    if (is_variable_exist(variable_token->value)) {
        add_error("VARIABLE ALREADY EXIST", tokens[*position].line_num);
        return NULL;
    }
    Token *colon_token = match(tokens, position, COLON);
    if (colon_token == NULL) {
        add_error("EXPECTED COLON", tokens[*position].line_num);
        return NULL;
    }

    Node *declare_node = init_node(declare_token->value, declare_token->type);
    Node *variable_node = init_node(variable_token->value, variable_token->type);


    Token *type_token = handle_type(tokens, position, &var_type);
    if(type_token == NULL) {
        return NULL;
    }

    Token *assign_token = match(tokens, position, ASSIGN);
    if (assign_token == NULL) {
        add_error("EXPECTED ASSIGN", tokens[*position].line_num);
        return NULL;
    }


    Node *right_formula_node = parse_formula(tokens, position,0);
    Node *bin_node = create_bin_node(declare_node, variable_node, right_formula_node);

    if (!check_leaf_same_type(right_formula_node, var_type)) {
        add_error("EXPECTED CORRECT TYPE", tokens[*position].line_num);
        return NULL;
    }


    if (var_type == STRING) {
        add_variable_string(variable_token, right_formula_node->value, var_type);
    } else {
        float result = evaluate_tree(right_formula_node);
        printf("\n%f", result);
        add_variable_num(variable_token, result, var_type);
    }
    variable_node->variable = search_var_in_stack(variable_node->value);
    return bin_node;
}

Node *handle_func(Token *tokens, int *position) {
    hashMap *func_symtable = initializeHashMap();
    push_stack(stack_symtable,func_symtable);

    Token *func_token = match(tokens, position, FUNC);
    if (func_token == NULL) {
        add_error("EXPECTED FUNC", tokens[*position].line_num);
        return NULL;
    }

    Token *func_name_token = match(tokens, position, VARIABLE);
    if (func_name_token == NULL) {
        add_error("EXPECTED FUNCTION NAME", tokens[*position].line_num);
        return NULL;
    }

    Token *l_par_token = match(tokens, position, LPAR);
    if (l_par_token == NULL) {
        add_error("EXPECTED (", tokens[*position].line_num);
        return NULL;
    }

    Token **param_tokens = malloc(sizeof(Token *) * 50);
    TokenType *param_types = malloc(sizeof(TokenType) * 50);
    int param_count = 0;

    while (tokens[*position].type != RPAR && tokens[*position].type != END_OF_TOKENS) {
        Token *param_name_token = match(tokens, position, VARIABLE);
        if (param_name_token == NULL) {
            add_error("EXPECTED PARAMETER NAME", tokens[*position].line_num);
            return NULL;
        }
        Token *colon_token = match(tokens, position, COLON);
        if (colon_token == NULL) {
            add_error("EXPECTED COLON", tokens[*position].line_num);
            return NULL;
        }
        Token *type_token = match(tokens, position, INT_T);
        if (type_token == NULL) {
            type_token = match(tokens, position, FLOAT_T);
        }
        if (type_token == NULL) {
            type_token = match(tokens, position, STRING_T);
        }
        if (type_token == NULL) {
            add_error("EXPECTED PARAMETER TYPE", tokens[*position].line_num);
            return NULL;
        }
        param_tokens[param_count] = param_name_token;

        switch (type_token->type) {
            case INT_T:
                param_types[param_count] = INT;
                break;
            case FLOAT_T:
                param_types[param_count] = FLOAT;
                break;
            case BOOL_T:
                param_types[param_count] = BOOL;
                break;
            case STRING_T:
                param_types[param_count] = STRING;
                break;
        }

        param_count++;

        if (tokens[*position].type == COMMA) {
            (*position)++;
        } else if (tokens[*position].type != RPAR) {
            add_error("EXPECTED COMMA OR )", tokens[*position].line_num);
            return NULL;
        }
    }


    Token *r_par_token = match(tokens, position, RPAR);
    if (r_par_token == NULL) {
        add_error("EXPECTED )", tokens[*position].line_num);
        return NULL;
    }

    Token *lbrace_token = match(tokens, position, LBRACE);
    if (lbrace_token == NULL) {
        add_error("EXPECTED {", tokens[*position].line_num);
        return NULL;
    }

    Node **statements = malloc(sizeof(Node *) * 100);
    int statement_count = 0;
    while (tokens[*position].type != RBRACE) {
        statements[statement_count] = parse_expression(tokens, position);
        if (match(tokens, position, SEMICOLON) == NULL) {
            add_error("EXPECTED SEMICOLON", tokens[*position].line_num);
            return NULL;
        }
        statement_count++;
    }

    Token *rbrace_token = match(tokens, position, RBRACE);
    if (rbrace_token == NULL) {
        add_error("EXPECTED }", tokens[*position].line_num);
        return NULL;
    }

    FunctionNode *func_node = malloc(sizeof(FunctionNode));
    func_node->name = func_name_token->value;
    func_node->param_types = param_types;
    func_node->param_names = malloc(sizeof(char *) * param_count);
    for (int i = 0; i < param_count; i++) {
        func_node->param_names[i] = param_tokens[i]->value;
    }
    func_node->param_count = param_count;
    func_node->statements = statements;
    func_node->statement_count = statement_count;


    Variable *variable = malloc(sizeof(Variable));
    variable->token = func_name_token;
    variable->var_type = FUNC;
    variable->scope = 0;
    variable->func_value = func_node;
    variable->is_func = 1;

    insert(global_symtable, func_name_token->value, variable);


    Node *node = init_node(func_name_token->value, FUNC);
    node->func_node = func_node;

    func_node->curr_stack_sym = stack_clone(stack_symtable);

    pop_stack(stack_symtable);
    return node;
}


Node *handle_func_call(Token *tokens, int *position) {
    Token *func_call_token = match(tokens, position, FUNC_CALL);
    if (func_call_token == NULL) {
        add_error("EXPECTED FUNC", tokens[*position].line_num);
        return NULL;
    }


    Token *func_name_token = match(tokens, position, VARIABLE);
    if (func_name_token == NULL) {
        add_error("EXPECTED FUNCTION NAME", tokens[*position].line_num);
        return NULL;
    }

    Variable *func_variable = search_var_in_stack(func_name_token->value);
    if (func_variable == NULL) {
        add_error("FUNCTION NOT DEFINED", func_name_token->line_num);
        return NULL;
    }

    if (!func_variable->is_func) {
        add_error("CALLED NAME IS NOT A FUNCTION", func_name_token->line_num);
        return NULL;
    }

    FunctionNode *function_def = func_variable->func_value;
    if (function_def == NULL) {
        add_error("FUNCTION DEFINITION NOT FOUND", func_name_token->line_num);
        return NULL;
    }


    Token *l_par_token = match(tokens, position, LPAR);
    if (l_par_token == NULL) {
        add_error("EXPECTED (", tokens[*position].line_num);
        return NULL;
    }

    Node **args = malloc(sizeof(Node *) * 50);
    int arg_count = 0;

    while (tokens[*position].type != RPAR && tokens[*position].type != END_OF_TOKENS) {
        Node *arg_node = parse_formula(tokens, position, 0);
        if (arg_node == NULL) {
            add_error("EXPECTED EXPRESSION OR )", tokens[*position].line_num);
            return NULL;
        }
        args[arg_count] = arg_node;
        arg_count++;

        if (tokens[*position].type == COMMA) {
            (*position)++;
        } else if (tokens[*position].type != RPAR) {
            add_error("EXPECTED COMMA OR )", tokens[*position].line_num);
            return NULL;
        }
    }

    Token *r_par_token = match(tokens, position, RPAR);
    if (r_par_token == NULL) {
        add_error("EXPECTED )", tokens[*position].line_num);
        return NULL;
    }

    if (arg_count != function_def->param_count) {
        char *buffer = malloc(sizeof(char)* 1024);
        snprintf(buffer, 1024, "INCORRECT NUMBER OF ARGUMENTS (NEED %d, GIVEN %d)", function_def->param_count, arg_count);

        add_error(buffer, func_name_token->line_num);
        return NULL;
    }


    for (int i = 0; i < arg_count; i++) {
        Node *exp =  args[i];

        while(exp->left != NULL){
            exp = (Node *) exp->left;
        }
        TokenType arg_type = exp->type;
        if (arg_type == VARIABLE){
            arg_type = getVarType(args[i]->value);
        }

        TokenType func_type = function_def->param_types[i];
        if (arg_type != func_type) {
            add_error("INCORRECT ARGUMENT TYPE", tokens[*position].line_num);
            return NULL;
        }
    }




    FuncCallNode *func_node = malloc(sizeof(FunctionNode));


    func_node->func_name = func_name_token->value;
    func_node->args = args;
    func_node->arg_count = arg_count;

    Node *call_node = init_node(func_name_token->value, FUNC_CALL);
    call_node->func_call_node = (struct FuncCallNode *) func_node;


    return call_node;
}


Node *handle_if(Token *tokens, int *position) {
    hashMap *curr_symtable = initializeHashMap();
    push_stack(stack_symtable,curr_symtable);

    Token *if_token = match(tokens, position, IF);
    if (if_token == NULL) {
        add_error("EXPECTED IF", tokens[*position].line_num);
        return NULL;
    }

    Token *lpar_token = match(tokens, position, LPAR);
    if (lpar_token == NULL) {
        add_error("EXPECTED ( AFTER IF", tokens[*position].line_num);
        return NULL;
    }

    Node *condition_node = parse_formula(tokens, position, 0);
    if (condition_node == NULL) {
        add_error("EXPECTED CONDITION AFTER (", tokens[*position].line_num);
        return NULL;
    }


    Token *rpar_token = match(tokens, position, RPAR);
    if (rpar_token == NULL) {
        add_error("EXPECTED ) AFTER CONDITION", tokens[*position].line_num);
        return NULL;
    }

    Token *lbrace_token = match(tokens, position, LBRACE);
    if (lbrace_token == NULL) {
        add_error("EXPECTED { AFTER )", tokens[*position].line_num);
        return NULL;
    }

    Node **then_statements = malloc(sizeof(Node *) * 100);
    int then_statement_count = 0;
    while (tokens[*position].type != RBRACE) {
        then_statements[then_statement_count] = parse_expression(tokens, position);
        if (match(tokens, position, SEMICOLON) == NULL) {
            add_error("EXPECTED SEMICOLON IN THEN BLOCK", tokens[*position].line_num);
            return NULL;
        }
        then_statement_count++;
    }

    Token *rbrace_token = match(tokens, position, RBRACE);
    if (rbrace_token == NULL) {
        add_error("EXPECTED } AFTER THEN BLOCK", tokens[*position].line_num);
        return NULL;
    }

    Node **else_statements = NULL;
    int else_statement_count = 0;
    Token *else_token = match(tokens, position, ELSE);
    if (else_token != NULL) {
        Token *else_lbrace_token = match(tokens, position, LBRACE);
        if (else_lbrace_token == NULL) {
            add_error("EXPECTED { AFTER ELSE", tokens[*position].line_num);
            return NULL;
        }

        else_statements = malloc(sizeof(Node *) * 100);
        while (tokens[*position].type != RBRACE) {
            else_statements[else_statement_count] = parse_expression(tokens, position);
            if (match(tokens, position, SEMICOLON) == NULL) {
                add_error("EXPECTED SEMICOLON IN ELSE BLOCK", tokens[*position].line_num);
                return NULL;
            }
            else_statement_count++;
        }

        Token *else_rbrace_token = match(tokens, position, RBRACE);
        if (else_rbrace_token == NULL) {
            add_error("EXPECTED } AFTER ELSE BLOCK", tokens[*position].line_num);
            return NULL;
        }
    }

    IfNode *if_node = malloc(sizeof(IfNode));
    if_node->condition = condition_node;
    if_node->then_statements = then_statements;
    if_node->then_statement_count = then_statement_count;
    if_node->else_statements = else_statements;
    if_node->else_statement_count = else_statement_count;


    if_node->curr_stack_sym = stack_clone(stack_symtable);

    Node *node = init_node("if", IF);
    node->if_node = (struct IfNode *) if_node;

    pop_stack(stack_symtable);
    return node;
}

Node *handle_while(Token *tokens, int *position) {
    hashMap *curr_symtable = initializeHashMap();
    push_stack(stack_symtable,curr_symtable);


    Token *while_token = match(tokens, position, WHILE);
    if (while_token == NULL) {
        add_error("EXPECTED WHILE", tokens[*position].line_num);
        return NULL;
    }

    Token *lpar_token = match(tokens, position, LPAR);
    if (lpar_token == NULL) {
        add_error("EXPECTED ( AFTER WHILE", tokens[*position].line_num);
        return NULL;
    }

    Node *condition_node = parse_formula(tokens, position, 0);
    if (condition_node == NULL) {
        add_error("EXPECTED CONDITION AFTER (", tokens[*position].line_num);
        return NULL;
    }

    Token *rpar_token = match(tokens, position, RPAR);
    if (rpar_token == NULL) {
        add_error("EXPECTED ) AFTER CONDITION", tokens[*position].line_num);
        return NULL;
    }

    Token *lbrace_token = match(tokens, position, LBRACE);
    if (lbrace_token == NULL) {
        add_error("EXPECTED { AFTER )", tokens[*position].line_num);
        return NULL;
    }

    Node **body_statements = malloc(sizeof(Node *) * 100);
    int body_statement_count = 0;
    while (tokens[*position].type != RBRACE) {
        body_statements[body_statement_count] = parse_expression(tokens, position);
        if (match(tokens, position, SEMICOLON) == NULL) {
            add_error("EXPECTED SEMICOLON IN WHILE BODY", tokens[*position].line_num);
            return NULL;
        }
        body_statement_count++;
    }

    Token *rbrace_token = match(tokens, position, RBRACE);
    if (rbrace_token == NULL) {
        add_error("EXPECTED } AFTER WHILE BODY", tokens[*position].line_num);
        return NULL;
    }

    WhileNode *while_node = malloc(sizeof(WhileNode));
    while_node->condition = condition_node;
    while_node->body_statements = body_statements;
    while_node->body_statement_count = body_statement_count;

    Node *node = init_node("while", WHILE);
    node->while_node = (struct WhileNode *) while_node;

    while_node->curr_stack_sym = stack_clone(stack_symtable);

    pop_stack(stack_symtable);
    return node;
}

Node *parse_expression(Token *tokens, int *position) {
    switch (tokens[*position].type) {
        case VARIABLE:
            return handle_variable(tokens, position);
        case WRITE:
            return parse_print(tokens, position);
        case DECLARE:
            return handle_declare(tokens, position);
        case FUNC:
            return handle_func(tokens, position);
        case IF:
            return handle_if(tokens, position);
        case WHILE:
            return handle_while(tokens,position);
        case FUNC_CALL:
            return handle_func_call(tokens,position);
    }
    return NULL;
}

int tokens_size(Token *tokens){
    int i = 0;
    while(tokens[i].type != END_OF_TOKENS){
        i++;
    }
    return i;
}

Node **parse_code(Token *tokens) {
    global_symtable = initializeHashMap();
    stack_symtable =  stack_create();
    push_stack(stack_symtable,global_symtable);
    cur_stack = stack_symtable;
    all_variables = (All_Variables *) calloc(50,sizeof (All_Variables));
    all_variables->ind = 0;


    int position = 0;
    Node **code_strings = malloc(sizeof(Node *) * 100);
    int pos_list = 0;

    while (tokens[position].type != END_OF_TOKENS) {
        Node *code_string_node = parse_expression(tokens, &position);
        if (code_string_node == NULL) {
            code_strings[pos_list] = init_node(0, NULL_NODE);
            pos_list++;
            while(tokens[position].type != SEMICOLON && position  < tokens_size(tokens) ){
                position++;
            }
        }

        if (match(tokens, &position, SEMICOLON) == NULL) {
            add_error("EXPECTED SEMICOLON", tokens[position].line_num);
        }
        code_strings[pos_list] = code_string_node;
        pos_list++;
    }
    code_strings[pos_list] = init_node(0, NULL_NODE);
    return code_strings;
}

Token *match(Token *tokens, int *position, TokenType type_excepted) {
    if (tokens[*position].type == END_OF_TOKENS || tokens[*position].type != type_excepted) {
        return NULL;
    }
    (*position)++;
    return &tokens[(*position) - 1];
}
