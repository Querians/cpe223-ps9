// writeBack + LRU

/*
ถ้ามีการเขียนทับ set ค่า dirtybit ใน cacheblock (ถ้ามีใน L1 เปลี่ยนแค่ L1, ถ้ามีแค่ใน L2 เปลี่ยนแค่ L2) นั้น = 1

  - ถ้ามีการแก้ค่าแล้วค่านั้นมีใน L1 เปลี่ยนค่าแค่ใน L1 + set dirtybit
    -> ถ้า cache block ที่โดนแก้จะโดนเตะออก เตะออกและให้ไปแก้ค่า dirtybit ใน L2
  - ถ้ามีแค่ใน L2 เปลี่ยนค่าแค่ใน L2 + set dirty bit
    -> ถ้า cache block ที่โดนแก้จะโดนเตะออก เตะออกและให้ไปแก้ค่าใน DRAM
  - ถ้ามีแค่ใน DRAM แก้ค่าเลย

*/

/*
แก้ใน function
- Read   -> ถ้าจะเตะออกให้ไปแก้ค่าและแก้ dirtybit ของชั้นที่ไกลกว่าเดิม 1 ชั้น
- write  -> เขียนทับใน L1, L2 เปลี่ยนค่า dirtybit
*/

#include "cacheSim.h"
#include <stdio.h>

// In this question, we will assume DRAM 
// will take a 4-byte values starting from 0 to n
void init_DRAM()
{
	unsigned int i=0;
	DRAM = malloc(sizeof(char) * DRAM_SIZE);
	for(i=0;i<DRAM_SIZE/4;i++)
	{
		*((unsigned int*)DRAM+i) = i;
    }
}

void printCache()
{
	int i,j,k;
	printf("===== L1 Cache Content =====\n");
	for(i=0;i<2;i++)
	{
		printf("Set %d :", i);
		for(j=0;j<2;j++)
		{
			printf(" {(TAG: 0x%x)", (unsigned int)(L1_cache[i][j].tag));
			for(k=0;k<16;k++)
				printf(" 0x%x,", (unsigned int)(L1_cache[i][j].data[k]));
			printf(" |\n");
		}
		printf("\n");
	}
	printf("===== L2 Cache Content =====\n");
	for(i=0;i<4;i++)
	{
		printf("Set %d :", i);
		for(j=0;j<4;j++)
		{
			printf(" {(TAG: 0x%x)", (unsigned int)(L2_cache[i][j].tag));
			for(k=0;k<16;k++)
				printf(" 0x%x,", (unsigned int)(L2_cache[i][j].data[k]));
			printf(" |\n");
		}
		printf("\n");
	}
}

