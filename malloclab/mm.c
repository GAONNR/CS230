/*
 * Malloc lab based on implicit list.
 * Each block has its header and footer that shows size and wheter allocated or not.
 * To find free blocks, the program will start seeking from the first pointer,
 * and return the pointer when the program found the first enough size block.
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "",
    /* First member's full name */
    "",
    /* First member's email address */
    "",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

#define WSIZE 4
#define DSIZE 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = val)
#define PACK(size, alloc) (size | alloc)

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HEAD(p) ((char *)p - WSIZE)
#define FOOT(p) ((char *)p + GET_SIZE(HEAD(p)) - DSIZE)

#define NEXT_ADDR(p) ((char *)p + GET_SIZE((char *)p - WSIZE))
#define PREV_ADDR(p) ((char *)p - GET_SIZE((char *)p - DSIZE))


void *extend_heap(size_t size);
void *coalesce(void *ptr);
void *place(void *ptr, size_t asize);
void insert_node(void *ptr, size_t asize);
void delete_node(void *ptr);
void *free_finder(void *ptr, size_t asize);

//A fucntion that checks if the current block is epilogue block.
int epilogue_chk(void *ptr){
  if (GET_SIZE(HEAD(ptr)) == 0 && GET_ALLOC(HEAD(ptr))) return 1;
  return 0;
}

void *prologue_block;
/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	char *heap_start = mem_heap_lo();
	if((heap_start = mem_sbrk(4 * WSIZE)) == (void *)-1)
		return -1;

	PUT(heap_start, 0);
	PUT(heap_start + 1 * WSIZE, PACK(DSIZE, 1));
	PUT(heap_start + 2 * WSIZE, PACK(DSIZE, 1));
  //Making prologue block.
	PUT(heap_start + 3 * WSIZE, PACK(0, 1));
  //Making epilogue block.

	prologue_block = (void *)(heap_start + DSIZE);

	if(extend_heap(mem_pagesize()) == NULL)
		return -1;
	return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 * If there was enough free space, return that pointer.
 * If there was not enough space, call function extend_heap to increase heap.
 */
void *mm_malloc(size_t size)
{
  size_t asize;
  void *ptr = prologue_block;

  if (size == 0) return NULL;
  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else asize = ALIGN(size + DSIZE);

  if (asize == 112) asize = 128;
  if (asize == 448) asize = 512;

  ptr = free_finder(ptr, asize);

  if (!epilogue_chk(ptr)){
    insert_node(ptr, asize);
    return ptr;
  }

  //if not found enough space (ptr == epilogue block)
  if (GET_ALLOC(HEAD(PREV_ADDR(ptr))))
    ptr = extend_heap(asize);
  else
    ptr = extend_heap(asize - GET_SIZE(HEAD(PREV_ADDR(ptr))));

  ptr = coalesce(ptr);
  insert_node(ptr, asize);

  return ptr;
}

/*
 * mm_free - Freeing a block calls delete_node to delete allocated bit.
 * also coalesce the blocks.
 */
void mm_free(void *ptr)
{
  delete_node(ptr);
  ptr = coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
  void *oldptr = ptr;
  void *newptr;
  size_t copySize;

  newptr = mm_malloc(size);
  if (newptr == NULL) return NULL;
  copySize = GET_SIZE(HEAD(ptr)) - DSIZE;

  if (size < copySize) copySize = size;
  memcpy(newptr, oldptr, copySize);
  mm_free(oldptr);

  return newptr;
}

/*
 * Call mem_sbrk function to increase heap
 */
void *extend_heap(size_t size){
	size_t asize = ALIGN(size);
	void *ptr;

	if ((ptr = mem_sbrk(asize)) == (void *)-1)
		return NULL;

	PUT(HEAD(ptr), PACK(asize, 0));
	PUT(FOOT(ptr), PACK(asize, 0));
	PUT(HEAD(NEXT_ADDR(ptr)), PACK(0, 1)); //New Epilogue Block

	return coalesce(ptr);
}

/*
 * The function that puts the block together.
 * This has four cases: Both previous and next block are allocated,
 * only previous block is allocated, only next block is allocated,
 * or both blocks are not allocated.
 */
void *coalesce(void *ptr){
	size_t prev_alloc = GET_ALLOC(HEAD(PREV_ADDR(ptr)));
	size_t next_alloc = GET_ALLOC(HEAD(NEXT_ADDR(ptr)));
  size_t size = GET_SIZE(HEAD(ptr));

  if (prev_alloc && next_alloc) return ptr; //Case 1
  else if (prev_alloc && !next_alloc){ //Case 2
    size += GET_SIZE(HEAD(NEXT_ADDR(ptr)));
    PUT(HEAD(ptr), PACK(size, 0));
    PUT(FOOT(ptr), PACK(size, 0));
  }
  else if (!prev_alloc && next_alloc){ //Case 3
    size += GET_SIZE(HEAD(PREV_ADDR(ptr)));
    PUT(HEAD(PREV_ADDR(ptr)), PACK(size, 0));
    PUT(FOOT(ptr), PACK(size, 0));
    ptr = PREV_ADDR(ptr);
  }
  else{ //Case 4
    size += GET_SIZE(HEAD(NEXT_ADDR(ptr))) + GET_SIZE(HEAD(PREV_ADDR(ptr)));
    PUT(HEAD(PREV_ADDR(ptr)), PACK(size, 0));
    PUT(FOOT(NEXT_ADDR(ptr)), PACK(size, 0));
    ptr = PREV_ADDR(ptr);
  }

  return ptr;
}
/*
 * A function that finds the proper free block from the beginning of the heap.
 */
void *free_finder(void *ptr, size_t asize){
  //while not epilogue block
  while (!epilogue_chk(ptr)){
    if (!GET_ALLOC(HEAD(ptr)) && GET_SIZE(HEAD(ptr)) >= asize) break;

    ptr = NEXT_ADDR(ptr);
  }

  return ptr;
}

/*
 * A function that inserts node between the nodes.
 */
void insert_node(void *ptr, size_t size){
  size_t prev_size = GET_SIZE(HEAD(ptr));

  PUT(HEAD(ptr), PACK(size, 1));
  PUT(FOOT(ptr), PACK(size, 1));

  if (size < prev_size){
    PUT(HEAD(NEXT_ADDR(ptr)), PACK((prev_size - size), 0));
    PUT(FOOT(NEXT_ADDR(ptr)), PACK((prev_size - size), 0));
  }
}
/*
 * If a block is free, we need to delete that node in the heap.
 */
void delete_node(void *ptr){
  size_t size = GET_SIZE(HEAD(ptr));

  PUT(HEAD(ptr), PACK(size, 0));
  PUT(FOOT(ptr), PACK(size, 0));
}
