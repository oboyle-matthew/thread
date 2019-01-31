#include <stdio.h>  // needed for size_t
#include <sys/mman.h> // needed for mmap
#include <assert.h> // needed for asserts
#include "dmm.h"

/* You can improve the below metadata structure using the concepts from Bryant
 * and OHallaron book (chapter 9).
 */

typedef struct metadata {
  /* size_t is the return type of the sizeof operator. Since the size of an
   * object depends on the architecture and its implementation, size_t is used
   * to represent the maximum size of any object in the particular
   * implementation. size contains the size of the data object or the number of
   * free bytes
   */
  size_t size;
  struct metadata* next;
  struct metadata* prev; 
} metadata_t;

/* freelist maintains all the blocks which are not in use; freelist is kept
 * sorted to improve coalescing efficiency 
 */

static metadata_t* freelist = NULL;

void* dmalloc(size_t numbytes) {
  /* initialize through mmap call first time */
  if(freelist == NULL) {      
    if(!dmalloc_init())
      return NULL;
  }

  assert(numbytes > 0);

  /* your code here */

  // loop through freelist (linked list) and check if size (size + header) is > numbytes ... access via ->size
  // update linked list to point to the split free memory at the bottom ... block.prev.next ? where block is an instance of metadata
    // if it fits exactly... update linked list to next free block ... list.next = list.next.next, skip over the block we just filled
  // update size of blocks
  // pointer should point to the start of the data block, skipped past the header
  // if no more free room... error message?


  metadata_t *freelist_temp = freelist; //initially points to head of freelist

  metadata_t *tempprevious;
  metadata_t *tempnext;


  size_t beforesplitsize;

  void *userpointer;

  while (freelist_temp != NULL) {

    if (((freelist_temp->size)+sizeof(metadata_t)) > (numbytes + sizeof(metadata_t))) {
      tempprevious = freelist_temp->prev;
      tempnext = freelist_temp->next;
      if (tempprevious != NULL) {
        tempprevious->next = tempnext;
        if (tempnext != NULL) {
          tempnext->prev = tempprevious;

          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

          freelist_temp = ((void*)freelist_temp) + sizeof(metadata_t) + numbytes;

          tempprevious->next = freelist_temp;
          freelist_temp->prev = tempprevious;

          freelist_temp->next = tempnext;
          tempnext->prev = freelist_temp;

          freelist_temp->size = beforesplitsize - numbytes - sizeof(metadata_t) - sizeof(metadata_t);

        }
        else {
          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

          freelist_temp = ((void*)freelist_temp) + sizeof(metadata_t) + numbytes;

          tempprevious->next = freelist_temp;
          freelist_temp->prev = tempprevious;

          freelist_temp->next = tempnext;

          freelist_temp->size = beforesplitsize - numbytes - sizeof(metadata_t) - sizeof(metadata_t);

        }
      }
      else {
        if (tempnext != NULL) {
          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

          freelist_temp = ((void*)freelist_temp) + sizeof(metadata_t) + numbytes;

          freelist_temp->next = tempnext;
          tempnext->prev = freelist_temp;

          freelist_temp->prev = tempprevious;

          freelist_temp->size = beforesplitsize - numbytes - sizeof(metadata_t) - sizeof(metadata_t);

          freelist = freelist_temp;
        //  freelist->next = freelist_temp->next;
        //  freelist->prev = freelist_temp->prev;
        //  freelist->size = freelist_temp->size;
        }
        else {
          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

          freelist_temp = ((void*)freelist_temp) + sizeof(metadata_t) + numbytes;

          freelist_temp->next = tempnext;
          freelist_temp->prev = tempprevious;

          freelist_temp->size = beforesplitsize - numbytes - sizeof(metadata_t) - sizeof(metadata_t);

          freelist = freelist_temp;
        //  freelist->next = freelist_temp->next;
        //  freelist->prev = freelist_temp->prev;
        //  freelist->size = freelist_temp->size;
        }
      }
    return userpointer;
    /*  
      //split the block, update size of split blocks
      beforesplitsize = freelist_temp->size + sizeof(metadata_t);
      freelist_temp->size = numbytes;
      userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

      freelist_temp = ((void*)freelist_temp) + sizeof(metadata_t) + numbytes;
      // insert the split block BACK into the freelist

      if (previous != NULL) {
        previous->next = freelist_temp;
      }
      else if (previous==NULL && freelist_temp->next != NULL) {
        freelist_temp->next->prev = NULL;
        freelist = freelist_temp;
      }

      freelist_temp->size = beforesplitsize - numbytes - sizeof(metadata_t) - sizeof(metadata_t);

      //freelist_temp = freelist_temp->next;
*/
    }

    if (((freelist_temp->size)+sizeof(metadata_t)) == (numbytes + sizeof(metadata_t))) {
      tempprevious = freelist_temp->prev;
      tempnext = freelist_temp->next;
      if (tempprevious != NULL) {
        tempprevious->next = tempnext;
        if (tempnext != NULL) {
          tempnext->prev = tempprevious;

          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);
        }
        else {
          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);
        }
      }
      else {
        if (tempnext != NULL) {
          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

          freelist = tempnext;
        //  freelist->next = freelist_temp->next;
        //  freelist->prev = freelist_temp->prev;
        //  freelist->size = freelist_temp->size;
        }
        else {
          beforesplitsize = freelist_temp->size + sizeof(metadata_t);
          freelist_temp->size = numbytes;
          userpointer = ((void*)freelist_temp) + sizeof(metadata_t);

          freelist_temp = NULL;
        }
      }
/*
      previous = freelist_temp->prev;
      if (previous != NULL) {
        previous->next = freelist_temp->next;
        if (freelist_temp->next != NULL) {
          freelist_temp->next->prev = previous;
        }
      }
      userpointer = freelist_temp + sizeof(metadata_t);

      // what if previous is null??
      if (tempprevious==NULL && freelist_temp->next != NULL) {
        freelist_temp->next->prev = NULL;
        //freelist_temp = freelist_temp->next;
      }

      //previous = freelist_temp->prev;
      //previous->next = freelist_temp->next;
*/
      return userpointer;
    }

    else {
      freelist_temp = freelist_temp->next;
    }

  }
  
  //return userpointer;
}

