#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bloomfilter.h"

//The following three functions have been taken and inserted into this file from the
//instructors of the course from piazza.com and calculates the hash value of a given string

// This algorithm (k=33) was first reported by dan bernstein many years
// ago in comp.lang.c. The magic of number 33 (why it works better than
// many other constants, prime or not) has never been adequately explained.

unsigned long djb2(unsigned char *str)
{
    int c;
    unsigned long hash = 5381;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

// This algorithm was created for sdbm (a public-domain reimplementation of ndbm)
// database library. it was found to do well in scrambling bits, causing better
// distribution of the keys and fewer splits. it also happens to be a good
// general hashing function with good distribution. The actual function
// is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below
// is the faster version used in gawk. There is even a faster, duff-device
// version. The magic constant 65599 was picked out of thin air while experimenting
// with different constants, and turns out to be a prime. this is one of the
// algorithms used in berkeley db (see sleepycat) and elsewhere.

unsigned long sdbm(unsigned char *str)
{
    int c;
    unsigned long hash = 0;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

// Return the result of the ith hash function. This function uses djb2 and sdbm.
// None of the functions used here is strong for cryptography purposes but they
// are good enough for the purpose of the class.
//
// The approach in this function is based on the paper:
// https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf

unsigned long hash_i(unsigned char *str, unsigned int i)
{
    return djb2(str) + i * sdbm(str) + i * i;
}

//This function initializes the values of the bloom filter from the values given
void bloomFilter_init(bloomFilterPtr bloomFltr, int numOfHashes, int bloomsize)
{
    bloomFltr->bloomSize = bloomsize;
    bloomFltr->numOfHashes = numOfHashes;
    //the number of chars used is dependent upon the number of the bloomsize which is counted in bits
    //and since the type char has 8 bits I use bloomsize div 8 to allocate the necessary number of chars
    //Then I use the bloomsize mod 8 to see if there is resedual to the division so I can allocated one more
    //char if needed.
    //I use the variable type char because its the smallest in size variable in C so that if there is residual
    //in the calculation (bloomsize mod 8) there will be the minimum possible amount of unused memory (max 7 bits of unsused memory)
    bloomFltr->bloomBitArray = malloc(sizeof(char) * ((bloomsize / 8) + (bloomsize % 8 > 0 ? 1 : 0)));
}

//This function sets the n bit of the bloomBitArray to 1, if the bit is 0 it becomes 1 and if it is 1 it remains 1
void bloom_setNBit(bloomFilterPtr bloomFltr, int n)
{
    bloomFltr->bloomBitArray[n / 8] |= 1 << (n % 8);
}

//This function returns the value of the n bit
//It return (int) 1 if the bit is (bit)1 and it returns (int)0 if the bit is (bit)0
int bloom_checkNBit(bloomFilterPtr bloomFltr, int n)
{
    return ((bloomFltr->bloomBitArray[n / 8] & (1 << (n % 8))) != 0);
}

//This function inserts an element into the bloomBitArray
void bloom_insertElement(bloomFilterPtr bloomFltr, char *element)
{
    int bit;
    for (int i = 0; i < bloomFltr->numOfHashes; i++)
    {
        //It uses the function given that changes depending on the value of the i
        bit = hash_i(element, i) % bloomFltr->bloomSize;
        bloom_setNBit(bloomFltr, bit);
    }
}

//This functions checks if the element exists in the bloomBitArray or not
//If it finds it it returns 1 and if it does not it returns 0
int bloom_checkElement(bloomFilterPtr bloomFltr, char *element)
{
    int bit = 1;
    int i = 0;
    while (i < bloomFltr->numOfHashes && bit)
    {
        //It uses the function given that changes depending on the value of the i
        bit = hash_i(element, i) % bloomFltr->bloomSize;
        bit = bloom_checkNBit(bloomFltr, bit);
        if (bit == 0)
        {
            return 0;
        }
        i++;
    }
    return 1;
}

//This function destroies all allocated values the the bloom filter uses and the destries the bloomfilter
void bloom_destructor(bloomFilterPtr bloomFltr)
{
    free(bloomFltr->bloomBitArray);
    free(bloomFltr);
}