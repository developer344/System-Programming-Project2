#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "Virus.h"

//Virus initialization
void virus_init(virusPtr virus, char *virusName, int numOfHashes, int bloomsize)
{
    virus->name = malloc(sizeof(char) * strlen(virusName));
    strcpy(virus->name, virusName);
    virus->vaccinatedVirusSL = malloc(sizeof(SkipList));
    skipList_init(virus->vaccinatedVirusSL);
    virus->notVaccinatedVirusSL = malloc(sizeof(SkipList));
    skipList_init(virus->notVaccinatedVirusSL);
    virus->bloomfilter = malloc(sizeof(bloomFilter));
    bloomFilter_init(virus->bloomfilter, numOfHashes, bloomsize);
}

//Insertes a citizen in the correct skiplist according to the option yes or no
void virus_insert(virusPtr virus, citizenRecordPtr citizenRecord, char *citizenID, char *vaccinated, char *date)
{
    if (!strcmp("YES", vaccinated))
    {
        //If the option is yes then I insert the ciizen to the vaccinated skiplist
        //and also the bloomfilter
        skipList_insert(virus->vaccinatedVirusSL, citizenRecord, date);
        bloom_insertElement(virus->bloomfilter, citizenID);
    }
    else if (!strcmp("NO", vaccinated))
        //If the option is yes then I insert the ciizen to the non-vaccinated skiplist
        skipList_insert(virus->notVaccinatedVirusSL, citizenRecord, NULL);
    else
    {
        perror("Error!");
    }
}

//Checks if a citizen is in the given virus either in the vaccinated skiplist or the non-vaccinated skiplist
int find_inVirus(virusPtr virus, int citizenID)
{
    if (skipList_findRecord(virus->vaccinatedVirusSL, citizenID) != NULL)
        return -1;
    if (skipList_findRecord(virus->notVaccinatedVirusSL, citizenID) != NULL)
        return -1;
    return 1;
}

//Checks if the citizen is vaccinated through the bloomfilter
int virus_checkIfVaccinatedBloom(virusPtr virus, char *citizenID)
{
    return bloom_checkElement(virus->bloomfilter, citizenID);
}

//This functin destroies the virus given
void virus_destructor(virusPtr virus)
{
    bloom_destructor(virus->bloomfilter);
    free(virus->name);
    skipList_deleteList(virus->notVaccinatedVirusSL);
    skipList_deleteList(virus->vaccinatedVirusSL);
    free(virus);
}