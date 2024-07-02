//
// Created by Likva32 on 30.05.2024.
//

#include "generate_code.h"

Register registers[NUM_OF_REGISTERS];
FILE *dest_file;
FILE *data_section_file;
FILE *text_section_file;
FILE *function_block_file;
FILE *curr_file;
int is_in_func;

void code_generator_output(FILE *file, char *string) {
    fprintf(file, "%s", string);
}


void code_generator_init() {
    dest_file = fopen("test.asm", "w");
    data_section_file = tmpfile();
    text_section_file = tmpfile();
    function_block_file = tmpfile();
    cur_stack = stack_symtable;
    is_in_func = 0;

    int r = 0;
    strncpy(registers[r].name, R10, REGISTER_NAME_LENGTH);
    registers[r++].used = false;
    strncpy(registers[r].name, R11, REGISTER_NAME_LENGTH);
    registers[r++].used = false;
    strncpy(registers[r].name, R12, REGISTER_NAME_LENGTH);
    registers[r++].used = false;
    strncpy(registers[r].name, R13, REGISTER_NAME_LENGTH);
    registers[r++].used = false;
    strncpy(registers[r].name, R14, REGISTER_NAME_LENGTH);
    registers[r++].used = false;
    strncpy(registers[r].name, R15, REGISTER_NAME_LENGTH);
    registers[r++].used = false;
}

int code_generator_register_alloc() {
    for (int r = 0; r < NUM_OF_REGISTERS; r++) {
        if (!registers[r].used) {
            registers[r].used = true;
            return r;
        }
    }
    printf("%s", "no register");
    exit(1);
}

int get_param_index(FunctionNode *func_node, const char *param_name) {
    for (int i = 0; i < func_node->param_count; i++) {
        if (strcmp(func_node->param_names[i], param_name) == 0) {
            return i;
        }
    }
    return -1;
}

char *code_generator_label_create() {
    static int label_num = 0;
    char *label = (char *) calloc(LABEL_NAME_LENGTH, sizeof(char));
    sprintf(label, LABEL_FORMAT, label_num);
    label_num++;
    return label;
}

void code_generator_output_data_segment() {
    curr_file = data_section_file;
    fprintf(curr_file, "%s", "section .data\n");

    All_Variables *test = all_variables;
    for (int i = 0; i < test->ind; i++) {
        Variable *var = all_variables->arr[i];
        fprintf(curr_file, "\t%s_%d:", var->token->value, var->scope);

        if (var->var_type == STRING) {
            fprintf(curr_file, " db %s, 0\n",
                    var->string_value);
        } else if (var->var_type == INT) {
            fprintf(curr_file, " dq %d\n", var->int_value);
        } else if (var->var_type == FLOAT) {
            fprintf(curr_file, " dq %f\n", var->float_value);
        }
    }
    fprintf(data_section_file, "%s", "\n");
    curr_file = text_section_file;
}

