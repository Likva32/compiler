//
// Created by Likva32 on 21.05.2024.
//

#include "stack.h"

stack_ht* stack_create(){
    stack_ht *stack = malloc(sizeof(stack_ht));
    stack->top= -1;
    return stack;
}

hashMap *peek_stack(stack_ht *stack){
    return stack->content[stack->top];
}

void push_stack(stack_ht *stack, hashMap *element){
    stack->top++;
    stack->content[stack->top] = element;
}

hashMap *pop_stack(stack_ht *stack){
    if(stack->top != -1){
        hashMap *result = stack->content[stack->top];
        stack->top--;
        return result;
    }
    return NULL;
}

hashMap* stack_get(stack_ht *stack, int index) {
    if (index < 0 || index > stack->top) {
        return NULL;
    }
    return stack->content[index];
}

void print_tree(Node *node, int indent, char *identifier){
    if(node == NULL){
        return;
    }
    for(int i = 0; i < indent; i++){
        printf(" ");
    }
    printf("%s -> ", identifier);
    printf("%s", node->value);
    printf("\n");
    print_tree((Node *) node->left, indent + 1, "left");
    print_tree((Node *) node->right, indent + 1, "right");
}

stack_ht* stack_clone(stack_ht *original_stack) {
    if (original_stack == NULL) {
        return NULL;
    }

    stack_ht *new_stack = malloc(sizeof(stack_ht));
    if (new_stack == NULL) {
        perror("Failed to allocate memory for stack");
        exit(EXIT_FAILURE);
    }

    new_stack->top = original_stack->top;
    for (int i = 0; i <= original_stack->top; i++) {
        new_stack->content[i] = original_stack->content[i];
    }

    return new_stack;
}
