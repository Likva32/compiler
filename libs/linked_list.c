#include <stdio.h>
#include <malloc.h>
#include "linked_list.h"

int is_empty_linked_list(linked_list* manager){
    return manager == NULL;
}

//התחלה
//הקצאת מקום בזיכרון  לצומת החדש שיצרנו
//מגדירים שהמצביע של הצומת החדש שווה לכתובת של  manager
//משנים את manager שיצביע לכתובת של הצומת החדש
linked_list* push_linked_list(linked_list** manager,TYPELINKEDLIST newData) {
    linked_list *newNode = malloc(sizeof(linked_list));
    newNode->info = newData;
    newNode->next = (struct linked_list *) (*manager);
    *manager = newNode;
    return newNode;
}

//התחלה
//הקצאת מקום בזיכרון  לצומת החדש שיצרנו
//העברנו את ה newData שקלטנו לצומת החדש
//דאגנו שהצומת החדש יצביע לצומת שאליה מצביע ptrMidl
//דאגנו שה ptrMidl יצביע לצומת החדש
linked_list* insert_after_linked_list(linked_list* ptrMidl,TYPELINKEDLIST newData) {
    linked_list *newNode = malloc(sizeof(linked_list));
    newNode->info = newData;
    newNode->next = ptrMidl->next;
    ptrMidl->next = (struct linked_list *) newNode;
    return newNode;
}

//התחלה
//עשינו כך שה ptrMidl יצביע על הצומת שמצביע הצומת הבא
//משחררים את המקום שהוקצה בזיכרון עבור הצומת שאליה הצביע ptrMidl בהתחלה
void Delete_After_Linear_list(linked_list* ptrMidl){
    linked_list *node = (linked_list *) ptrMidl->next;
    ptrMidl-> next = node->next;
    free(node);
}

//התחלה
//נגרום ש manager יצביע על הצומת הבאה
//משחררים את המקום שהוקצה בזיכרון לצומת הראשון
TYPELINKEDLIST pop_linked_list(linked_list** manager) {
    linked_list *node = *manager;
    TYPELINKEDLIST data = node->info;
    *manager = (linked_list *) (*manager)->next;
    free(node);
    return data;
}

void print_linked_list(linked_list* manager){
    while(!is_empty_linked_list(manager)){
        printf("%p -> ", manager->info);
        manager = (linked_list *) manager->next;
    }
    printf("NULL\n");
}