int code_generator_expr(Node *node) {
    char buffer[256];
    int left_reg, right_reg;

    if (node->left == NULL && node->right == NULL) {
        int reg = code_generator_register_alloc();
        switch (node->type) {
            case INT:
                snprintf(buffer, sizeof(buffer), MOV, registers[reg].name, node->value);
                break;
            case FLOAT:
                snprintf(buffer, sizeof(buffer), MOVSS, registers[reg].name, node->value);
                break;
            case VARIABLE:{
                Variable *var = search_var_in_stack(node->value);
                snprintf(buffer, sizeof(buffer), "\tmov %s, [%s_%d]\n", registers[reg].name,
                         node->value, var->scope);
                break;
                //exit(EXIT_FAILURE);
            }
            default:

                printf("\nError: Unsupported leaf node type %d.\n", node->type);
                printf("test %s.\n", node->value);
                exit(EXIT_FAILURE);
        }
        code_generator_output(curr_file, buffer);
        return reg;
    }
    left_reg = code_generator_expr((Node *) node->left);
    right_reg = code_generator_expr((Node *) node->right);

    switch (node->type) {
        case PLUS:
            snprintf(buffer, sizeof(buffer), ADD, registers[left_reg].name, registers[right_reg].name);
            break;
        case MINUS:
            snprintf(buffer, sizeof(buffer), SUB, registers[left_reg].name, registers[right_reg].name);
            break;
        case MULTIPLY:
            snprintf(buffer, sizeof(buffer), IMUL, registers[left_reg].name, registers[right_reg].name);
            break;
        case DIVIDE:
            snprintf(buffer, sizeof(buffer), "\tmov rax, %s\n", registers[left_reg].name);
            code_generator_output(curr_file, buffer);
            code_generator_output(curr_file, "\tcqo\n");
            snprintf(buffer, sizeof(buffer), IDIV, registers[right_reg].name);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "\tmov %s, rax\n", registers[left_reg].name);

            break;

        case GREATER:
        case GREATER_EQUAL:
        case LESS:
        case LESS_EQUAL:
        case EQUAL:
        case NOT_EQUAL: {
            char *jump_instr;
            switch (node->type) {
                case GREATER:
                    jump_instr = "jg";
                    break;
                case GREATER_EQUAL:
                    jump_instr = "jge";
                    break;
                case LESS:
                    jump_instr = "jl";
                    break;
                case LESS_EQUAL:
                    jump_instr = "jle";
                    break;
                case EQUAL:
                    jump_instr = "je";
                    break;
                case NOT_EQUAL:
                    jump_instr = "jne";
                    break;
            }

            char *compare_label = code_generator_label_create();
            char *end_label = code_generator_label_create();

            snprintf(buffer, sizeof(buffer), CMP, registers[left_reg].name, registers[right_reg].name);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "\t%s %s\n", jump_instr, compare_label);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "\tmov %s, 0\n", registers[left_reg].name);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "\tjmp %s\n", end_label);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "%s:\n", compare_label);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "\tmov %s, 1\n", registers[left_reg].name);
            code_generator_output(curr_file, buffer);
            snprintf(buffer, sizeof(buffer), "%s:\n", end_label);
            code_generator_output(curr_file, buffer);

            free(compare_label);
            free(end_label);
            break;
        }
        case NOT:
            snprintf(buffer, sizeof(buffer), "\tnot %s\n", registers[left_reg].name);
            break;
        case OR:
            snprintf(buffer, sizeof(buffer), "\tor %s, %s\n", registers[left_reg].name, registers[right_reg].name);
            break;
        case AND:
            snprintf(buffer, sizeof(buffer), "\tand %s, %s\n", registers[left_reg].name, registers[right_reg].name);
            break;
        default:
            printf("Error: Unsupported operator node type.\n");
            exit(EXIT_FAILURE);
    }
    code_generator_output(curr_file, buffer);
    registers[right_reg].used = 0;
    return left_reg;
}

void code_generator_assign(Node *node) {
    char buffer[256];
    if (node->right != NULL) {
        if (((Node *) node->right)->left == NULL && ((Node *) node->right)->right == NULL) {
            Node *node_var = ((Node *) node->left);
            if(node_var->variable->var_type != STRING){
                snprintf(buffer, sizeof(buffer), "\tmov qword [%s_%d], %s\n\n", node_var->value,
                         node_var->variable->scope, ((Node *) node->right)->value);
                code_generator_output(curr_file, buffer);
            }


        } else {

            int reg = code_generator_expr((Node *) node->right);
            Node *node_var = ((Node *) node->left);
            snprintf(buffer, sizeof(buffer), "\tmov qword [%s_%d], %s\n\n", node_var->value,
                     node_var->variable->scope, registers[reg].name);
            code_generator_output(curr_file, buffer);
            registers[reg].used = 0;
        }

    }

    Node *variable_node = ((Node *) node->left);
    if (getVarType(variable_node->value) == INT || getVarType(variable_node->value) == FLOAT) {
        float result = evaluate_tree((Node *) node->right);
        setVarValue(variable_node->value, &result, getVarType(variable_node->value));
    } else {
        setVarValue(variable_node->value, ((Node *) node->right)->value, getVarType(variable_node->value));
    }

}

void code_generator_if(Node *if_node) {
    char buffer[256];
    char *else_label = code_generator_label_create();
    char *end_label = code_generator_label_create();
    cur_stack = ((IfNode *) if_node->if_node)->curr_stack_sym;


   int cond_reg = code_generator_expr(((IfNode *) if_node->if_node)->condition);



    snprintf(buffer, sizeof(buffer), CMP, registers[cond_reg].name, "0");
    code_generator_output(curr_file, buffer);
    snprintf(buffer, sizeof(buffer), JE, else_label);
    code_generator_output(curr_file, buffer);
    registers[cond_reg].used = 0;

    for (int i = 0; i < ((IfNode *) if_node->if_node)->then_statement_count; i++) {
        code_generator_stmt(((IfNode *) if_node->if_node)->then_statements[i]);
    }
    snprintf(buffer, sizeof(buffer), "\tjmp %s\n", end_label);
    code_generator_output(curr_file, buffer);

    snprintf(buffer, sizeof(buffer), "%s:\n", else_label);
    code_generator_output(curr_file, buffer);
    if (((IfNode *) if_node->if_node)->else_statements != NULL) {
        for (int i = 0; i < ((IfNode *) if_node->if_node)->else_statement_count; i++) {
            code_generator_stmt(((IfNode *) if_node->if_node)->else_statements[i]);
        }
    }

    snprintf(buffer, sizeof(buffer), "%s:\n", end_label);
    code_generator_output(curr_file, buffer);

    cur_stack = stack_symtable;
    free(else_label);
    free(end_label);
}

