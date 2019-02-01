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

int iter = 0;

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

  if (freelist_temp == NULL) {
    return NULL;
  }

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
  //iterator
//  iter = iter + 1;
  //printf("successful runs = %d", iter);
  //return userpointer;
}

void dfree(void* ptr) {
  /* your code here */

  // keep free list in SORTED ORDER by addresses
  // check adjacent blocks to the right and left, to see if they're also free
  // if so, we go into coalescing --> 1. add space of second block and its metadata/header to the first block. 2. unlink second block from freelist since it has now been absorbed by the first block

//  printf("starting print free list\n");
//  print_freelist();
//  printf("ending print free list\n");

  metadata_t *traverse = freelist;
  metadata_t *travcopy = freelist;
  //metadata_t *permanenthead = freelist;

  metadata_t *left;
  metadata_t *right;

  metadata_t *coal = freelist;

  metadata_t *l;
  metadata_t *r; 
  metadata_t *tnext;

// Find where in the freelist to re-insert the newly freed block.  Update previous and next pointers accordingly.

  while (travcopy != NULL) {
    left = traverse->prev;
    right = traverse->next;

    if (left == NULL) {
      if (right != NULL) {
        if ((void*)traverse > ptr && (void*)right < ptr) {
          //left = traverse;
          //right = traverse->next;
          //if (traverse->next->next == NULL) {

          //}

          //printf("addy in between \n");

          traverse->next = (metadata_t*)ptr;
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = right;
          right->prev = ((metadata_t*)ptr);

          //freelist = traverse;
          traverse = freelist;

          break;
        }
        else if ((void*)traverse < ptr) {
          //printf("addy BEFORE\n");
          ((metadata_t*)ptr)->next = traverse;
          traverse->prev = ((metadata_t*)ptr);

          ((metadata_t*)ptr)->prev = NULL;

          freelist = ((metadata_t*)ptr);

          traverse = freelist;

          break;
        }
        else {
          travcopy = travcopy->next;
          traverse = traverse->next;
        }

      }
      else { 
        //left = traverse;
        //right = NULL
        //printf("left null right null \n");
        if ((void*)traverse > ptr) {
          traverse->next = ((metadata_t*)ptr);
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = NULL;

          traverse = freelist;

          break;
        }
        else {
          ((metadata_t*)ptr)->next = traverse;
          traverse->prev = ((metadata_t*)ptr);

          ((metadata_t*)ptr)->prev = NULL;

          freelist = ((metadata_t*)ptr);

          //traverse = freelist;
          //((metadata_t*)ptr) = freelist;

          break;
        }
      }
    }


    else {
      if (right != NULL) {
        //printf("left not right not \n");
        if ((void*)traverse > ptr && (void*)right < ptr) {
          //left = traverse;
          //right = traverse->next

          traverse->next = ((metadata_t*)ptr);
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = right;
          right->prev = ((metadata_t*)ptr);

          traverse = freelist;

          break;
        }
        else if ((void*)traverse < ptr) {
          ((metadata_t*)ptr)->next = traverse;
          traverse->prev = ((metadata_t*)ptr);

          ((metadata_t*)ptr)->prev = left;
          left->next = ((metadata_t*)ptr);

          //freelist = ((metadata_t*)ptr);

          traverse = freelist;

          break;
        }
        else {
          travcopy = travcopy->next;
          traverse = traverse->next;
        }
      }
      else {
        //printf("left not right null \n");
        //left = traverse;
        //right = NULL;
        if ((void*)traverse > ptr) {
          traverse->next = ((metadata_t*)ptr);
          ((metadata_t*)ptr)->prev = traverse;

          ((metadata_t*)ptr)->next = NULL;

          traverse = freelist;

          //freelist = coal;

          break;
        }
        else {
          travcopy = travcopy->next;
          traverse = traverse->next;
        }
      }

    } 

    //traverse = freelist;

  }

  //traverse = freelist;
  //coal = freelist;

// Coalescing
/*
  while (coal != NULL) {
    l = coal->prev;
    r = coal->next;
    if (l != NULL) {
      if (r != NULL) {
        if ((void*)coal == (void*)l + sizeof(metadata_t) + l->size) {
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
*/

  /*
  //coal = freelist
  while (coal->next != NULL) {
    if (coal->prev == NULL) { //first free element
      coal=coal->next;
    }
    else { //we are at least at the second block
      // COALESCE BACKWARDS
      tnext = coal->next;
      l = coal->prev;
      if (coal == (((void*)l) + sizeof(metadata_t) + l->size)) {
        l->size = l->size + coal->size + sizeof(metadata_t);
        l->next = coal->next;
        coal->next->prev = l;
      }
      // COALESCE FORWARDS
      r = coal->next;
      if (r == (((void*)l) + sizeof(metadata_t) + l->size)) {
        l->size = l->size + r->size + sizeof(metadata_t);
        l->next = r->next;
        if (r->next != NULL) {
          r->next->prev = l;
        }
        coal = r->next;
      }
      else  {
        coal = tnext;
      }
    }
  }


*/
  int coalesced = 0;

  while(coal != NULL){
    if((((void *) coal + coal->size + METADATA_T_ALIGNED) == coal->next) && (coal->size != 0)){

      coal->size = coal->size + coal->next->size + METADATA_T_ALIGNED;
      coal->next = coal->next->next;
      if (coal->next->size != 0) {
        coal->next->prev = coal;
      }
      coalesced = 1;
    }
 
    if(coalesced == 1) {
      coalesced = 0;
      coal = freelist;
    } else {
      coal = coal->next;
    }
  }

  //coal = freelist;
  //freelist = coal;
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