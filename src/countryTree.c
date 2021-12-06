#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "countryTree.h"
#include "linkedList.h"

//This function initializes the root of the country tree to null
void countryTree_init(CTPtr tree)
{
    tree->Root = NULL;
}

//This function inserts a new country to the country tree
//int age is used to initialize the population of the age groups correctly
cTreeNodePtr countryTree_insertCountry(CTPtr Tree, char *countryName, int age)
{
    cTreeNodePtr currNode = Tree->Root;
    if (currNode == NULL)
    {
        //When the tree in empty
        Tree->Root = malloc(sizeof(cTreeNode));
        Tree->Root->pupolation = 1;
        //Initialization of age groups
        if (age > 0)
            if (age <= 20)
            {
                Tree->Root->pop0_20 = 1;
                Tree->Root->pop20_40 = 0;
                Tree->Root->pop40_60 = 0;
                Tree->Root->pop60 = 0;
            }
            else if (age <= 40)
            {
                Tree->Root->pop0_20 = 0;
                Tree->Root->pop20_40 = 1;
                Tree->Root->pop40_60 = 0;
                Tree->Root->pop60 = 0;
            }
            else if (age <= 60)
            {
                Tree->Root->pop0_20 = 0;
                Tree->Root->pop20_40 = 0;
                Tree->Root->pop40_60 = 1;
                Tree->Root->pop60 = 0;
            }
            else
            {
                Tree->Root->pop0_20 = 0;
                Tree->Root->pop20_40 = 0;
                Tree->Root->pop40_60 = 0;
                Tree->Root->pop60 = 1;
            }
        Tree->Root->leftChild = NULL;
        Tree->Root->rightChild = NULL;
        Tree->Root->countryName = malloc(sizeof(char) * strlen(countryName));
        strcpy(Tree->Root->countryName, countryName);
        return Tree->Root;
    }
    //When the tree is not empty
    while (currNode != NULL)
    {
        if (!strcmp(currNode->countryName, countryName))
        {
            //The country already exists
            return NULL;
        }
        else if (strcmp(countryName, currNode->countryName) > 0)
        {
            if (currNode->rightChild != NULL)
            {
                currNode = currNode->rightChild;
            }
            else
            {
                currNode->rightChild = malloc(sizeof(cTreeNode));
                currNode->rightChild->pupolation = 1;
                if (age > 0)
                    if (age <= 20)
                    {
                        currNode->rightChild->pop0_20 = 1;
                        currNode->rightChild->pop20_40 = 0;
                        currNode->rightChild->pop40_60 = 0;
                        currNode->rightChild->pop60 = 0;
                    }
                    else if (age <= 40)
                    {
                        currNode->rightChild->pop0_20 = 0;
                        currNode->rightChild->pop20_40 = 1;
                        currNode->rightChild->pop40_60 = 0;
                        currNode->rightChild->pop60 = 0;
                    }
                    else if (age <= 60)
                    {
                        currNode->rightChild->pop0_20 = 0;
                        currNode->rightChild->pop20_40 = 0;
                        currNode->rightChild->pop40_60 = 1;
                        currNode->rightChild->pop60 = 0;
                    }
                    else
                    {
                        currNode->rightChild->pop0_20 = 0;
                        currNode->rightChild->pop20_40 = 0;
                        currNode->rightChild->pop40_60 = 0;
                        currNode->rightChild->pop60 = 1;
                    }
                currNode->rightChild->countryName = malloc(sizeof(char) * strlen(countryName));
                strcpy(currNode->rightChild->countryName, countryName);
                currNode->rightChild->leftChild = NULL;
                currNode->rightChild->rightChild = NULL;
                return currNode->rightChild;
            }
        }
        else if (strcmp(countryName, currNode->countryName) < 0)
        {
            if (currNode->leftChild != NULL)
            {
                currNode = currNode->leftChild;
            }
            else
            {
                currNode->leftChild = malloc(sizeof(cTreeNode));
                currNode->leftChild->pupolation = 1;
                if (age > 0)
                    if (age <= 20)
                    {
                        currNode->leftChild->pop0_20 = 1;
                        currNode->leftChild->pop20_40 = 0;
                        currNode->leftChild->pop40_60 = 0;
                        currNode->leftChild->pop60 = 0;
                    }
                    else if (age <= 40)
                    {
                        currNode->leftChild->pop0_20 = 0;
                        currNode->leftChild->pop20_40 = 1;
                        currNode->leftChild->pop40_60 = 0;
                        currNode->leftChild->pop60 = 0;
                    }
                    else if (age <= 60)
                    {
                        currNode->leftChild->pop0_20 = 0;
                        currNode->leftChild->pop20_40 = 0;
                        currNode->leftChild->pop40_60 = 1;
                        currNode->leftChild->pop60 = 0;
                    }
                    else
                    {
                        currNode->leftChild->pop0_20 = 0;
                        currNode->leftChild->pop20_40 = 0;
                        currNode->leftChild->pop40_60 = 0;
                        currNode->leftChild->pop60 = 1;
                    }
                currNode->leftChild->countryName = malloc(sizeof(char) * strlen(countryName));
                strcpy(currNode->leftChild->countryName, countryName);
                currNode->leftChild->leftChild = NULL;
                currNode->leftChild->rightChild = NULL;
                return currNode->leftChild;
            }
        }
    }
    //If something goes wrong return NULL
    return NULL;
}