void code_generator_while(Node *while_node) {
    char buffer[256];
    char *start_label = code_generator_label_create();
    char *end_label = code_generator_label_create();
    cur_stack = ((WhileNode *) while_node->while_node)->curr_stack_sym;

    printf("\n\n");
    print_tree(((IfNode *) while_node->while_node)->condition, 5 , "cond_while");
    printf("\n\n");

    snprintf(buffer, sizeof(buffer), "%s:\n", start_label);
    code_generator_output(curr_file, buffer);

    int cond_reg = code_generator_expr(((WhileNode *) while_node->while_node)->condition);
    snprintf(buffer, sizeof(buffer), "\tcmp %s, 0\n", registers[cond_reg].name);
    code_generator_output(curr_file, buffer);
    snprintf(buffer, sizeof(buffer), "\tje %s\n", end_label);
    code_generator_output(curr_file, buffer);
    registers[cond_reg].used = 0;

    for (int i = 0; i < ((WhileNode *) while_node->while_node)->body_statement_count; i++) {
        code_generator_stmt(((WhileNode *) while_node->while_node)->body_statements[i]);
    }
    snprintf(buffer, sizeof(buffer), "\tjmp %s\n", start_label);
    code_generator_output(curr_file, buffer);

    snprintf(buffer, sizeof(buffer), "%s:\n", end_label);
    code_generator_output(curr_file, buffer);

    free(start_label);
    free(end_label);
}

void code_generator_write(Node *node) {
    char buffer[256];
    static int var_label_count = 0;
    if ((Node *) node->left != NULL){
        TokenType key = ((Node *) node->left)->type;
        switch (key) {
            case VARIABLE: {
                Variable *var = search_var_in_stack(((Node *) node->left)->value);
                switch (var->var_type) {
                    case INT: {
                        char num_str[20];
                        snprintf(num_str, sizeof(num_str), "%d", var->int_value);


                        snprintf(buffer, sizeof(buffer), "\tvar_str%d: db '%s', 0\n", var_label_count, num_str);
                        code_generator_output(data_section_file, buffer);

                        snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(num_str) + 1);
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                        code_generator_output(curr_file, buffer);
                        break;
                    }
                    case FLOAT:{
                        char num_str[20];
                        snprintf(num_str, sizeof(num_str), "%d", var->int_value);

                        snprintf(buffer, sizeof(buffer), "\tvar_str%d: db '%s', 0\n", var_label_count, num_str);
                        code_generator_output(data_section_file, buffer);



                        snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(num_str) + 1);
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                        code_generator_output(curr_file, buffer);
                        var_label_count++;
                        break;
                    }

                    case STRING:{
                        snprintf(buffer, sizeof(buffer), "\tvar_str%d: db %s, 0\n", var_label_count,var->string_value);
                        code_generator_output(data_section_file, buffer);

                        snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(var->string_value) -1);
                        code_generator_output(curr_file, buffer);
                        snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                        code_generator_output(curr_file, buffer);
                        var_label_count++;
                        break;
                    }
                }
                var_label_count++;
                break;
            }
            case INT: {


                snprintf(buffer, sizeof(buffer), "\tvar_str%d: db '%s', 0\n", var_label_count,
                         ((Node *) node->left)->value);
                code_generator_output(data_section_file, buffer);

                snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(((Node *) node->left)->value) + 1);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                code_generator_output(curr_file, buffer);
                var_label_count++;
                break;
            }
            case FLOAT:
                snprintf(buffer, sizeof(buffer), "\tvar_str%d: db '%s', 0\n", var_label_count,
                         ((Node *) node->left)->value);
                code_generator_output(data_section_file, buffer);

                snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(((Node *) node->left)->value) + 1);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                code_generator_output(curr_file, buffer);
                var_label_count++;
                break;

            case STRING:
                snprintf(buffer, sizeof(buffer), "\tvar_str%d: db %s, 0\n", var_label_count,
                         ((Node *) node->left)->value);
                code_generator_output(data_section_file, buffer);


                snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(((Node *) node->left)->value) -1);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                code_generator_output(curr_file, buffer);
                var_label_count++;
                break;
            case MULTIPLY:
            case PLUS:
            case DIVIDE:
            case MINUS:

                snprintf(buffer, sizeof(buffer), "\tvar_str%d: db '%f', 0\n", var_label_count,
                         evaluate_tree(((Node *) node->left)));
                code_generator_output(data_section_file, buffer);

                snprintf(buffer, sizeof(buffer), "\tmov eax, 4\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ebx, 1\n");
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov ecx, var_str%d\n", var_label_count);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tmov edx, %llu\n", strlen(((Node *) node->left)->value) + 1);
                code_generator_output(curr_file, buffer);
                snprintf(buffer, sizeof(buffer), "\tint 80h\n");
                code_generator_output(curr_file, buffer);
                var_label_count++;
                break;
            default:
                printf("Error: Unsupported data type for print operation.\n");
                exit(EXIT_FAILURE);
        }
        code_generator_output(curr_file, "\n");
    }


}

