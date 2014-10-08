/*
 * mm.c
 * akuruvad - Abhinav Kuruvadi
 *
 *  Malloc implementation using explicit lists and first fit placement.
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

/*Defining constants and macros that will be used in malloc implementation*/
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x,y) ((x > y) ? (x) : (y))
#define MIN(x,y) ((x < y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(uint32_t *)(p))
#define PUT(p , value) (*(uint32_t *)(p) = (value))

#define correctedAddress(addr) (uint32_t *)(((unsigned long)1 << 35) | addr)

#define BLOCK_NEXT_PTR(block) (block+1)
#define BLOCK_PREV_PTR(block) (block+2)

static uint32_t *heapPtr;

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
 *  ---------------
 *  TODO: Add your comment describing block functions here.
 *  The functions below act similar to the macros in the book, but calculate
 *  size in multiples of 4 bytes.
 */

// Return the size of the given block in multiples of the word size
static inline unsigned int block_size(const uint32_t* block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    return (block[0] & ~0x7);
}

// Return true if the block is free, false otherwise
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

//Return header of the current block
static inline uint32_t* hdrp(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    REQUIRES(in_heap(bp));
    
    return bp - 1;
}

static inline uint32_t* ftrp(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    return block + (block_size(block)/WSIZE) - 1;
}

//Return block pointer of the next block
static inline uint32_t* next_bp(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    REQUIRES(in_heap(bp));
    
    return bp + (block_size(hdrp(bp))/WSIZE);
}