void dfree(void* ptr) {
  /* your code here */

  // keep free list in SORTED ORDER by addresses
  // check adjacent blocks to the right and left, to see if they're also free
  // if so, we go into coalescing --> 1. add space of second block and its metadata/header to the first block. 2. unlink second block from freelist since it has now been absorbed by the first block

  metadata_t *traverse = freelist;

  metadata_t *left;
  metadata_t *right;

  metadata_t *coal = freelist;

  metadata_t *l;
  metadata_t *r;

// Find where in the freelist to re-insert the newly freed block.  Update previous and next pointers accordingly.

  while (traverse != NULL) {
    left = traverse->prev;
    right = traverse->next;

    if (left == NULL) {
      if (right != NULL) {
        if ((void*)traverse < ptr && (void*)right > ptr) {
          //left = traverse;
          //right = traverse->next;
          traverse->next = (metadata_t*)ptr;
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = right;
          right->prev = ((metadata_t*)ptr);

          break;
        }
        else if ((void*)traverse > ptr) {
          ((metadata_t*)ptr)->next = traverse;
          traverse->prev = ((metadata_t*)ptr);

          ((metadata_t*)ptr)->prev = NULL;

          break;
        }
        else {
          traverse = traverse->next;
        }

      }
      else {
        //left = traverse;
        //right = NULL
        if ((void*)traverse < ptr) {
          traverse->next = ((metadata_t*)ptr);
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = NULL;

          break;
        }
        else {
          ((metadata_t*)ptr)->next = traverse;
          traverse->prev = ((metadata_t*)ptr);

          ((metadata_t*)ptr)->prev = NULL;

          break;
        }
      }
    }
    else {
      if (right != NULL) {
        if ((void*)traverse < ptr && (void*)right > ptr) {
          //left = traverse;
          //right = traverse->next

          traverse->next = ((metadata_t*)ptr);
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = right;
          right->prev = ((metadata_t*)ptr);

          break;
        }
        else {
          traverse = traverse->next;
        }
      }
      else {
        //left = traverse;
        //right = NULL;
        traverse->next = ((metadata_t*)ptr);
        ((metadata_t*)ptr)->prev = traverse;

        ((metadata_t*)ptr)->next = NULL;

        break;
      }

    }

  }

// Coalescing

  while (coal != NULL) {
    l = coal->prev;
    r = coal->next;
    if (l != NULL) {
      if (r != NULL) {
        if (coal == (void*)l + sizeof(metadata_t) + l->size) {
          l->next = r;
          r->prev = l;
          l->size = l->size + coal->size + sizeof(metadata_t);
        }
      }
      else {
        break;  // we are at the end of the freelist
      }
    }

    coal = coal->next;

  }

}

bool dmalloc_init() {

  /* Two choices: 
   * 1. Append prologue and epilogue blocks to the start and the
   * end of the freelist 
   *
   * 2. Initialize freelist pointers to NULL
   *
   * Note: We provide the code for 2. Using 1 will help you to tackle the 
   * corner cases succinctly.
   */

  size_t max_bytes = ALIGN(MAX_HEAP_SIZE);
  /* returns heap_region, which is initialized to freelist */
  freelist = (metadata_t*) mmap(NULL, max_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  /* Q: Why casting is used? i.e., why (void*)-1? */
  if (freelist == (void *)-1)
    return false;
  freelist->next = NULL;
  freelist->prev = NULL;
  freelist->size = max_bytes-METADATA_T_ALIGNED;
  return true;
}

/* for debugging; can be turned off through -NDEBUG flag*/
void print_freelist() {
  metadata_t *freelist_head = freelist;
  while(freelist_head != NULL) {
    DEBUG("\tFreelist Size:%zd, Head:%p, Prev:%p, Next:%p\t",
    freelist_head->size,
    freelist_head,
    freelist_head->prev,
    freelist_head->next);
    freelist_head = freelist_head->next;
  }
  DEBUG("\n");
}
