#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int log2(int n) {
    int r = 0;
    while (n >>= 1) {
        r++;
    }
    return r;
}

int power_two(int n) {
    return 1 << n;
}

struct block {
    int lruCount;
    char data[64];
    int validBit;
    int tagBits;
    int dirtyBit;
    int address;
};

struct block createBlock(int tag, int dirty, int blockSize, char* mainMemory, int address) {
    struct block* newBlock;
    newBlock->validBit = 1;
    newBlock->tagBits = tag;
    newBlock->address = address;
    for (int i = 0; i<blockSize; i++) {
        newBlock->data[i] = mainMemory[address+i];
    }
    return *newBlock;
}

//Function that checks if an entire set is full
int isFull(struct block** cache, int set, int ways) {
    int full = 1;
    for (int i = 0; i<ways; i++) {
        if (cache[set][i].validBit == 0) {
            full = 0;
        }
    }
    return full;
}

//Function that finds lru block in a set by finding smallest lruCount
int findLRU(struct block** cache, int set, int ways) {
    int maxLRU = 0;
    int maxIndex = 0;
    for (int i = 0; i<ways; i++) {
        if (cache[set][i].lruCount >= maxLRU) {
            maxIndex = i;
        }
    }
    return maxIndex;
}

int main(int argc, char* argv[]) {

    char mainMemory[power_two(16)];

    FILE* f = fopen(argv[1], "r+");
    
    //Establishing frames and bits for blocks
    char writeType[3];
    strcpy(writeType, argv[4]);
    int capacityKB = atoi(argv[2]);
    int blockSize = atoi(argv[5]);
    int assoc = atoi(argv[3]);

    int capacity = capacityKB*1024;
    int numFrames = capacity/blockSize;
    int numSets = numFrames/assoc;

    int offsetBits = log2(blockSize);
    int indexBits = log2(numSets);
    int tagBits = 16 - offsetBits - indexBits;

    //Declaring cache as 2D array
    struct block cache[numSets][assoc];

    for (int r = 0; r<numSets; r++) {
        for (int c = 0; c<assoc; c++) {
            for (int i = 0; i<64; i++) {
                cache[r][c].data[i] = '0';
            }
            cache[r][c].lruCount = 0;
            cache[r][c].validBit = 0;
            cache[r][c].tagBits = 0;
            cache[r][c].dirtyBit = 0;
            cache[r][c].address = 0;
        }
    }

    //Declaring variables from file lines
    char instruct[10];
    int address;
    int size;

    //Loop through the file
    while (fscanf(f, "%s", instruct) != EOF){
       fscanf(f, "%x", &address);
       fscanf(f, "%d", &size);
       char value[size];

       //Establishing address bits
       int blockOffset = address%blockSize;
       int setIndex = (address/blockSize)%numSets;
       int tag = address/(numSets*blockSize);

       //Storing value that will be stored in a store
       if (strcmp(instruct, "store") == 0) {
           for (int i=0; i<size; i++) {
               fscanf(f, "%02hhx", value+i);
           }
       }

       //Storing value that will be stored in a store
       if (strcmp(instruct, "store") == 0) {
           int hit = 0;
           for (int i = 0; i<assoc; i++) {
               if (cache[setIndex][i].tagBits == tag && cache[setIndex][i].validBit == 1) {
                   printf("%s %04x hit\n", instruct, address);
                   //Storing value in cache block and main memory if wt
                   for (int x = 0; x<size; x++) {
                       cache[setIndex][i].data[blockOffset+x] = value[x];
                       cache[setIndex][i].lruCount = 0;
                       if (strcmp(writeType, "wt") == 0) {
                           mainMemory[address+x] = value[x];
                       }
                       else {
                           cache[setIndex][i].dirtyBit = 1;
                       }
                    }
                   hit = 1;
                   break;
               }
           }
           //If a miss, store the value into main memory
           if (hit == 0) {
               printf("%s %04x miss\n", instruct, address);
                for (int i = 0; i<size; i++) {
                    mainMemory[address+i] = value[i];
                }
               if (strcmp(writeType, "wb") == 0) {
                   //struct block newBlock = createBlock(tag, 1, blockSize, mainMemory, address-blockOffset);

                    struct block* newBlock = (struct block*) malloc(sizeof(block));
                    newBlock->validBit = 1;
                    newBlock->tagBits = tag;
                    newBlock->address = address-blockOffset;
                    for (int i = 0; i<blockSize; i++) {
                        newBlock->data[i] = mainMemory[address-blockOffset+i];
                    }

                   //If full, finds LRU and replaces
                    int full = 1;
                    for (int i = 0; i<assoc; i++) {
                        if (cache[setIndex][i].validBit == 0) {
                            full = 0;
                        }
                    }
                    if (full == 1) {
                        //Finding LRU
                        int maxLRU = 0;
                        int lruIndex = 0;
                        for (int i = 0; i<assoc; i++) {
                            if (cache[setIndex][i].lruCount >= maxLRU) {
                                lruIndex = i;
                                maxLRU = cache[setIndex][i].lruCount;
                            }
                        }
                        int replaceAddress = cache[setIndex][lruIndex].address;
                        
                        if (cache[setIndex][lruIndex].dirtyBit == 1) {
                            for (int i = 0; i<64; i++) {
                                mainMemory[replaceAddress+i] = cache[setIndex][lruIndex].data[i];
                            }
                        }
                        //Adding new block to the cache
                        cache[setIndex][lruIndex] = *newBlock;
                    }
                    
                    else {
                        for (int i = 0; i<assoc; i++) {
                            if (cache[setIndex][i].validBit == 0) {
                                cache[setIndex][i] = *newBlock;
                                break;
                            }
                        }
                    }
               }
           } 
       }

        //Working with load instruction
       if (strcmp(instruct, "load") == 0) {
           int hit = 0;
           //Checking if the load is a hit
           for (int i = 0; i<assoc; i++) {
               if (cache[setIndex][i].tagBits == tag && cache[setIndex][i].validBit == 1) {
                   printf("%s %04x hit ", instruct, address);
                   //Printing value that was a hit
                   for (int x = 0; x<size; x++) {
                       printf("%02hhx", cache[setIndex][i].data[blockOffset+x]);
                   }
                   printf("\n");
                   cache[setIndex][i].lruCount = 0;
                   hit = 1;
                   break;
               }
           }
           
           //If load is a miss
           if (hit == 0) {
               printf("%s %04x miss ", instruct, address);
               //Retrieving the data from main memory
               for (int i = 0; i<size; i++) {
                   printf("%02hhx", mainMemory[address+i]);
                }
                printf("\n");
                
                
                //Creating new block as if it was in memory
                //struct block newBlock = createBlock(tag, 0, blockSize, mainMemory, address-blockOffset);

                struct block* newBlock = (struct block*) malloc(sizeof(block));
                newBlock->validBit = 1;
                newBlock->tagBits = tag;
                newBlock->address = address-blockOffset;
                for (int i = 0; i<blockSize; i++) {
                    newBlock->data[i] = mainMemory[address-blockOffset+i];
                }

                //If full, finds LRU and replaces
                int full = 1;
                for (int i = 0; i<assoc; i++) {
                    if (cache[setIndex][i].validBit == 0) {
                        full = 0;
                    }
                }
                if (full == 1) {
                    //Finding LRU
                    int maxLRU = 0;
                    int lruIndex = 0;
                    for (int i = 0; i<assoc; i++) {
                        if (cache[setIndex][i].lruCount < 0) {
                            lruIndex = i;
                            break;
                        }
                        if (cache[setIndex][i].lruCount >= maxLRU) {
                            lruIndex = i;
                            maxLRU = cache[setIndex][i].lruCount;
                        }
                    }
                    int replaceAddress = cache[setIndex][lruIndex].address;
                    //How do I evict this address correctly into main memory (NOT SURE IF THIS IS CORRECT!)
                    if (cache[setIndex][lruIndex].dirtyBit == 1) {
                        for (int i = 0; i<64; i++) {
                            mainMemory[replaceAddress+i] = cache[setIndex][lruIndex].data[i];
                        }
                    }
                    //Adding new block to the cache
                    cache[setIndex][lruIndex] = *newBlock;
                }
                
                else {
                    for (int i = 0; i<assoc; i++) {
                        if (cache[setIndex][i].validBit == 0) {
                            cache[setIndex][i] = *newBlock;
                            break;
                        }
                    }
                }
           }
           for (int r = 0; r<numSets; r++) {
               for (int c = 0; c<assoc; c++) {
                   if (cache[r][c].validBit == 1) {
                       cache[r][c].lruCount++;
                    }
                }
            }    
       }
    }
    /*
    for (int r = 0; r<numSets; r++) {
        for (int c = 0; c<assoc; c++) {
            if (cache[r][c].validBit == 0) {
                free((void*) cache[r][c]);
            }
        }
    }
    */
    return 0;
}