//This function is used to find if the given country exists in the country tree
//int age is used to as a flag so that the function knows if it shoud increase the population
//and aslo the variable is also used to see which age group population itt should increase
//if int age is a non positive intiger the function doesn't increase the population
cTreeNodePtr countryTree_findCountry(CTPtr Tree, char *countryName, int age)
{
    cTreeNodePtr currNode = Tree->Root;
    while (currNode != NULL)
    {
        if (!strcmp(currNode->countryName, countryName))
        {
            if (age > 0)
            {
                currNode->pupolation++;
                if (age <= 20)
                    currNode->pop0_20++;
                else if (age <= 40)
                    currNode->pop20_40++;
                else if (age <= 60)
                    currNode->pop40_60++;
                else
                    currNode->pop60++;
            }
            return currNode;
        }
        else if (strcmp(countryName, currNode->countryName) > 0)
            currNode = currNode->rightChild;
        else if (strcmp(countryName, currNode->countryName) < 0)
            currNode = currNode->leftChild;
    }
    return NULL;
}

//This function is used to print the statisics of how many people of a country have been vaccinated to the virus given.
//If the variable char *countryName is null then it prints the statistics of how many people have been vaccinated to the virus given
//for every country in the database.
void countryTree_popStatus(CTPtr Tree, char *countryName, virusPtr virus, DatePtr date1, DatePtr date2)
{
    if (countryName != NULL)
    {
        int numOfVaccinated = skipList_popStat(virus->vaccinatedVirusSL, countryName, date1, date2);
        cTreeNodePtr currCountry = countryTree_findCountry(Tree, countryName, 0);
        float percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pupolation);
        printf("%s %d %.2f%%\n", currCountry->countryName, numOfVaccinated, percentage);
    }
    else
        countryTree_popStat(Tree->Root, virus, date1, date2);
}

//This function uses in-order binary tree traversal so that it visits all nodes by order and print for each country-node
//of the tree ad print the statisics of how many people of a country have been vaccinated to the virus given
void countryTree_popStat(cTreeNodePtr currCountry, virusPtr virus, DatePtr date1, DatePtr date2)
{
    //I use in order tree traversal to print the countries in alphabetical order
    if (currCountry->leftChild != NULL)
        countryTree_popStat(currCountry->leftChild, virus, date1, date2);

    int numOfVaccinated = skipList_popStat(virus->vaccinatedVirusSL, currCountry->countryName, date1, date2);
    float percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pupolation);
    printf("%s %d %.2f%%\n", currCountry->countryName, numOfVaccinated, percentage);

    if (currCountry->rightChild != NULL)
        countryTree_popStat(currCountry->rightChild, virus, date1, date2);
}

