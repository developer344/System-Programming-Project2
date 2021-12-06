#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "Virus.h"

//Forward declaration of virusPtr
typedef struct virus *virusPtr;

//Defining struct list node and pointer list node
typedef struct listnode listNode;
typedef struct listnode *listNodePtr;

typedef struct listnode
{
    virusPtr virus;
    listNodePtr nextNode;
} listNode;

//Defining struct linked list and pointer linked list
typedef struct linkedlist linkedList;
typedef struct linkedlist *linkedListPtr;

typedef struct linkedlist
{
    listNodePtr Begining;
    listNodePtr End;
    int numOfElements;
} linkedList;

void list_init(linkedListPtr list);
int list_insertItem(linkedListPtr linkedListOfViruses, virusPtr virus);
virusPtr list_searchElement(linkedListPtr linkedListOfViruses, char *virusName);
int list_getNumOfElements(linkedListPtr linkedListOfViruses);
int list_isEmpty(linkedListPtr linkedListOfViruses);
void list_deleteList(linkedListPtr linkedListOfViruses);
void list_deleteListNode(listNodePtr linkedListNode);

#endif