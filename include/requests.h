#ifndef _REQUESTS_H_
#define _REQUESTS_H_

#include "date.h"

typedef struct requestnode requestNode;
typedef struct requestnode *requestNodePtr;

typedef struct requestnode
{
    int typeOfRequest; //0 reject request, 1 accept request
    DatePtr date;
    char *virusName;
    char *countryName;
    requestNodePtr nextNode;
} requestNode;

typedef struct requestsList requestList;
typedef struct requestsList *requestListPtr;

typedef struct requestsList
{
    requestNodePtr Begining, End;
    int numOfElements;
} requestList;

void Rlist_init(requestListPtr list);
void Rlist_deleteList(requestListPtr linkedListOfViruses);
int Rlist_isEmpty(requestListPtr linkedListOfViruses);
void Rlist_deleteListNode(requestNodePtr linkedListNode);
int Rlist_insertItem(requestListPtr linkedListOfRequests, int typeOfRequest, DatePtr date, char *countryname, char *virusname);

#endif