// return the data you are reading
uint32_t read_LRU(uint32_t address)
{
  // initialize variable for cache hit
  uint32_t bib, tag, index;

  // initialize variable to access DRAM
  uint32_t bib1, tag1, index1;
  uint32_t bib2, tag2, index2;
  int block;

  //------------------------------L1 Cache address check-------------------------
  
  // check if there is L1 hit-address in L1 cache
  // return current data accessed by address
  if (L1lookup(address) == 1){
    //get tag(11), index(1), and byte in block(4) from an address 
    index = getL1SetID(address);
    tag = getL1Tag(address);
    bib = ((address - (tag << 5)) - (index << 4));
    
    // for L1 cache, there are 2 set of duo 16-bits cache block
    for(int i=0; i<2; i++){
      // check if there is target tag in current set(i)
      if(L1_cache[index][i].tag == tag){
        // if found, then assign ith cache block of set to block 
        L1_cache[index][i].timeStamp = cycles;
        // printf("time %d\n",L1_cache[index][i].timeStamp);
        block = i;
        break;
      }
    }

    uint32_t firstbib = bib - (bib%4);    // the beginning of block
    uint32_t retval;                      // return value

    // join 1 byte (unsigned char) 4 blocks -> 4 bytes (uint32_t)
    for(int i=3; i>0; i--){
      retval = L1_cache[index][block].data[firstbib + i] << 8 | L1_cache[index][block].data[firstbib + i - 1];
    }
    return retval;
    // return L1_cache[index][block].data[bib];
  }

  //------------------------------L2 Cache address check-------------------------
    
  // if that address is not relevant to L1 cache,
  // check if L2's hit is possible
  // return current data accessed by address
  else if (L2lookup(address) == 1){
    //get tag(10), index(2), and byte in block(4) from an address 
    tag = getL2Tag(address);
    index = getL2SetID(address);
    bib = ((address - (tag << 6)) - (index << 4));

    // for L2 cache, there are 4 set of tetra 16-bits cache block
    for(int i=0; i<4;i++){
      // check if there is target tag in current set(i)
      if(L2_cache[index][i].tag == tag){
        // if found, then assign ith cache block of set to block 
        L2_cache[index][i].timeStamp = cycles;
        block = i;
        break;
     }
    }

    // ก้อนนี้ถึงคำว่า เอ่อ ถามจาร
    // write in L1 cache      //get tag(11), index(1), and byte in block(4) from an address
    index1 = getL1SetID(address);
    tag1 = getL1Tag(address);
    bib1 = ((address - (tag1 << 5)) - (index1 << 4));
    // printf("time %d %d\n",L1_cache[index][0].timeStamp,L1_cache[index][1].timeStamp);
    uint32_t current_block;
    if(L1_cache[index1][0].timeStamp < L1_cache[index1][1].timeStamp){
      current_block = 0; // first cache block
    }
    else{
      current_block = 1; // second cache block
    }
    printf("current block %d",current_block);
    // replace with new value into cache block and set new timestamp
    for(int j = 0 ; j < 16 ; j ++){
        L1_cache[index1][current_block].data[j] = L2_cache[index][block].data[j];
    }

    L1_cache[index1][current_block].timeStamp = cycles;
    // เอ่อ

    uint32_t firstbib = bib - (bib%4);    // the beginning of block
    uint32_t retval;                      // return value

    // join 1 byte (unsigned char) 4 blocks -> 4 bytes (uint32_t)
    for(int i=3; i>0; i--){
      retval = L1_cache[index][block].data[firstbib + i] << 8 | L1_cache[index][block].data[firstbib + i - 1];
    }
    return retval;
    // return L2_cache[index][block].data[bib];
  }

  //------------------------------DRAM address check-------------------------
    
  // if address is not possible to hit cache, must look up to DRAM
  // and save the result to L1 and L2 cache.
  else{
    // Find data in DRAM
    // unsigned char data = DRAM[address];
    uint32_t data;
    for(int i=3; i>0; i--){
      data = DRAM[address + i] << 8 | DRAM[address + i - 1];
    }
    //get tag(11), index(1), and byte in block(4) from an address
    index1 = getL1SetID(address);
    tag1 = getL1Tag(address);
    bib1 = ((address - (tag1 << 5)) - (index1 << 4));

    // get current position to DRAM
    unsigned char neighbors1[16];
    uint32_t current1 = ((tag1 << 1) + index1) << 4; 

    // get the data from DRAM to neighbors
    for (int i=0; i<16 ; i++, current1++ ){
      neighbors1[i] = DRAM[current1];
    }
    
    // initialize variable for looping through cache set
    int i = 0;
    int j = 0;
    int empty = 1; // 1 = full, 0 = empty

    // Edit data in L1
    // loop finding cache block that never be accessed in index1 set of L1 cache
    for (int i=0; i<2 ; i++) {
      // if cache block is empty (timestamp hadn't been set)
      if(L1_cache[index1][i].tag == 0){
        // change tag and timestamp for accessing
        L1_cache[index1][i].tag = tag1;
        L1_cache[index1][i].timeStamp = cycles;
        // assign value to cache block
        for(int j = 0 ; j < 16 ; j ++){
          L1_cache[index1][i].data[j] = neighbors1[j];
        }
        // set empty to 0 (previous processed cache block is empty)
        empty = 0;
        break;
      }
    }
      
      // if L1 cache set full is already full, 
      // replace data on the oldest cache block and set tag and timestamp
      if(empty == 1){
        // finding which cache block is older (min timestamp)
        uint32_t current_block;
        
        if(L1_cache[index1][0].timeStamp < L1_cache[index1][1].timeStamp ){
          current_block = 0; // first cache block
        }
        else{
          current_block = 1; // second cache block
        }
        
        // เปลี่ยนค่าและแก้ dirtybit ของ L2

        
        // 1) ต้องการแก้ไข(เขียน)
        // 2) read แต่พื้นที่ใน cache เต็ม และ cache นั้น dirtyBit = 1
        
        // ถ้า cacheblock นี้เคยมีการเขียนทับ
        if(L1_cache[index1][current_block].dirtyBit == 1){
          // update ค่า dirtyBit และค่าของข้อมูลลง cacheBlock นั้นใน L2
    
          uint32_t oldtag = L1_cache[index1][current_block].tag;
          uint32_t oldIndex = current_block;
          uint32_t oldAddress =  ((oldtag << 1) + oldIndex) << 4;

          
          uint32_t tag = getL2Tag(oldAddress);
          uint32_t index = getL2SetID(oldAddress);
          uint32_t bib = ((oldAddress - (tag << 6)) - (index << 4));
          int i = 0;
          for(int i=0; i<4; i++){
            if(L2_cache[index][i].tag == tag){
              // update ค่า dirtyBit ของ cacheBlock นั้น in L2
              L2_cache[index][i].dirtyBit = 1;
              // update value ของ cacheBlock นั้น in L2
              for(int j = 0 ; j < 16 ; j++){
                L2_cache[index][i].data[j] = L1_cache[index1][current_block].data[j];
              }
            }
         }
        }

        // เตะ cache block ออก (replace with new value)
        for(int j = 0 ; j < 16 ; j ++){
            L1_cache[index1][current_block].data[j] = neighbors1[j];
        }
        L1_cache[index1][current_block].tag = tag1;
        L1_cache[index1][current_block].timeStamp = cycles;
        L1_cache[index1][current_block].dirtyBit = 0;
    }
    //----------------------------------------------------------------//
    // Edit in L2
    empty = 1; // 1 = full, 0 = empty 

    // get tag(10), index(2), and byte in block(4) from an address
    index2 = getL2SetID(address);
    tag2 = getL2Tag(address);
    bib2 = ((address - (tag2 << 6)) - (index2 << 4));

    // get current2 - data start position of DRAM
    unsigned char neighbors2[16];
    uint32_t current2 = ((tag2 << 2) + index2) << 4;

    // get data to temp ("neighbor2")
    for (int i=0; i<16; i++, current2++){
      neighbors2[i] = DRAM[current2];
    }

    // Edit data in L2
    // loop finding cache block that never be accessed in index2 set of L2 cache
    for (int i=0; i<4 ; i++) {
      // if cache block is empty (timestamp hadn't been set)
      if(L2_cache[index2][i].tag == 0){
        // change tag and timestamp for accessing
        L2_cache[index2][i].tag = tag2;
        L2_cache[index2][i].timeStamp = cycles;
        // assign value to cache block (L2)
        for(int j = 0 ; j < 16 ; j ++){
          L2_cache[index2][i].data[j] = neighbors2[j];
        }
        // set empty to 0 (previous processed cache set is empty)
        empty = 0;
        break;
      }
    }
    
    // if L2 cache set full is already full, 
    // replace data on the oldest cache block and set tag and timestamp
    if(empty == 1){
        // find the oldest cache block (min timestamp)
        uint32_t current_block;
        for(int j = 0 ; j < 3 ; j++){
          if(L2_cache[index2][j].timeStamp < L2_cache[index2][j+1].timeStamp ){
            current_block = j;
          }
          else{
            current_block = j+1;
          }
        }

        // แก้ค่าใน DRAM
        // implement
        if(L2_cache[index2][current_block].dirtyBit == 1){
          uint32_t oldtag = L2_cache[index2][current_block].tag;
          uint32_t oldIndex = current_block;
          uint32_t oldAddress =  ((oldtag << 2) + oldIndex) << 4;

          for(int z = 0 ; z < 16 ; z++){
            DRAM[oldAddress+z] = L2_cache[index2][current_block].data[z];
          }
          
        }

        // replace new value to cache block and set new timestamp
        for(int j = 0 ; j < 16 ; j ++){
            L2_cache[index2][current_block].data[j] = neighbors1[j];
        }
        L2_cache[index2][current_block].tag = tag2;
        L2_cache[index2][current_block].timeStamp = cycles;
        L2_cache[index2][current_block].dirtyBit = 0;
      }

    return data;

  }
}

