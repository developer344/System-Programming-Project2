#ifndef _BLOOMFILTER_H_
#define _BLOOMFILTER_H_

#define NUMOFHASHFUNCTIONS 16

//Defining struct bloomfilter and pointer bloomfilter
typedef struct bloomFilter bloomFilter;
typedef struct bloomFilter *bloomFilterPtr;

struct bloomFilter
{
    //Number of hash functions
    int numOfHashes;
    //Number of bits in the bloomfilter
    int bloomSize;

    //I use the variable type char because its the smallest in size variable in C so that if there is residual
    //in the calculation (bloomsize mod 8) there will be the minimum possible amount of unused memory (max 7 bits of unsused memory)

    //Contains the bits of the bloomfilter
    char *bloomBitArray;
};

void bloomFilter_init(bloomFilterPtr bloomFltr, int numOfHashes, int bloomsize);
void bloom_setNBit(bloomFilterPtr bloomFltr, int n);
int bloom_checkNBit(bloomFilterPtr bloomFltr, int n);
void bloom_insertElement(bloomFilterPtr bloomFltr, char *element);
int bloom_checkElement(bloomFilterPtr bloomFltr, char *element);
void bloom_destructor(bloomFilterPtr bloomFltr);
#endif