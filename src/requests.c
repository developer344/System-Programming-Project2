#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "requests.h"

//This function initializes the linked list that containe the viruses
void Rlist_init(requestListPtr list)
{
    list->Begining = NULL;
    list->End = NULL;
    list->numOfElements = 0;
}

//This function is used to insert a virus in the linked list
int Rlist_insertItem(requestListPtr linkedListOfRequests, int typeOfRequest, DatePtr date, char *countryname, char *virusname)
{
    if (Rlist_isEmpty(linkedListOfRequests))
    {
        linkedListOfRequests->Begining = malloc(sizeof(requestList));
        linkedListOfRequests->End = linkedListOfRequests->Begining;
        linkedListOfRequests->End->typeOfRequest = typeOfRequest;
        linkedListOfRequests->End->date = date;
        linkedListOfRequests->End->countryName = countryname;
        linkedListOfRequests->End->virusName = virusname;
        linkedListOfRequests->End->nextNode = NULL;
        linkedListOfRequests->numOfElements++;
        return 0;
    }
    else
    {
        linkedListOfRequests->End->nextNode = malloc(sizeof(requestList));
        linkedListOfRequests->End = linkedListOfRequests->End->nextNode;
        linkedListOfRequests->End->typeOfRequest = typeOfRequest;
        linkedListOfRequests->End->date = date;
        linkedListOfRequests->End->nextNode = NULL;
        linkedListOfRequests->numOfElements++;
        return 0;
    }
    return -1;
}

//This function is used to check whether the linked list is empty or not
int Rlist_isEmpty(requestListPtr linkedListOfRequests)
{
    return linkedListOfRequests->numOfElements == 0;
}

//This function deletes the entire linked list
void Rlist_deleteList(requestListPtr linkedListOfRequests)
{
    requestNodePtr nextNode, currNode = linkedListOfRequests->Begining;
    while (currNode != NULL)
    {
        nextNode = currNode->nextNode;
        Rlist_deleteListNode(currNode);
        currNode = nextNode;
    }
    free(linkedListOfRequests);
}

//This function deletes the given linked list node
void Rlist_deleteListNode(requestNodePtr linkedListNode)
{
    dateDestructor(linkedListNode->date);
    free(linkedListNode);
}