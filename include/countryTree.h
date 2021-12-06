#ifndef _COUNTRY_TREE_H_
#define _COUNTRY_TREE_H_

#include "linkedList.h"
#include "date.h"

//Forward declaration of virus
typedef struct virus *virusPtr;

//Defining struct country treenode and pointer country treenode
typedef struct ctreeNode cTreeNode;
typedef struct ctreeNode *cTreeNodePtr;

typedef struct ctreeNode
{
    char *countryName;
    int pupolation; //General population
    int pop0_20;    //population for age group from 0 to 20
    int pop20_40;   //population for agr group from 20 to 40
    int pop40_60;   //population for agr group from 40 to 60
    int pop60;      //population for agr group from 60+
    cTreeNodePtr rightChild;
    cTreeNodePtr leftChild;
} cTreeNode;

//Defining struct country tree and pointer country tree
typedef struct cTree CT;
typedef struct cTree *CTPtr;

typedef struct cTree
{
    cTreeNodePtr Root;
} CT;

void countryTree_init(CTPtr tree);
cTreeNodePtr countryTree_insertCountry(CTPtr Tree, char *countryName, int age);
cTreeNodePtr countryTree_findCountry(CTPtr Tree, char *countryName, int age);
void countryTree_popStatus(CTPtr Tree, char *countryName, virusPtr virus, DatePtr date1, DatePtr date2);
void countryTree_popStatusAge(CTPtr Tree, char *countryName, virusPtr virus, DatePtr date1, DatePtr date2);
void countryTree_popStat(cTreeNodePtr treeNode, virusPtr virus, DatePtr date1, DatePtr date2);
void countryTree_popStatAge(cTreeNodePtr currCountry, virusPtr virus, DatePtr date1, DatePtr date2);
void countryTree_destructor(CTPtr Tree);
void countryTree_nodeDesructor(cTreeNodePtr treeNode);
#endif