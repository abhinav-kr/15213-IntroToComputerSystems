/*
 * mm.c
 * akuruvad - Abhinav Kuruvadi
 *
 *  Malloc implementation using implicit lists and first fit placement
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "contracts.h"

#include "mm.h"
#include "memlib.h"


// Create aliases for driver tests
// DO NOT CHANGE THE FOLLOWING!
#ifdef DRIVER
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif


// Basic constants and helpful Macros (from CsApp)
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x,y) ((x > y) ? (x) : (y))
#define MIN(x,y) ((x < y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(uint32_t *)(p))
#define PUT(p , value) (*(uint32_t *)(p) = (value))

static char *heapPtr;


/*
 *  Logging Functions
 *  -----------------
 *  - dbg_printf acts like printf, but will not be run in a release build.
 *  - checkheap acts like mm_checkheap, but prints the line it failed on and
 *    exits if it fails.
 */

#ifndef NDEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#define checkheap(verbose) do {if (mm_checkheap(verbose)) {  \
                             printf("Checkheap failed on line %d\n", __LINE__);\
                             exit(-1);  \
                        }}while(0)
#else
#define dbg_printf(...)
#define checkheap(...)
#endif

/*
 *  Helper functions
 *  ----------------
 */

// Align p to a multiple of w bytes
static inline void* align(const void const* p, unsigned char w) {
    return (void*)(((uintptr_t)(p) + (w-1)) & ~(w-1));
}

// Check if the given pointer is 8-byte aligned
static inline int aligned(const void const* p) {
    return align(p, 8) == p;
}

// Return whether the pointer is in the heap.
static int in_heap(const void* p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}


/*
 *  Block Functions
 *  ----------------
 */

// Return the size of the given block in multiples of the word size
static inline unsigned int block_size(const uint32_t* block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    return (block[0] & ~0x7);
}

// Return True if the block is free, False otherwise
static inline int block_free(const uint32_t* block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    return !(block[0] & 0x1);
}

// Mark the given block as free(1)/alloced(0) by marking the header and footer.
static inline void block_mark(uint32_t* block, int free) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    unsigned int size = block_size(block);
    PUT(block, PACK(size, free));
    PUT((block + (size/WSIZE) - 1), PACK(size, free));
}

// Return a pointer to the memory malloc should return
static inline uint32_t* block_mem(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    REQUIRES(aligned(block + 1));
    
    return block + 1;
}

// Return the header to the previous block
static inline uint32_t* block_prev(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    return block - (block_size(block - 1)/WSIZE);
}

// Return the header to the next block
static inline uint32_t* block_next(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    return block + (block_size(block)/WSIZE);
}

// Return header of the current block
static inline uint32_t* hdrp(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    if ( !in_heap(bp)) {
        return NULL;
    }
    
    return bp - 1;
}

// Return footer of the current block
static inline uint32_t* ftrp(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    REQUIRES(in_heap(bp));
    
    return bp + (block_size(hdrp(bp))/WSIZE) - 2;
}

// Return a pointer to the next block
static inline uint32_t* nextblkptr(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    REQUIRES(in_heap(bp));
    
    return bp + (block_size(hdrp(bp))/WSIZE);
}

//Return a pointer to the previous block
static inline uint32_t* prevblkptr(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    REQUIRES(in_heap(bp));
    
    return bp - (block_size(hdrp(bp) - 1)/WSIZE);
}

//Coalesce free blocks
static void *coalesce(void *bp) {
    
    int isPrevBlockFree = block_free(block_prev(hdrp(bp)));
    int isNextBlockFree = block_free(block_next(hdrp(bp)));
    size_t size = block_size(hdrp(bp));
    
    
    if(!isPrevBlockFree && !isNextBlockFree) {
        /* When both adjoining blocks are allocated */
        return bp;
        
    } else if(!isPrevBlockFree && isNextBlockFree) {
        /* When only the next block is free */
        size += block_size(block_next(hdrp(bp)));
        PUT(hdrp(bp), PACK(size, 0));
        PUT(ftrp(bp), PACK(size, 0));
        
    } else if(isPrevBlockFree && !isNextBlockFree) {
        /* When only the prev block is free */
        size += block_size(block_prev(hdrp(bp)));
        PUT(hdrp(prevblkptr(bp)), PACK(size, 0));
        PUT(ftrp(bp), PACK(size, 0));
        bp = prevblkptr(bp);
        
    } else if(isPrevBlockFree && isNextBlockFree) {
        /*  When both adjoining blocks are free */
        size += block_size(block_next(hdrp(bp))) + block_size(block_prev(hdrp(bp)));
        PUT(hdrp(prevblkptr(bp)), PACK(size, 0));
        PUT(ftrp(nextblkptr(bp)), PACK(size, 0));
        bp = prevblkptr(bp);
        
    }
    checkheap(1);
    return bp;
}

