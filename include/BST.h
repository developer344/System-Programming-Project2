#ifndef _BST_H_
#define _BST_H_

#include "citizenRecord.h"

//Defining struct treeNode and pointer treeNode
typedef struct treeNode TreeNode;
typedef struct treeNode *TreeNodePtr;

typedef struct treeNode
{
    citizenRecordPtr citizenRecord; //Citizen
    TreeNodePtr rightChild;
    TreeNodePtr leftChild;
} TreeNode;

//Defining struct tree and pointer tree
typedef struct Tree BST;
typedef struct Tree *BSTPtr;

typedef struct Tree
{
    TreeNodePtr Root;
} BST;

void BST_init(BSTPtr Tree);
int insertcitizenRecord(BSTPtr Tree, citizenRecordPtr citizenRecord);
citizenRecordPtr findcitizenRecord(BSTPtr Tree, int citizenId);
void BST_destructor(BSTPtr Tree);
void BSTnode_destructror(TreeNodePtr treeNode);
#endif