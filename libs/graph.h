#ifndef COMPILER_GRAPH_H
#define COMPILER_GRAPH_H
#define TYPEINFO int
#define V 500
#define ASCII_LEN 127
#include <stdbool.h>

typedef enum {
    BEGINNING,
    INT,
    FLOAT,
    BOOL,
    VARIABLE,
    STRING,

    STRING_T,
    INT_T,
    FLOAT_T,
    BOOL_T,

    COMP,
    LPAR, // (
    RPAR, // )
    LBRACE, // {
    RBRACE, // }
    WRITE,
    WHILE,
    DECLARE,
    OR,
    AND,
    PLUS, // +
    MINUS, // -
    MULTIPLY, // *
    DIVIDE, // /
    SEMICOLON, // <3
    COMMA, // ,
    COLON, // :
    LESS, // <
    GREATER, // >
    LESS_EQUAL, // <=
    GREATER_EQUAL, // >=
    NOT_EQUAL, // !=
    ASSIGN, // =
    EQUAL, // ==
    NOT,
    IF,
    ELSE,
    FUNC,
    FUNC_CALL,
    NO_STATEMENT,
    GIVEBACK,
    EXIT,
    NULL_NODE,
    END_OF_TOKENS
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line_num;
} Token;

typedef struct
{
    TYPEINFO vertex[V][V];
    bool visited[V];
    TokenType type_token;
    char START;
    char END;
}Graph;

Graph* initGraph(Graph* graph);
bool isEmptyGraph (const Graph*  graph);
void join (Graph* graph, TYPEINFO  a, TYPEINFO  b);
void joinWeight (Graph* graph, TYPEINFO const a, TYPEINFO const b,TYPEINFO const weight);
void removeArc (Graph* graph, TYPEINFO const a, TYPEINFO const b);
bool isAdjacent (Graph const *graph, TYPEINFO const a, TYPEINFO  b);
bool isAdjacentUniqe (Graph const *graph, TYPEINFO const a, TYPEINFO  b);
void printGraph (Graph *graph);
bool is_no_adj(Graph const *graph, int i);
void set_visited_false(Graph* graph);


#endif //COMPILER_GRAPH_H