//Extend the heap
static void *extend_heap(size_t words) {
    
    //Even number of words to maintain alignment
    size_t size = (words % 2) ? ((words + 1) * WSIZE) : (words * WSIZE);
    uint32_t *bp = mem_sbrk(size);
    
    if((long)bp == -1)
        return NULL;
    
    /* Add a header, footer and an epilogue header */
    PUT(hdrp(bp), PACK(size, 0));
    PUT(ftrp(bp), PACK(size, 0));
    PUT(block_next(hdrp(bp)), PACK(0,1));
    
    return coalesce(bp);
}

//Find the first fit
static void *find_fit(size_t size) {
    
    uint32_t *ptr = (uint32_t *)heapPtr + 2;
    unsigned int blkSize = block_size(ptr);
   
    while(blkSize > 0) {
        
        if((size <= blkSize) && block_free(ptr)) {
            /* Found a free block with sufficient size */
            
            return (void *)block_mem(ptr);
        }
        
        ptr = block_next(ptr);
        blkSize = block_size(ptr);
    }
    
    return NULL;
}


//Placing the allocated block in the free block
static void place(void *bp, size_t size) {
    
    size_t blkSize = block_size(hdrp(bp));
    
    if((blkSize-size) >= (2*DSIZE)) {
        /* If enough block size remains, split the block */

        PUT(hdrp(bp), PACK(size, 1));
        PUT(ftrp(bp), PACK(size, 1));
        bp = nextblkptr(bp);
        PUT(hdrp(bp), PACK(blkSize-size, 0));
        PUT(ftrp(bp), PACK(blkSize-size, 0));
        
    } else {
        /* If the block cannot be splitted, just occupy the free block */
        
        PUT(hdrp(bp), PACK(blkSize, 1));
        PUT(ftrp(bp), PACK(blkSize, 1));
        
    }
}

/*
 *  Malloc Implementation
 *  ---------------------
 *  The following functions deal with the user-facing malloc implementation.
 */

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void) {
    
    /* Set heap to be empty inititially */
    if((heapPtr = mem_sbrk(4*WSIZE)) == (void *)-1) {
        return -1;
    }
    
    PUT(heapPtr, 0);
    PUT(heapPtr + (1*WSIZE), PACK(DSIZE, 1));
    PUT(heapPtr + (2*WSIZE), PACK(DSIZE, 1));
    PUT(heapPtr + (3*WSIZE), PACK(0,1));
    heapPtr += WSIZE;
    
    /* Add CHUNKSIZE bytes to heap */
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        return -1;
    }
    
    return 0;
}

/*
 * malloc
 */
void *malloc (size_t size) {
    
    size_t adjustedBlockSize;
    size_t extendedSize;
    uint32_t *bp = NULL;
    
    /* Ignore bad requests */
    if(size <= 0)
        return NULL;
    
    /* Adjust block size */
    if(size <= DSIZE)
        adjustedBlockSize = 2*DSIZE;
    else
        adjustedBlockSize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    
    /* Searh for a fit */
    if( (bp = find_fit(adjustedBlockSize)) != NULL ) {
        place(bp, adjustedBlockSize);
        return bp;
    }
    
    /* If no fit found, request more blocks to be added to heap */
    extendedSize = MAX(adjustedBlockSize, CHUNKSIZE);
    if( (bp = extend_heap(extendedSize/WSIZE)) != NULL) {
        place(bp, adjustedBlockSize);
        return bp;
    }

    return NULL;
}

/*
 * free
 */
void free (void *ptr) {
    
    if (ptr == NULL) {
        return;
    }
    
    block_mark(hdrp(ptr), 0);
    coalesce(ptr);
}

/*
 * realloc - Updated the realloc function of mm-naive.c
 */
void *realloc(void *oldptr, size_t size)
{
    size_t oldsize;
    void *newptr;
    
    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        free(oldptr);
        return 0;
    }
    
    /* If oldptr is NULL, then this is just malloc. */
    if(oldptr == NULL) {
        return malloc(size);
    }
    
    newptr = malloc(size);
    
    /* If realloc() fails the original block is left untouched  */
    if(!newptr) {
        return 0;
    }
    
    /* Copy the old data. */
    oldsize = block_size(hdrp(oldptr));
    
    if(size < oldsize) {
        oldsize = size;
    }
    
    memcpy(newptr, oldptr, oldsize);
    
    /* Free the old block. */
    free(oldptr);
    
    return newptr;
}

/*
 * calloc - Updated the calloc function of mm-naive.c
 */
void *calloc (size_t nmemb, size_t size) {
    size_t bytes = nmemb * size;
    void *newptr;
    
    newptr = malloc(bytes);
    memset(newptr, 0, bytes);
    
    return newptr;
}

// Returns 0 if no errors were found, otherwise returns the error
int mm_checkheap(int verbose) {
    
    printf("\n*****************\n");

    uint32_t *ptr = (uint32_t *)heapPtr;
    unsigned int blkSize = block_size(ptr);
    
    while(blkSize > 0) {
        printf("%x \n",  *ptr);
        ptr = block_next(ptr);
        blkSize = block_size(ptr);
    }
    
    printf("%x \n", *ptr);
    printf("Block size: %d Block is free: %d\n", blkSize, block_free(ptr));
    printf("\n*****************\n");

    
    verbose = verbose;
    return 0;
}
