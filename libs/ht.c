#include "ht.h"
#include "graph.h"
#include "../parser.h"


Variable *create_variable_num(Token *token, float value_var, TokenType var_type){
    Variable *var = malloc(sizeof(Variable));
    var->var_type = var_type;
    var->token = token;
    var->is_func = 0;
    if(var_type == INT){
        var->int_value = (int)value_var;
    }
    if(var_type == FLOAT){
        var->float_value = value_var;
    }
    var->scope = cur_stack->top;
    return var;
}

Variable *create_variable_string(Token *token, char *value_var, TokenType var_type){
    Variable *var = malloc(sizeof(Variable));
    var->var_type = var_type;
    var->token = token;
    var->is_func = 0;
    if(var_type == STRING){
        var->string_value = value_var;
    }
    var->scope = cur_stack->top;
    return var;
}


void setNode(node* node, char* key, Variable* value)
{
    node->key = key;
    node->value = value;
    node->next = NULL;
}


hashMap* initializeHashMap()
{
    hashMap* mp = (hashMap*)malloc(sizeof(hashMap));

    mp->capacity = 1000;
    mp->numOfElements = 0;

    mp->arr = (struct node**)calloc(sizeof(struct node*)
                                    , mp->capacity);
    return mp;
}

int hashFunction(hashMap* mp, char* key)
{
    int bucketIndex;
    int sum = 0, factor = 31;
    for (int i = 0; i < strlen(key); i++) {
        sum = ((sum % mp->capacity)
               + (((int)key[i]) * factor) % mp->capacity)
              % mp->capacity;

        factor = ((factor % __INT16_MAX__)
                  * (31 % __INT16_MAX__))
                 % __INT16_MAX__;
    }

    bucketIndex = sum;
    return bucketIndex;
}

void insert(hashMap* mp, char* key, Variable* value)
{
    int bucketIndex = hashFunction(mp, key);
    node* newNode = (node*)malloc(

            sizeof(node));

    setNode(newNode, key, value);

    if (mp->arr[bucketIndex] == NULL) {
        mp->arr[bucketIndex] = newNode;
    }

    else {

        newNode->next = mp->arr[bucketIndex];
        mp->arr[bucketIndex] = newNode;
    }
}

void delete (hashMap* mp, char* key)
{

    int bucketIndex = hashFunction(mp, key);

    node* prevNode = NULL;

    node* currNode = (node *) mp->arr[bucketIndex];

    while (currNode != NULL) {
        if (strcmp(key, currNode->key) == 0) {
            if (currNode == mp->arr[bucketIndex]) {
                mp->arr[bucketIndex] = currNode->next;
            }

            else {
                prevNode->next = currNode->next;
            }
            free(currNode);
            break;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
}

Variable* search(hashMap* mp, char* key)
{
    int bucketIndex = hashFunction(mp, key);

    node* bucketHead = (node *) mp->arr[bucketIndex];
    while (bucketHead != NULL) {

        if (strcmp(bucketHead->key, key) == 0) {
            return bucketHead->value;
        }
        bucketHead = bucketHead->next;
    }

    return NULL;
}