//Returns block pointer of the previous block
static inline uint32_t* prev_bp(uint32_t* const bp) {
    REQUIRES(bp != NULL);
    REQUIRES(in_heap(bp));
    
    return bp - (block_size(hdrp(bp) - 1)/WSIZE);
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

//Places the free block at the front of the heap
static inline void *enqueueBlock(uint32_t* const block) {
    REQUIRES(block != NULL);
    REQUIRES(in_heap(block));
    
    PUT(BLOCK_NEXT_PTR(block), *(BLOCK_NEXT_PTR(heapPtr)));
    PUT(BLOCK_PREV_PTR(block), (int)(unsigned long)heapPtr);
    
    PUT(BLOCK_NEXT_PTR(heapPtr), (int)(unsigned long)block);
    
    /* If the next block is not epilogue, set the prev pointer of the next block. */
    if(block_free(correctedAddress((unsigned long)(*(BLOCK_NEXT_PTR(block))))))
        PUT(correctedAddress((unsigned long)(*(BLOCK_NEXT_PTR(block)) + 8)), (int)(unsigned long)block);
    
    return block;
}

//Remove a block from the free block list and sets the prev and next pointers accordingly
static inline void removeBlock(uint32_t *block) {
    uint32_t *prev = correctedAddress((unsigned long)(*(BLOCK_PREV_PTR(block))));
    uint32_t *next = correctedAddress((unsigned long)(*(BLOCK_NEXT_PTR(block))));
    PUT(BLOCK_NEXT_PTR(prev), *(BLOCK_NEXT_PTR(block)));
    if(block_free(next))
        PUT(BLOCK_PREV_PTR(next), *(BLOCK_PREV_PTR(block)));
}

//Coalesce free blocks if possible
static inline void *coalesce(uint32_t *bp) {
    
    if(!in_heap(ftrp(bp)+1))
        return enqueueBlock(bp);
    
    size_t isPrevBlockFree = block_free(bp-1);
    size_t isNextBlockFree = block_free(ftrp(bp)+1);
    size_t size = block_size(bp);
    
    if(!isPrevBlockFree && !isNextBlockFree) {
        /* When both adjoining blocks are allocated */
        return enqueueBlock(bp);
    }

    else if(!isPrevBlockFree && isNextBlockFree) {
        /* When only the next block is free, remove it from the list, merge and add it back */
        uint32_t *nextblkptr = ftrp(bp)+1;
        size += block_size(nextblkptr);
        PUT(bp, PACK(size, 0));
        PUT(ftrp(nextblkptr), PACK(size, 0));
        removeBlock(nextblkptr);
        return enqueueBlock(bp);
    }
    else if(isPrevBlockFree && !isNextBlockFree) {
        /* When only the prev block is free, merge it to the current block */

        uint32_t *prevblkptr = bp - (block_size(bp-1)/WSIZE);
        size += block_size(prevblkptr);
        PUT(prevblkptr, PACK(size, 0));
        PUT(ftrp(bp), PACK(size, 0));
        return prevblkptr;
    }

    else if(isPrevBlockFree && isNextBlockFree) {
        /*  When both adjoining blocks are free, join all 3 and add to front of queue */

        uint32_t *nextblkptr = ftrp(bp)+1;
        uint32_t *prevblkptr = bp - (block_size(bp-1)/WSIZE);
        size += block_size(prevblkptr) + block_size(nextblkptr);
        PUT(prevblkptr, PACK(size, 0));
        PUT(ftrp(nextblkptr), PACK(size, 0));
        removeBlock(prevblkptr);
        removeBlock(nextblkptr);
        return enqueueBlock(prevblkptr);
    }
    return NULL;
}

//Extend the heap
static inline void *extend_heap(size_t words) {
    
    /* Even number of words to maintain alignment */
    size_t size = (words % 2) ? ((words + 1) * WSIZE) : (words * WSIZE);
    uint32_t *bp = mem_sbrk(size);
    
    if((long)bp == -1)
        return NULL;
    
    /* Add a header and footer */
    PUT(bp, PACK(size, 0));
    PUT(ftrp(bp), PACK(size, 0));
    
    return coalesce(bp);
}

//Find the first fit
static inline void *find_fit(size_t size) {
    uint32_t *ptr = heapPtr;
    unsigned int blkSize = block_size(ptr);
    while(blkSize > 0) {
        if((size <= blkSize) && block_free(ptr)) {
            /* Found a free block with sufficient size */

            return (void *)block_mem(ptr);
        }
        ptr = correctedAddress((unsigned long)(*(BLOCK_NEXT_PTR(ptr))));
        blkSize = block_size(ptr);
    }
    return NULL;
}


//Placing the allocated block in the free block
static inline void place(void *bp, size_t size) {
    uint32_t *block = (uint32_t *)bp - 1;
    size_t blkSize = block_size(block);
    
    if((blkSize-size) >= (2*DSIZE)) {
        /* Set the block header and footer to occupied. */
        PUT(block, PACK(size, 1));
        PUT(ftrp(block), PACK(size, 1));
        
        uint32_t *temp = ftrp(block) + 2;

        PUT(temp-1, PACK(blkSize-size, 0));
        PUT(ftrp(temp-1), PACK(blkSize-size, 0));
        
        /* Set the prev and next pointers of the block to the reduced free block */
        PUT(BLOCK_NEXT_PTR(temp-1), (int)(unsigned long)*(BLOCK_NEXT_PTR(block)));
        PUT(BLOCK_PREV_PTR(temp-1), (int)(unsigned long)*(BLOCK_PREV_PTR(block)));
        
        uint32_t *next = correctedAddress((unsigned long)*(BLOCK_NEXT_PTR(temp-1)));
        uint32_t *prev = correctedAddress((unsigned long)*(BLOCK_PREV_PTR(temp-1)));
        PUT(BLOCK_NEXT_PTR(prev), (int)(unsigned long)(temp - 1));
        if( block_free(next)) {
            PUT(BLOCK_PREV_PTR(next), (int)(unsigned long)(temp - 1));
        }
    }
    else {
        /* If the block cannot be split, just occupy the free block */

        PUT(block, PACK(blkSize, 1));
        PUT(ftrp(block), PACK(blkSize, 1));
        uint32_t *next = correctedAddress((unsigned long)*(BLOCK_NEXT_PTR(block)));
        uint32_t *prev = correctedAddress((unsigned long)*(BLOCK_PREV_PTR(block)));
        if( block_free(next) ) {
            PUT(BLOCK_PREV_PTR(next), (int)(unsigned long)prev);
        }
        PUT(BLOCK_NEXT_PTR(prev), (int)(unsigned long)next);
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
    if((heapPtr = mem_sbrk(5*WSIZE)) == (void *)-1)
        return -1;
    
    PUT(heapPtr, 0);
    PUT(heapPtr + 1, PACK(2*DSIZE, 1));
    PUT(heapPtr + 2, (uint32_t)(unsigned long)(heapPtr + 4));
    PUT(heapPtr + 3, PACK(2*DSIZE, 1));
    PUT(heapPtr + 4, PACK(0,1));
    heapPtr += 1;

    /* Add CHUNKSIZE bytes to heap */
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
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
    if(size == 0)
        return NULL;
    
    /* Adjust block size */
    if(size <= DSIZE)
        adjustedBlockSize = 2*DSIZE;
    else
        adjustedBlockSize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    
    /* Searh for a fit */
    bp = find_fit(adjustedBlockSize);
    if(bp != NULL) {
        place(bp, adjustedBlockSize);
        return bp;
    }
    
    /* If no fit found, request more blocks to be added to heap */
    extendedSize = MAX(adjustedBlockSize, CHUNKSIZE);
    bp = extend_heap(extendedSize/WSIZE);
    if(bp != NULL) {
        place(bp+1, adjustedBlockSize);
        return bp+1;
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
    //Set the block as free
    block_mark((uint32_t *)ptr-1, 0);
    //Coalesce if possible
    coalesce((uint32_t *)ptr-1);
}

/*
 * realloc
 */
void *realloc(void *oldptr, size_t size) {
    size_t oldsize;
    void *newptr;
    
    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        free(oldptr);
        return 0;
    }
    
    /* If oldptr is NULL, then this is just malloc. */
    if(oldptr == NULL)
        return malloc(size);
    
    newptr = malloc(size);
    
    /* If realloc() fails the original block is left untouched  */
    if(!newptr)
        return 0;
    
    oldsize = block_size(hdrp(oldptr));
    if(size < oldsize)
        oldsize = size;
    memcpy(newptr, oldptr, oldsize);

    /* Free the old block. */
    free(oldptr);
    
    return newptr;
}

/*
 * calloc
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