//This function is used to print the statisics of how many people of a country have been vaccinated to the virus given
//categorized in four age groups(0-20, 20-40, 40-60 and 60+).
//If the variable char *countryName is null then it prints the statistics of how many people have been vaccinated to the virus given
//for every country in the database categorized in four age groups(0-20, 20-40, 40-60 and 60+).
void countryTree_popStatusAge(CTPtr Tree, char *countryName, virusPtr virus, DatePtr date1, DatePtr date2)
{
    if (countryName != NULL)
    {
        cTreeNodePtr currCountry = countryTree_findCountry(Tree, countryName, 0);

        printf("\n%s:\n", countryName);

        float percentage = 0.0;

        int numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, countryName, date1, date2, 0, 20);
        if (numOfVaccinated != 0)
            percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop0_20);
        printf("0-20 %d %.2f%%\n", numOfVaccinated, percentage);

        percentage = 0.0;
        numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, countryName, date1, date2, 20, 40);
        if (numOfVaccinated != 0)
            percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop20_40);
        printf("20-40 %d %.2f%%\n", numOfVaccinated, percentage);

        percentage = 0.0;
        numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, countryName, date1, date2, 40, 60);
        if (numOfVaccinated != 0)
            percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop40_60);
        printf("40-60 %d %.2f%%\n", numOfVaccinated, percentage);

        percentage = 0.0;
        numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, countryName, date1, date2, 60, 200);
        if (numOfVaccinated != 0)
            percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop60);
        printf("60+ %d %.2f%%\n", numOfVaccinated, percentage);
    }
    else
        countryTree_popStatAge(Tree->Root, virus, date1, date2);
}

//This function uses in-order binary tree traversal so that it visits all nodes by order and print for each country-node
//of the tree ad print the statisics of how many people of a country have been vaccinated to the virus given
//categorized in four age groups(0-20, 20-40, 40-60 and 60+).
void countryTree_popStatAge(cTreeNodePtr currCountry, virusPtr virus, DatePtr date1, DatePtr date2)
{
    //I use in order tree traversal to print the countries in alphabetical order
    if (currCountry->leftChild != NULL)
        countryTree_popStatAge(currCountry->leftChild, virus, date1, date2);

    printf("\n%s:\n", currCountry->countryName);

    float percentage = 0.0;
    int numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, currCountry->countryName, date1, date2, 0, 20);
    if (numOfVaccinated != 0)
        percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop0_20);
    printf("0-20 %d %.2f%%\n", numOfVaccinated, percentage);

    percentage = 0.0;
    numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, currCountry->countryName, date1, date2, 20, 40);
    if (numOfVaccinated != 0)
        percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop20_40);
    printf("20-40 %d %.2f%%\n", numOfVaccinated, percentage);

    percentage = 0.0;
    numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, currCountry->countryName, date1, date2, 40, 60);
    if (numOfVaccinated != 0)
        percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop40_60);
    printf("40-60 %d %.2f%%\n", numOfVaccinated, percentage);

    percentage = 0.0;
    numOfVaccinated = skipList_popStatAge(virus->vaccinatedVirusSL, currCountry->countryName, date1, date2, 60, 200);
    if (numOfVaccinated != 0)
        percentage = (float)numOfVaccinated * 100.0 / (float)(currCountry->pop60);
    printf("60+ %d %.2f%%\n", numOfVaccinated, percentage);

    if (currCountry->rightChild != NULL)
        countryTree_popStatAge(currCountry->rightChild, virus, date1, date2);
}

//This function destroys the country tree
void countryTree_destructor(CTPtr Tree)
{
    countryTree_nodeDesructor(Tree->Root);
    free(Tree);
}

//This function destroys each node of the country tree, it uses recursion to travers the entire binary tree.
//When the child nodes of a given node have been destroyed it destroys their parent node and continues up tree.
void countryTree_nodeDesructor(cTreeNodePtr treeNode)
{
    if (treeNode->leftChild != NULL)
        countryTree_nodeDesructor(treeNode->leftChild);
    if (treeNode->rightChild != NULL)
        countryTree_nodeDesructor(treeNode->rightChild);
    free(treeNode->countryName);
    free(treeNode);
}