void code_generator_generate_function(FunctionNode *func_node) {
    cur_stack =  func_node->curr_stack_sym;

    curr_file = function_block_file;
    is_in_func = 1;

    char buffer[256];

    snprintf(buffer, sizeof(buffer), "%s:\n",  func_node->name);
    code_generator_output(curr_file, buffer);
    snprintf(buffer, sizeof(buffer), "\tpush rbp\n");
    code_generator_output(curr_file, buffer);
    snprintf(buffer, sizeof(buffer), "\tmov rbp, rsp\n\n");
    code_generator_output(curr_file, buffer);



//    int param_offset = 16;
//    for (int i = 0; i < func_node->param_count; i++) {
//        snprintf(buffer, sizeof(buffer), "\tmov [rbp - %d], %s\n",  param_offset, func_node->param_names[i]);
//        code_generator_output(curr_file, buffer);
//        param_offset += 8;  // Assuming parameters are 8 bytes (64-bit)
//    }

    snprintf(buffer, sizeof(buffer), "\n");
    code_generator_output(curr_file, buffer);


    for (int i = 0; i < func_node->statement_count; i++) {
        code_generator_stmt(func_node->statements[i]);
    }

    snprintf(buffer, sizeof(buffer), "\tpop rbp\n");
    code_generator_output(curr_file, buffer);
    snprintf(buffer, sizeof(buffer), "\tret\n");
    code_generator_output(curr_file, buffer);

    curr_file = text_section_file;
    is_in_func = 0;
    cur_stack = stack_symtable;
}

void code_generator_generate_func_call(FuncCallNode *func_call) {
    char buffer[256];

    for (int i = func_call->arg_count - 1; i >= 0; i--) {
        int reg =  code_generator_expr(func_call->args[i]);

        snprintf(buffer, sizeof(buffer), "\tmov rax, %s\n\n", registers[reg].name);

        snprintf(buffer, sizeof(buffer), "\tpush rax\n");
        code_generator_output(curr_file, buffer);
    }
    snprintf(buffer, sizeof(buffer), "\tcall %s\n\n", func_call->func_name);
    code_generator_output(curr_file, buffer);

    snprintf(buffer, sizeof(buffer), "\tadd rsp, %d\n", func_call->arg_count * 8);
    code_generator_output(curr_file, buffer);
}

void code_generator_stmt(Node *parse_tree) {
    switch (parse_tree->type) {
        case DECLARE:
        case ASSIGN:
            code_generator_assign(parse_tree);
            break;
        case IF:
            code_generator_if(parse_tree);
            break;
        case WHILE:
            code_generator_while(parse_tree);
            break;
        case FUNC:
            code_generator_generate_function(parse_tree->func_node);
            break;
        case FUNC_CALL:
            code_generator_generate_func_call(((FuncCallNode*)parse_tree->func_call_node));
            break;
        case WRITE:
            code_generator_write(parse_tree);
            break;
    }
}

void code_generator_generate(Node **arr_parse_tree) {
    code_generator_init();

    code_generator_output_data_segment();

    code_generator_output(curr_file, "section .text\n");
    code_generator_output(curr_file, "\t global _start\n");
    code_generator_output(curr_file, "_start:\n");
    code_generator_output(curr_file, "\tpush rbp\n");
    code_generator_output(curr_file, "\tmov rbp, rsp\n");
    code_generator_output(curr_file, "\tsub rsp, 1000h\n\n");
    int i = 0;
    while (arr_parse_tree[i]->type != NULL_NODE) {
        code_generator_stmt(arr_parse_tree[i]);
        i++;
    }

    code_generator_output(curr_file, "\n\tmov rsp, rbp\n");
    code_generator_output(curr_file, "\tpop rbp\n");
    code_generator_output(curr_file, "\tmov rax, 0\n");
    code_generator_output(curr_file, "\tint 80h\n\n");
    code_generator_output(curr_file, "\tmov eax,1\n");
    code_generator_output(curr_file, "\tmov ebx,0\n");
    code_generator_output(curr_file, "\tint 80h\n\n");

    rewind(data_section_file);
    char ch;
    while ((ch = fgetc(data_section_file)) != EOF) {
        fputc(ch, dest_file);
    }


    rewind(text_section_file);
    while ((ch = fgetc(text_section_file)) != EOF) {
        fputc(ch, dest_file);
    }

    rewind(function_block_file);
    while ((ch = fgetc(function_block_file)) != EOF) {
        fputc(ch, dest_file);
    }
    fclose(dest_file);

}