int L1lookup(uint32_t address)
{
  // slicing binary to  11(tag),1(index),4(BIB)
  uint32_t tag = getL1Tag(address);
  uint32_t index = getL1SetID(address);
  
  // uint32_t bib = ((address - (tag << 5)) - (index << 4));

  /**   printf("\nads:\t%016b",address);
        printf("\nlookup L1");
        printf("\ntag:\t%011b\n", tag);
        printf("index:\t%d\n", index);
        printf("bib:\t%04b\n", bib);
  */

  // use index to access set of cache block
  for(int i=0; i<2; i++){
    if(L1_cache[index][i].tag == tag){
      //printf("L1 hit\n");
      return 1;
    }
  }
  //printf("L1 miss\n");
  // check if thare is tag occurs in cache block(s) return 1
  return 0;
}

int L2lookup(uint32_t address)
{
  // slicing binary to  10(tag),2(index),4(BIB)
  uint32_t tag = getL2Tag(address);
  uint32_t index = getL2SetID(address);

  // uint32_t bib = ((address - (tag << 6)) - (index << 4)); 
  
  // printf("lookup L2");
  // printf("\ntag:\t%010b\n", tag);
  // printf("index:\t%d\n", index);
  // printf("bib:\t%04b\n", bib);

  // use index to access set of cache block
  for(int i=0; i<4; i++){
    if(L2_cache[index][i].tag == tag){
      //printf("L2 hit\n");
      return 1;
    }
  }
  // check if thare is tag occurs in cache block return 0
  //printf("L2 miss\n");
return 0;
}

