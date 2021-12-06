#ifndef _SKIP_LIST_H_
#define _SKIP_LIST_H_

#include "citizenRecord.h"

//Max level is defined as the log10000~14
#define MAX_SKIP_LIST_HEIGHT 14

//Forward declaration of citizenRecordPtr
typedef struct citizenRecord *citizenRecordPtr;

//Defining struct skipList node and pointer skipList node
typedef struct skipListNode slNode;
typedef struct skipListNode *slNodePtr;

typedef struct skipListNode
{
    int citizenId;
    DatePtr date;
    citizenRecordPtr citizenRecord;
    slNodePtr nextNodes[MAX_SKIP_LIST_HEIGHT];
} slNode;

//Defining struct skipList and pointer skipList
typedef struct skipList SkipList;
typedef struct skipList *SkipListPtr;

typedef struct skipList
{
    int height;
    slNodePtr head;
} SkipList;

int skipList_init(SkipListPtr SList);
int skipList_insert(SkipListPtr SList, citizenRecordPtr citizenRecord, char *date);
int nlvl_insert(SkipListPtr SList, int level, slNodePtr node);
slNodePtr skipList_findRecord(SkipListPtr SList, int citizenID);
void skipList_deleteCitizen(SkipListPtr SList, int citizenID);
void skipList_check_level(SkipListPtr SList);
void skipList_deleteList(SkipListPtr SList);
void skipList_print(SkipListPtr SList);
void skipList_printl(SkipListPtr SList);
int skipList_popStat(SkipListPtr SList, char *countryName, DatePtr date1, DatePtr date2);
int skipList_popStatAge(SkipListPtr SList, char *country, DatePtr date1, DatePtr date2, int minAge, int maxAge);
void skipList_destructNode(slNodePtr slnode);

#endif