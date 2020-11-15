`/* 
* Ethan Trott
* COS235 - HW3
* Spring 2020
*
* emulates a cache using either direct or associative mapping
*/

#include <stdio.h>

//-------------------------------------------------

// CACHE_TYPE 0 is direct mapping
// CACHE_TYPE 1 is associative mapping
#define CACHE_TYPE 0

// array of values to insert into cache
int testValues[] = {0, 6, 78, 38, 234, 30, 52, 29, 26, 3, 80, 160, 180, 162, 188, 183, 163, 27, 29, 30, 26, 31, 210, 230, 255, 213, 211, 232, 253, 40, 46, 42, 49, 232, 88, 7, 22, 44, 9, 13, 14, 11, 23, 233, 55, 88, 86, 57};

//-------------------------------------------------

int mainMemory[256][4];         // main memory  - 256 rows of 4 bytes each (1KB total)
int cache[8][5];                // cache        - 8 rows of 4 bytes (+ tag byte) each
int numHits = 0;                // counts amount of cache hits upon search

// initializes the memory
void initializeMemory(){
    // initialize main memory to each location's address
    for (int r=0; r<256; r++){
        for (int c=0; c<4; c++){
            mainMemory[r][c]= (r*4)+c;
        }
    }

    // initialize cache line tags to -1
    for (int i=0; i<8; i++)
        cache[i][0] = -1;
}

// converts "input" to binary and stores the first 8 bits in "binary"
void decimalToBinary(int input, int *binary){

    int digits[32];							// holds all converted digits
                                            // 32 is the most we'll need
    int digitsCompleted = 0;                // track digits completed
    
    //keep calculating new digits until there are none left
    //or we run out of space in our array
    while (input != 0){
        //store the remainder
        digits[digitsCompleted] = input % 2;
        
        //get next value to divide
        input = input / 2;
        
        digitsCompleted++;
    }
    
    
    int charCount = 0;
    // store first 8 digits in "binary" in the correct (non-reversed) order
    for (int i=digitsCompleted-1; i>=0; i--)
        binary[7-i] = digits[i];
}

// stores block with index "blockNum" in the cache line with index "cacheLine" with tag "tag"
void storeBlock(int blockNum, int cacheLine, int tag){
    printf("Storing block %d in line %d with tag %d..\n", blockNum, cacheLine, tag);

    // the first index of the cache line holds the tag
    cache[cacheLine][0] = tag;
    for (int i=0; i<4; i++)
        cache[cacheLine][i+1] = mainMemory[blockNum][i];
}

// check if "address" is cached. if not, cache it. (direct mapping)
void cacheDirect(int address){
    //convert the address to binary
    int binary[8] = {0};
    decimalToBinary(address, binary);

    printf("Binary Value: ");
    for (int i=0; i<8; i++)
        printf("%d", binary[i]);
    printf("\n");

    //caclulate the tag, cache line index, offset, and block number
    int tag = binary[2] + binary[1]*2 + binary[0]*4;
    printf("Tag: %d\n", tag);

    int cacheLine = binary[5] + binary[4]*2 + binary[3]*4;
    printf("Line Index: %d\n", cacheLine);

    int offset = binary[7] + binary[6]*2;
    printf("Offset: %d\n\n", offset);

    int blockNum = tag*8 + cacheLine;       //the magic of powers of two

    // if the cache line is empty, store the block there
    if (cache[cacheLine][0] == -1){
        printf("Miss: cache line %d is empty \n", cacheLine);

        storeBlock(blockNum, cacheLine, tag);
        
    }else{                                  //cache line is not empty
        // this is a cache hit if this block is already stored on this line
        if (cache[cacheLine][0] == tag){
            numHits++;
            printf("Hit: cache line %d already contains tag %d \n", cacheLine, tag);
            printf("Total Hits: %d\n", numHits);
        }else{
            printf("Miss: cache line %d contains a different tag (%d) \n", cacheLine, cache[cacheLine][0]);
            storeBlock(blockNum, cacheLine, tag);
        }
    }
}  

// check if "address" is cached. if not, cache it. (associative mapping)
void cacheAssociative(int address){
    // statically stores the next cacheLine to be filled (incremented on fill)
    static int counter = 0;

    //convert the address to binary
    int binary[8] = {0};
    decimalToBinary(address, binary);

    printf("Binary Value: ");
    for (int i=0; i<8; i++)
        printf("%d", binary[i]);
    printf("\n");

    //caclulate the tag, offset, and block number
    int tag = binary[5] + binary[4]*2 + binary[3]*4 + binary[2]*8 + binary[1]*16 + binary[0] * 32;
    printf("Tag: %d\n", tag);

    int offset = binary[7] + binary[6]*2;
    printf("Offset: %d\n\n", offset);

    // convenient coincidence
    int blockNum = tag;

    // search to see if this block is already stored
    int found = 0;
    for (int i=0; i<8; i++){
        // block already stored (cache hit) if tag matches
        if (cache[i][0] == tag){
            numHits++;
            printf("Hit: cache line %d already contains tag %d \n", i, tag);
            printf("Total Hits: %d\n", numHits);

            found = 1;
            break;
        }
    }

    //if not found (cache miss), store in next cache line
    if (!found){
        printf("Miss: no cache lines contained tag %d \n", tag);
        storeBlock(blockNum, counter, tag);

        //increment cacheLine for next fill
        counter = (counter + 1) % 8;
    }
}

int main(){
    initializeMemory();

    // for each value in testValues, store using correct cache type
    int numberOfValues = sizeof(testValues)/sizeof(testValues[0]);
    for (int i=0; i<numberOfValues; i++){
        printf("---------------\n");
        printf("Inserting %d..\n\n", testValues[i]);

        if (!CACHE_TYPE)
            cacheDirect(testValues[i]);
        else
            cacheAssociative(testValues[i]);
    }

    // print statistics
    printf("---------------\n");
    printf("Total Hits: %d\n", numHits);
    printf("Total Misses: %d\n", numberOfValues - numHits);
    printf("Hit Rate: %.2f%%\n", ((float)numHits/numberOfValues)*100);
}