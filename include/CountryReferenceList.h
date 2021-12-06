#ifndef _COUNTRY_REFERENCE_LIST_
#define _COUNTRY_REFERENCE_LIST_

typedef struct RefListNode CountryRefListNode;
typedef struct RefListNode *CountryRefListNodePtr;

struct RefListNode
{
    char *countryName;
    int monitorIndex;
    CountryRefListNodePtr nextNode;
};

typedef struct RefList CountryRefList;
typedef struct RefList *CountryRefListPtr;

struct RefList
{
    CountryRefListNodePtr FirstNode;
    CountryRefListNodePtr End;
    int numOfElements;
};

void CountryRefListInit(CountryRefListPtr List);
int CRLinsertRecord(CountryRefListPtr List, char *countryName, int monitorIndex);
char *getCountryName(CountryRefListNodePtr ListNode);
int getCMonitorIndex(CountryRefListNodePtr ListNode);
void CountryRefList_Delete(CountryRefListPtr List);
void CountryRefListNode_Delete(CountryRefListNodePtr ListNode);
#endif