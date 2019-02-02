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

// initialize prologue and epilogue blocks - better for edge cases
static metadata_t* prologue = NULL;
static metadata_t* epilogue = NULL;
static void searchforcoalesce();

void* dmalloc(size_t numbytes) {
  /* initialize through mmap call first time */
  if(freelist == NULL) {      
    if(!dmalloc_init())
      return NULL;
  }

  assert(numbytes > 0);
  numbytes = ALIGN(numbytes);

  /* your code here */

  metadata_t* temp = prologue;  //temp starts at the prologue block

  while ((temp != NULL) && (temp->size < numbytes)) {
    temp = temp->next;
  }

  if (temp == NULL) {
    return NULL; 
  }

  metadata_t* previous = temp->prev;
  metadata_t* nextblock = temp->next;

  if(temp->size > (numbytes + sizeof(metadata_t))) {

    metadata_t* splitblock = (metadata_t*)((void*)temp + numbytes + sizeof(metadata_t));

    splitblock->next = nextblock;
    nextblock->prev = splitblock;
    splitblock->prev = previous;
    previous->next = splitblock;

    splitblock->size = temp->size - numbytes - sizeof(metadata_t);
    temp->size = numbytes;
  }
  else {    //the block is exactly the size of numbytes; fits perfectly, so update size and take block out of freelist

    temp->size = numbytes;
    nextblock->prev = previous;
    previous->next = nextblock;
  }

//  void *userpointer;

//  userpointer = (void*)temp + sizeof(metadata_t);

//  return userpointer;
  return ((void*) temp) + sizeof(metadata_t);
}

 
void dfree(void* ptr) {
  /* your code here */

  int finished = 0;
  
  metadata_t* temp = prologue;
  metadata_t* free = (void*)ptr - sizeof(metadata_t);

  if (free == NULL) {
    return;
  }

  while ((temp->next != NULL) || (finished == 0)) {
    temp = temp->next;
    if((temp->prev < free) && (temp > free)) {
      free->prev = temp->prev;
      free->next = temp;
      temp->prev->next = free;
      temp->prev = free;

      finished = 1;

      print_freelist();
    }
    else if((temp->prev->size == 0) && (temp > free)) {  //free is the first element after the prologue
      free->next = temp;
      free->prev = temp->prev;
      temp->prev->next = free;
      temp->prev = free;

      finished = 1;

      print_freelist();

    }
    else if ((temp < free) && (temp->next->size == 0)) { 
      free->next = temp->next;
      free->prev = temp;
      temp->next->prev = free;
      temp->next = free;

      finished = 1;
    }
  }
  searchforcoalesce();
}

void searchforcoalesce() {
  printf("into coal\n");
  metadata_t *temp = prologue; //reset temp to prologue to carry out coalescing
  int finished = 0;

  while (temp != NULL) {
    if((temp->next == ((void*)temp + temp->size + sizeof(metadata_t))) && (temp->size != 0)) {
      temp->next = temp->next->next;
      temp->size = temp->size + temp->next->size + sizeof(metadata_t);
    
      if(temp->next->size != 0) { //not at end of list
        temp->next->prev = temp;
      }
    finished = 1;
    }
    if(finished == 1) {
      //start at beginning to look for more coalescing
      finished = 0;
      temp = prologue;
    }
    else {
      temp = temp->next;
    }
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
  prologue = (metadata_t*) mmap(NULL, (max_bytes+sizeof(metadata_t)+sizeof(metadata_t)), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  /* Q: Why casting is used? i.e., why (void*)-1? */
  if (prologue == (void *)-1)
    return false;

  freelist = prologue+1;
  freelist->size = max_bytes - sizeof(metadata_t);

  prologue->prev = NULL;
  prologue->next = freelist;

  epilogue = (void*)freelist + sizeof(metadata_t) + max_bytes;
  epilogue->prev = freelist;

  freelist->next = epilogue;
  freelist->prev = prologue;

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