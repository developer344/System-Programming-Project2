#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CountryReferenceList.h"

//This function initializes the root of the BST to null
void CountryRefListInit(CountryRefListPtr List)
{
    List->FirstNode = NULL;
    List->End = NULL;
    List->numOfElements = 0;
}

int CRLinsertRecord(CountryRefListPtr List, char *countryName, int monitorIndex)
{
    if (List->numOfElements == 0)
    {
        List->FirstNode = malloc(sizeof(CountryRefListNode));
        List->End = List->FirstNode;
        List->End->nextNode = NULL;
        List->FirstNode->countryName = malloc(sizeof(char) * strlen(countryName));
        strcpy(List->FirstNode->countryName, countryName);
        List->FirstNode->monitorIndex = monitorIndex;
        List->numOfElements++;
        return 0;
    }
    else
    {

        List->End->nextNode = malloc(sizeof(CountryRefListNode));
        List->End = List->End->nextNode;
        List->End->countryName = malloc(sizeof(char) * strlen(countryName));
        strcpy(List->End->countryName, countryName);
        List->End->monitorIndex = monitorIndex;
        List->numOfElements++;
        List->End->nextNode = NULL;
        return 0;
    }
    return -1;
}

char *getCountryName(CountryRefListNodePtr ListNode)
{
    return ListNode->countryName;
}

int getCMonitorIndex(CountryRefListNodePtr ListNode)
{
    return ListNode->monitorIndex;
}

//This function deletes the entire linked list
void CountryRefList_Delete(CountryRefListPtr List)
{
    CountryRefListNodePtr nextNode, currNode = List->FirstNode;
    while (currNode != NULL)
    {
        nextNode = currNode->nextNode;
        CountryRefListNode_Delete(currNode);
        currNode = nextNode;
    }
    free(List);
}

//This function deletes the given linked list node
void CountryRefListNode_Delete(CountryRefListNodePtr ListNode)
{
    free(ListNode->countryName);
    free(ListNode);
}