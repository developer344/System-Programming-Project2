#ifndef _RECORD_H_
#define _RECORD_H_

#include "date.h"
#include "countryTree.h"

//Forward declaration
typedef struct ctreeNode *cTreeNodePtr;

//Defining struct citizen record and pointer citizen record
typedef struct citizenRecord citizenRecord;
typedef struct citizenRecord *citizenRecordPtr;

typedef struct citizenRecord
{
    int citizenId;
    char *name;
    char *surname;
    cTreeNodePtr country;
    int age;
} citizenRecord;

#endif