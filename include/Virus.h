#ifndef _VIRUS_H_
#define _VIRUS_H_

#include "bloomfilter.h"
#include "skipList.h"
#include "citizenRecord.h"

//Forward declaration of SkipListPtr
typedef struct skipList *SkipListPtr;

//Forward declaration of citizenRecordPtr
typedef struct citizenRecord *citizenRecordPtr;

//Defining struct virus and pointer virus
typedef struct virus virus;
typedef struct virus *virusPtr;

typedef struct virus
{
    char *name;
    bloomFilterPtr bloomfilter;
    SkipListPtr vaccinatedVirusSL, notVaccinatedVirusSL;
} virus;

void virus_init(virusPtr virus, char *virusName, int numOfHashes, int bloomsize);
void virus_insert(virusPtr virus, citizenRecordPtr citizenRecord, char *citizenID, char *vaccinated, char *date);
int find_inVirus(virusPtr virus, int citizenID);
int virus_checkIfVaccinatedBloom(virusPtr virus, char *citizenID);
void virus_destructor(virusPtr virus);
//rounders
#endif