unsigned int getL1SetID(uint32_t address)
{
  uint32_t index = (address - (getL1Tag(address) << 5)) >> 4;
  return index;
}

unsigned int getL2SetID(uint32_t address)
{
  uint32_t index = (address - (getL2Tag(address) << 6)) >> 4;
  return index;
}

unsigned int getL1Tag(uint32_t address)
{
  uint32_t tag = (address >> 5);
  return tag;
}

unsigned int getL2Tag(uint32_t address)
{
  uint32_t tag = (address >> 6);
  return tag;
}


void write(uint32_t address, uint32_t data)
{
  
  // check if address exist in L1 edit data
  if(L1lookup(address) == 1){
    uint32_t tag = getL1Tag(address);
    uint32_t index = getL1SetID(address);
    uint32_t bib = ((address - (tag << 5)) - (index << 4));
    unsigned char data_4[4];
    *((unsigned int*)data_4) = data;

    int i = 0;
    for(int i=0; i<2; i++){
      if(L1_cache[index][i].tag == tag){
        for(int j=0; j<4; j++){
          L2_cache[index][i].data[bib+j] = data_4[j];
        }
        L1_cache[index][i].dirtyBit = 1; // set dirtyBit
        L1_cache[index][i].timeStamp = cycles; // set timeStamp
      }
    }
  }else  if(L2lookup(address) == 1){
    //check if address exist in L2 edit data
      uint32_t tag = getL2Tag(address);
      uint32_t index = getL2SetID(address);
      uint32_t bib = ((address - (tag << 6)) - (index << 4));
      unsigned char data_4[4];
      *((unsigned int*)data_4) = data;
      int i = 0;
      for(int i=0; i<4; i++){
        if(L2_cache[index][i].tag == tag){
          for(int j=0; j<4; j++){
            L2_cache[index][i].data[bib+j] = data_4[j];
          }
          L2_cache[index][i].dirtyBit = 1; // setdirtyBit
          L2_cache[index][i].timeStamp = cycles; // set timeStamp
          // printf("cache %x\n", L2_cache[index][i].data[bib]);
         }
      }
  }else{
    // DRAM[address] = data;
    *((unsigned int*)DRAM+address/4) = data;
  }

	// read_LRU(address);
return;
}


int main()
{
	init_DRAM();
	cacheAccess buffer;
	int timeTaken=0;
	FILE *trace = fopen("./test/input.trace","r");
	int L1hit = 0;
	int L2hit = 0;
	cycles = 0;
	while(!feof(trace))
	{
		fscanf(trace,"%d %x %x", &buffer.readWrite, &buffer.address, &buffer.data);
		printf("Processing the request for [R/W] = %d, Address = %x, data = %x\n", buffer.readWrite, buffer.address, buffer.data);
    
		// Checking whether the current access is a hit or miss so that we can advance time correctly
		if(L1lookup(buffer.address))// Cache hit
		{
			timeTaken = 1;
			L1hit++;
		}
		else if(L2lookup(buffer.address))// L2 Cache Hit
		{
			L2hit++;
			timeTaken = 5;
		}
		else timeTaken = 50;
		if (buffer.readWrite) write(buffer.address, buffer.data);
		else read_LRU(buffer.address);
		cycles+=timeTaken;
	}
	printCache();
	printf("Total cycles used = %ld\nL1 hits = %d, L2 hits = %d\n", cycles, L1hit, L2hit);
	fclose(trace);
	free(DRAM);

	return 0;
}