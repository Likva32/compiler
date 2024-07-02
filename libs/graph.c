#include <stdio.h>
#include <intrin.h>
#include "graph.h"

Graph* initGraph(Graph* graph)
{
    for (int i = 0; i < V ; i++)
    {
        graph->visited[i] = false;
        for (int j = 0; j < V; j++)
        {
            graph->vertex[i][j] = 0;
        }
    }
    return graph;
}

bool isEmptyGraph (const Graph*  graph)
{
    bool flag = true;

    for (int i = 0; i < V; i++)
    {
        for (int j = 0;j<V;j++)
        {
            flag = flag && (graph->vertex[i][j] != 1);
        }
    }
    return flag;
}

void join (Graph* graph, TYPEINFO const a, TYPEINFO  b)
{
    while(graph->visited[b] && b != ' '){
        b = b + ASCII_LEN;
    }
    graph->visited[b] = true;
    graph->vertex[a][b] = 1;
}

void joinWeight (Graph* graph, TYPEINFO const a, TYPEINFO const b,TYPEINFO const weight)
{
    graph->vertex[a][b] = weight;
}

void removeArc (Graph* graph, TYPEINFO const a, TYPEINFO const b)
{
    graph->vertex[a][b] = 0;
}

bool isAdjacentUniqe (Graph const *graph, TYPEINFO const a, TYPEINFO  b)
{
    while(graph->visited[b] && b != ' '){
        b = b + ASCII_LEN;
    }
    return graph->vertex[a][b];
}

bool isAdjacent (Graph const *graph, TYPEINFO const a, TYPEINFO  b)
{
    return graph->vertex[a][b];
}

void printGraph (Graph *graph)
{
    printf("Graph: {\n");
    set_visited_false(graph);
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if(isAdjacent(graph,i, j)) {
                printf("\t%c -> %c \n", i,j);
            }
        }
    }
    printf("}\n");
}

bool is_no_adj(Graph const *graph, int i) {
    bool no_value = true;
    int j = 0;
    while(graph->visited[i] && i != ' '){
        i = i + ASCII_LEN;
    }
    while(j++ < V && no_value) {
        if(graph->vertex[i][j] != 0)
            no_value = false;
    }
    return no_value;
}

void set_visited_false(Graph* graph)
{
    memset(graph->visited, 0, sizeof(graph->visited));

}