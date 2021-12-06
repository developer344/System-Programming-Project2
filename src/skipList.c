#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skipList.h"
#include "citizenRecord.h"

//Initialazation of the skip list I use a node head and a node tail
int skipList_init(SkipListPtr SList)
{
    SList->height = 0;
    SList->head = malloc(sizeof(slNode));
    SList->head->date = NULL;
    SList->head->citizenRecord = NULL;
    SList->head->citizenId = -1;
    slNodePtr lastNode = malloc(sizeof(slNode));
    lastNode->date = NULL;
    lastNode->citizenRecord = NULL;
    lastNode->citizenId = 10000;
    for (int i = 0; i < MAX_SKIP_LIST_HEIGHT; i++)
    {
        SList->head->nextNodes[i] = lastNode;
        lastNode->nextNodes[i] = NULL;
    }
}

//Inserts a citizen to the skiplist
int skipList_insert(SkipListPtr SList, citizenRecordPtr citizenRecord, char *date)
{
    if (skipList_findRecord(SList, citizenRecord->citizenId) > 0)
    {
        perror("Error");
        return -1;
    }
    int currHeight = 0;
    while (currHeight <= MAX_SKIP_LIST_HEIGHT)
    {

        if (rand() % 2 == 0)
        {
            break;
        }
        currHeight++;
    }

    slNodePtr node = malloc(sizeof(slNode));
    if (date != NULL)
    {
        node->date = date_init(date);
    }
    else
        node->date = NULL;
    node->citizenRecord = citizenRecord;
    node->citizenId = citizenRecord->citizenId;
    if (currHeight > SList->height)
        SList->height = currHeight;
    for (int i = 0; i <= currHeight; i++)
    {
        nlvl_insert(SList, i, node);
    }
    return 0;
}

//This function connect the node to the skip list on the n level
int nlvl_insert(SkipListPtr SList, int level, slNodePtr node)
{
    slNodePtr currNode = SList->head->nextNodes[level];
    slNodePtr prevNode = SList->head;
    while (!((prevNode->citizenId < node->citizenRecord->citizenId && currNode->citizenId > node->citizenRecord->citizenId) || currNode == NULL))
    {
        currNode = currNode->nextNodes[level];
        prevNode = prevNode->nextNodes[level];
    }

    if (currNode != NULL && prevNode != NULL)
    {
        node->nextNodes[level] = currNode;
        prevNode->nextNodes[level] = node;
        return 0;
    }
    return -1;
}

//This function searches for a citizen in the skiplist
//It returns the node if it exists and if it doesn;t exist it returns null
slNodePtr skipList_findRecord(SkipListPtr SList, int citizenID)
{
    slNodePtr prevNode = SList->head;
    slNodePtr currNode = SList->head->nextNodes[SList->height];
    for (int level = SList->height; level >= 0; level--)
    {
        while (currNode->citizenId != 10000)
        {
            if (citizenID == currNode->citizenId)
            {
                return currNode;
            }
            else if (citizenID > currNode->citizenId && citizenID < currNode->nextNodes[level]->citizenId)
            {
                break;
            }
            else if (citizenID > prevNode->citizenId && citizenID < currNode->citizenId)
            {
                //If we are at the bottom level and its not between the prev node and the current node
                //we can't go down a level so it doesn't exist
                if (level - 1 < 0)
                    return NULL;
                currNode = prevNode->nextNodes[level - 1];
                break;
            }
            prevNode = prevNode->nextNodes[level];
            currNode = currNode->nextNodes[level];
        }
    }
    return NULL;
}

//This function deletes a record from the skip list that
//correspondes to the citizen with the citizen Id given
void skipList_deleteCitizen(SkipListPtr SList, int citizenID)
{
    slNodePtr currNode, prevNode, desNode;
    for (int level = 0; level <= SList->height; level++)
    {
        prevNode = SList->head;
        currNode = SList->head->nextNodes[level];
        while (currNode != NULL)
        {
            if (currNode->citizenId == citizenID)
            {
                prevNode->nextNodes[level] = currNode->nextNodes[level];
                desNode = currNode;
                break;
            }
            prevNode = prevNode->nextNodes[level];
            currNode = currNode->nextNodes[level];
        }
    }
    skipList_destructNode(desNode);
    skipList_check_level(SList);
}

