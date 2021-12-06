#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

//This function initializes the linked list that containe the viruses
void list_init(linkedListPtr list)
{
    list->Begining = NULL;
    list->End = NULL;
    list->numOfElements = 0;
}

//This function is used to insert a virus in the linked list
int list_insertItem(linkedListPtr linkedListOfViruses, virusPtr virus)
{
    if (list_isEmpty(linkedListOfViruses))
    {
        linkedListOfViruses->Begining = malloc(sizeof(listNode));
        linkedListOfViruses->End = linkedListOfViruses->Begining;
        linkedListOfViruses->End->virus = virus;
        linkedListOfViruses->End->nextNode = NULL;
        linkedListOfViruses->numOfElements++;
        return 0;
    }
    else
    {
        linkedListOfViruses->End->nextNode = malloc(sizeof(listNode));
        linkedListOfViruses->End = linkedListOfViruses->End->nextNode;
        linkedListOfViruses->End->virus = virus;
        linkedListOfViruses->End->nextNode = NULL;
        linkedListOfViruses->numOfElements++;
        return 0;
    }
    return -1;
}

//This function is used to search for a virus with the given virus name in the tree
//If it finds it it returns it otherwise it returns null
virusPtr list_searchElement(linkedListPtr linkedListOfViruses, char *virusName)
{
    if (!list_isEmpty(linkedListOfViruses))
    {
        listNodePtr currNode = linkedListOfViruses->Begining;
        while (currNode != NULL)
        {
            if (!strcmp(currNode->virus->name, virusName))
                return currNode->virus;
            currNode = currNode->nextNode;
        }
    }
    return NULL;
}

//This function is used to get the number of elements in the list
int list_getNumOfElements(linkedListPtr linkedListOfViruses)
{
    return linkedListOfViruses->numOfElements;
}

//This function is used to check whether the linked list is empty or not
int list_isEmpty(linkedListPtr linkedListOfViruses)
{
    return list_getNumOfElements(linkedListOfViruses) == 0;
}

//This function deletes the entire linked list
void list_deleteList(linkedListPtr linkedListOfViruses)
{
    listNodePtr nextNode, currNode = linkedListOfViruses->Begining;
    while (currNode != NULL)
    {
        nextNode = currNode->nextNode;
        list_deleteListNode(currNode);
        currNode = nextNode;
    }
    free(linkedListOfViruses);
}

//This function deletes the given linked list node
void list_deleteListNode(listNodePtr linkedListNode)
{
    virus_destructor(linkedListNode->virus);
    free(linkedListNode);
}