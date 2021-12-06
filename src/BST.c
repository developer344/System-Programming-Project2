#include <stdio.h>
#include <stdlib.h>
#include "BST.h"

//This function initializes the root of the BST to null
void BST_init(BSTPtr Tree)
{
    Tree->Root = NULL;
}

//This function recieves  citizen record and insertes them in the BST
//When it returns 0 everything worked as intended and the citizen was insertes correctly
//if it returns 1 it means the citizen already exists in the tree
//and if it returns -1 than means something went wrong
int insertcitizenRecord(BSTPtr Tree, citizenRecordPtr citizenRecord)
{
    TreeNodePtr currNode = Tree->Root;
    //When the tree is empty
    if (currNode == NULL)
    {
        //Creation of the root node
        Tree->Root = malloc(sizeof(TreeNode));
        Tree->Root->leftChild = NULL;
        Tree->Root->rightChild = NULL;
        Tree->Root->citizenRecord = citizenRecord;
        return 0;
    }
    //When the tree in not empty
    while (currNode != NULL)
    {
        if (currNode->citizenRecord->citizenId == citizenRecord->citizenId)
        {
            //When citizen already exists in the tree
            return 1;
        }
        else if (citizenRecord->citizenId > currNode->citizenRecord->citizenId)
        {
            if (currNode->rightChild != NULL)
            {
                currNode = currNode->rightChild;
            }
            else
            {
                currNode->rightChild = malloc(sizeof(TreeNode));
                currNode->rightChild->leftChild = NULL;
                currNode->rightChild->rightChild = NULL;
                currNode->rightChild->citizenRecord = citizenRecord;
                return 0;
            }
        }
        else if (citizenRecord->citizenId < currNode->citizenRecord->citizenId)
        {
            if (currNode->leftChild != NULL)
            {
                currNode = currNode->leftChild;
            }
            else
            {
                currNode->leftChild = malloc(sizeof(TreeNode));
                currNode->leftChild->leftChild = NULL;
                currNode->leftChild->rightChild = NULL;
                currNode->leftChild->citizenRecord = citizenRecord;
                return 0;
            }
        }
    }
    return -1;
}

//This function searches for the citizen with the given citizen id by the user in the BST
citizenRecordPtr findcitizenRecord(BSTPtr Tree, int citizenId)
{
    TreeNodePtr currNode = Tree->Root;
    while (currNode != NULL)
    {
        if (currNode->citizenRecord->citizenId == citizenId)
        {
            //Found citizen in tree with the given citizen id
            return currNode->citizenRecord;
        }
        else if (citizenId > currNode->citizenRecord->citizenId)
            currNode = currNode->rightChild;
        else if (citizenId < currNode->citizenRecord->citizenId)
            currNode = currNode->leftChild;
    }
    //Couldn't find citizen in tree with the given id
    return NULL;
}

//This funtion destroys the BST
void BST_destructor(BSTPtr Tree)
{
    BSTnode_destructror(Tree->Root);
    free(Tree);
}

//This function destroys each node of the BST, it uses recursion to travers the entire binary tree.
//When the child nodes of a given node have been destroyed it destroys their parent node and continues up tree.
void BSTnode_destructror(TreeNodePtr treeNode)
{
    if (treeNode->leftChild != NULL)
        BSTnode_destructror(treeNode->leftChild);
    if (treeNode->rightChild != NULL)
        BSTnode_destructror(treeNode->rightChild);
    free(treeNode->citizenRecord->name);
    free(treeNode->citizenRecord->surname);
    free(treeNode->citizenRecord);
    free(treeNode);
}