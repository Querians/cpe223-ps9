#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
// #define DRAM_SIZE 10000
#define DRAM_SIZE 1048576

typedef struct cb_struct {
    unsigned char data[16]; // One cache block is 16 bytes.
    uint32_t tag;
    uint32_t timeStamp;   /// This is used to determine what to evict. You can update the timestamp using cycles.
    int dirtyBit;
} cacheBlock;


typedef struct access {
    int readWrite; // 0 for read, 1 for write
    uint32_t address;
    uint32_t data; // If this is a read access, value here is 0
} cacheAccess;

// This is our dummy DRAM. You can initialize this in anyway you want to test.
unsigned char * DRAM; 

cacheBlock L1_cache[2][2]; // Our 2-way, 64 byte cache
cacheBlock L2_cache[4][4]; // Our 4-way, 256 byte cache

// Trace points to a series of cache accesses.
FILE *trace;

long cycles;

void init_DRAM();


// This function print the content of the cache in the following format for an N-way cache with M Sets
// Set 0   : CB1 | CB2 | CB 3 | ... | CB N
// Set 1   : CB1 | CB2 | CB 3 | ... | CB N
// ...
// Set M-1 : CB1 | CB2 | CB 3 | ... | CB N
void printCache();

////// Part 1: Checking if the address is in the cache /////

// These functions perform a cache lookup to the associated cache levels. Return 0 if the address is not in the cache (cache miss) and 1 if the address is in the cache (cache hit)
int L1lookup(uint32_t address);
int L2lookup(uint32_t address);

// This function returns a setID given an address
unsigned int getL1SetID(uint32_t address);
unsigned int getL2SetID(uint32_t address);

// This function returns a tag given an address
unsigned int getL1Tag(uint32_t address);
unsigned int getL2Tag(uint32_t address);

////// Part 2: FIFO cache //////
// This function performs a read to a cache content. Return the 4-byte data content given by the address. Please note that the read access MUST evict existing cache block correctly based on the FIFO policy and insert the new cache block correctly into the cache.
uint32_t read_fifo(uint32_t address);

// EPart 3: handle write. Assuming a write-through cache. This is the only place you actually modify the data in the cache and DRAM
void write(uint32_t address, uint32_t data);