//Checks the height of the skiplist after a deletion of a node of the skip list
void skipList_check_level(SkipListPtr SList)
{
    for (int level = SList->height; level >= 0; level--)
    {
        if (SList->head->nextNodes[level]->citizenId == 10000)
            SList->height--;
        else
            break;
    }
}

//Deletes the whole skip list
//we take every node from the level 0 that contains all the elements of the skip list
//and deletes the node one by one and then deletes the skiip list structure
void skipList_deleteList(SkipListPtr SList)
{
    slNodePtr nextNode, currNode = SList->head;
    while (currNode != NULL)
    {
        nextNode = currNode->nextNodes[0];
        skipList_destructNode(currNode);
        currNode = nextNode;
    }
    free(SList);
}

//This function prints all the elements of the skip list using the level 0 that
//has all the elements of the skip list
void skipList_print(SkipListPtr SList)
{
    slNodePtr currNode = SList->head;
    while (currNode != NULL)
    {
        if (currNode->citizenId != -1 && currNode->citizenId != 10000)
            printf("%d %s %s %s %d\n", currNode->citizenRecord->citizenId, currNode->citizenRecord->name, currNode->citizenRecord->surname, currNode->citizenRecord->country->countryName, currNode->citizenRecord->age);
        currNode = currNode->nextNodes[0];
    }
    printf("\n");
}

//This function prints every level of the skip list its main use is for debugging
void skipList_printl(SkipListPtr SList)
{
    slNodePtr currNode;
    for (int level = SList->height; level >= 0; level--)
    {
        printf("Level:%d  :: ", level);
        currNode = SList->head;
        while (currNode != NULL)
        {
            printf("ID:%d,", currNode->citizenId);
            currNode = currNode->nextNodes[level];
        }
        printf("\n");
    }
}

//This function counts and returns the number of people that are in the given skip list
//and that are from the given country that have been vaccinated from in the given date range
//(between date1 and date2)
//This function uses only the vaccinated skip list of a virus and it can not be applied to the noVaccinated skip list
int skipList_popStat(SkipListPtr SList, char *country, DatePtr date1, DatePtr date2)
{
    slNodePtr currNode = SList->head->nextNodes[0];
    int numOfVaccinated = 0;
    //The tail node has the citizen id 10000
    while (currNode->citizenId != 10000)
    {
        if (!strcmp(currNode->citizenRecord->country->countryName, country) && datecmp(date1, currNode->date) < 0 && datecmp(date2, currNode->date) > 0)
        {
            numOfVaccinated++;
        }
        currNode = currNode->nextNodes[0];
    }
    return numOfVaccinated;
}

//This function counts and returns the number of people in the given age range (from minAge to maxAge)
//that are in the given skip list and that are from the given country that have been vaccinated from in
//the given date range (between date1 and date2)
//This function uses only the vaccinated skip list of a virus and it can not be applied to the noVaccinated skip list
int skipList_popStatAge(SkipListPtr SList, char *country, DatePtr date1, DatePtr date2, int minAge, int maxAge)
{
    slNodePtr currNode = SList->head->nextNodes[0];
    int numOfVaccinated = 0;
    //The tail node has the citizen id 10000
    while (currNode->citizenId != 10000)
    {
        if (!strcmp(currNode->citizenRecord->country->countryName, country) && datecmp(date1, currNode->date) < 0 && datecmp(date2, currNode->date) > 0)
        {
            if (currNode->citizenRecord->age > minAge && currNode->citizenRecord->age <= maxAge)
                numOfVaccinated++;
        }
        currNode = currNode->nextNodes[0];
    }
    return numOfVaccinated;
}

//THis function destries the given skip list node
void skipList_destructNode(slNodePtr slnode)
{
    if (slnode->date != NULL)
        free(slnode->date);
    free(slnode);
}