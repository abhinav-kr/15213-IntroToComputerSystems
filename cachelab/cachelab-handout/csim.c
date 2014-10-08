/* 
 Name      : Abhinav Kuruvadi
 Andrew-ID : akuruvad
 */

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include "cachelab.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct {
    int validBit;
    int tag;
    int data;
    int lastUsedTimeStamp;
} Line;

typedef struct {
    Line* arrayOfLines;
} Set;

int timeStamp = 0;
int noOfHits = 0;
int noOfMisses = 0;
int noOfEvictions = 0;
int noOfLinesPerSet = 0;
int noOfSets = 0;
Set *arrayOfSets;

void simulateLoadOperation( int setToSearch, int tagBits ) {
    
    int i;
    int tagNotFound = 1;
    int needToEvictMemory = 1;
    int lineThatHasToBeEvicted = 0;
    int minTimeStamp = 0;
    
    for ( i = 0 ; i < noOfLinesPerSet; i++ ) {
        
        if ( arrayOfSets[setToSearch].arrayOfLines[i].validBit && arrayOfSets[setToSearch].arrayOfLines[i].tag == tagBits ) {
            
            noOfHits++;
            
            arrayOfSets[setToSearch].arrayOfLines[i].lastUsedTimeStamp = timeStamp;
            tagNotFound = 0;
            break;
        }
    }
    
    if ( tagNotFound ) {
        
        noOfMisses++;
        
        for ( i = 0 ; i < noOfLinesPerSet; i++ ) {
            
            if ( arrayOfSets[setToSearch].arrayOfLines[i].validBit == 0 ) {
                
                arrayOfSets[setToSearch].arrayOfLines[i].validBit = 1;
                arrayOfSets[setToSearch].arrayOfLines[i].tag = tagBits;
                arrayOfSets[setToSearch].arrayOfLines[i].lastUsedTimeStamp = timeStamp;
                needToEvictMemory = 0;
                break;
            }
        }
    }
    
    if ( tagNotFound && needToEvictMemory ) {
        
        noOfEvictions++;
        
        minTimeStamp = arrayOfSets[setToSearch].arrayOfLines[0].lastUsedTimeStamp;
        
        for ( i = 0 ; i < noOfLinesPerSet; i++ ) {
            if ( arrayOfSets[setToSearch].arrayOfLines[i].lastUsedTimeStamp < minTimeStamp ) {
                minTimeStamp = arrayOfSets[setToSearch].arrayOfLines[i].lastUsedTimeStamp;
                lineThatHasToBeEvicted = i;
            }
        }
        
        arrayOfSets[setToSearch].arrayOfLines[lineThatHasToBeEvicted].validBit = 1;
        arrayOfSets[setToSearch].arrayOfLines[lineThatHasToBeEvicted].tag = tagBits;
        arrayOfSets[setToSearch].arrayOfLines[lineThatHasToBeEvicted].lastUsedTimeStamp = timeStamp;
        
    }
}

void simulateStoreOperation( int setToSearch, int tagBits ) {
    
    //It is exactly the same process as in case of load operation from the perspective of a cache.
    simulateLoadOperation(setToSearch, tagBits);
}


void processTraceLine( char operationType, int address, int noOfBytes, int s, int E, int b ) {
    
    int maskUptoSetBits, maskUptoTagBits;
    int setToSearch, tagBits;
    
    maskUptoSetBits =  ((~(1<<31))>>(31-s));
    setToSearch = ((address>>b)&maskUptoSetBits);
    
    maskUptoTagBits = ((~(1<<31))>>(31-(s+b)));
    tagBits = ((address>>(s+b))&maskUptoTagBits);
    
    
    switch ( operationType ) {
            
            case 'M':
                simulateLoadOperation(setToSearch, tagBits);
                simulateStoreOperation(setToSearch, tagBits);
                break;
            
            case 'L':
                simulateLoadOperation(setToSearch, tagBits);
                break;
            
            case 'S':
                simulateStoreOperation (setToSearch, tagBits);
                break;
            
            default:
                printf ("Illegal Operation!");
                break;
    }
}

void initializeCache ( int s, int E ) {
    
    int i, j, c=0;
    
    noOfSets = pow(2,s);
    noOfLinesPerSet = E;
    
    arrayOfSets = malloc ( sizeof(Set) * noOfSets );
    
    //Same logic as initializing a 2-D array.
    for (i = 0 ; i < noOfSets ; i ++) {
        arrayOfSets[i].arrayOfLines = malloc ( sizeof(Line) * noOfLinesPerSet ) ;
        
        //Must set all validBits to zero manually when initialized
        for ( j = 0; j < E ; j++ ) {
            arrayOfSets[i].arrayOfLines[j].validBit = 0;
            c++;
        }
    }
}

int main (int argc, char **argv) {
    
    int opt;
    
    int s, E, b;
    char traceFileName[1000];
    FILE* traceFileP;

    char operationType;
    int address, noOfBytes;
    
    while ( (opt = getopt(argc, argv, " s:E:b:t:")) != -1) {
        switch (opt) {
                
                case 's':
                    s = atoi(optarg);
                    break;
                
                case 'E':
                    E = atoi(optarg);
                    break;
                
                case 'b':
                    b = atoi(optarg);
                    break;
                
                case 't':
                    strcpy( traceFileName, optarg );
                    break;
                
                default :
                    printf ("WRONG OPTION!");
                    exit(0);
                
        }
    }
    
//    printf ( "\n Input values are : %d %d %d %s\n", s, E, b, traceFileName);
    
    initializeCache( s, E);
    
    if( (traceFileP = fopen ( traceFileName, "r")) == 0 ) {
        printf ("\nCould not open the file - %s\n", traceFileName);
    }
    
    while ( fscanf(traceFileP, " %c %x,%d", &operationType, &address, &noOfBytes) != -1)  {
        timeStamp++;
        processTraceLine( operationType, address, noOfBytes, s, E, b );
    }
    
    printSummary(noOfHits, noOfMisses, noOfEvictions);
    
    return 0;
}
