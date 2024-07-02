//
// Created by Likva32 on 12.11.2023.
//
#ifndef LINKED_LIST_LINKED_LIST_H
#define LINKED_LIST_LINKED_LIST_H
#define TYPELINKEDLIST void*
#define true 1
#define false 0
#include "..\memory_control.h"

typedef struct{
    TYPELINKEDLIST info;
    struct linked_list *next;
}linked_list;

int is_empty_linked_list(linked_list* manager);

linked_list* push_linked_list(linked_list** manager,TYPELINKEDLIST newData);

linked_list* insert_after_linked_list(linked_list* ptrMidl,TYPELINKEDLIST newData);

void Delete_After_Linear_list(linked_list* ptrMidl);

TYPELINKEDLIST pop_linked_list(linked_list** manager);

void print_linked_list(linked_list* manager);

#endif //LINKED_LIST_LINKED_LIST_H
