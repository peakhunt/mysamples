//
// a memory tracker to detect memory leak
//
// all rights reserved, hkim, 2012
//
// Revision History
// - Oct/22/2012 initial release by hkim
//
//
#include <stdio.h>
#include "hash.h"
#include <stdlib.h>
#include <string.h>

#define BUCKET_SIZE     1024

/**
 * a tag prepended to each memory allocated from heap
 */
typedef struct
{
   char*       file;       /** file where the memory is allocated          */
   int         line;       /** line number where the memory is allocated   */
   int         size;       /** size of memory allocated                    */
} MemDebugTag;

/**
 * memory allocation info tracked by memory tracker
 */
typedef struct
{
   MemDebugTag    tag;           /** memory tag                   */
   int            count;         /** number of active allocation  */
   int            total_size;    /** total size allocated         */
   HashElement    elem;          /** hash element for hashing     */
} AllocInfo;

/**
 * hash context for memory tracking
 */
static HashContext memHash;

/**
 * initialize internal data structores for memory tracking
 */
void
initMemTracker(void)
{
   initHash(&memHash, BUCKET_SIZE, offsetof(AllocInfo, elem),
         offsetof(AllocInfo, tag), sizeof(MemDebugTag) - sizeof(int), NULL);
}

/**
 * print memory tracking statistics to fp
 *
 * @param fp file pointer to write statistics to
 */
void
printMemStat(FILE* fp)
{
   int i;
   struct list_head* pos;

   fprintf(fp, "=============== MEMORY TRACKING INFO ==============\n");

   for(i = 0; i < BUCKET_SIZE; i++)
   {
      list_for_each(pos, &memHash.buckets[i])
      {
         AllocInfo* info;

         info = (AllocInfo*)((char*)pos - memHash.offset);
         fprintf(fp, "FILE %s, LINE %d, COUNT %d, TOTAL_SIZE %d\n",
               info->tag.file, info->tag.line,
               info->count, info->total_size);
      }
   }
   fflush(fp);
}

static void
checkMalloc(MemDebugTag* tag)
{
   AllocInfo* info;

   info = lookupHash(&memHash, tag);
   if(info == NULL)
   {
      info = malloc(sizeof(AllocInfo));
      if(info == NULL)
      {
         return;
      }

      info->tag = *tag;
      info->count = 0;
      info->total_size  = 0;

      addHash(&memHash, info);
   }

   info->count++;
   info->total_size += tag->size;
}

static void
checkFree(MemDebugTag* tag)
{
   AllocInfo* info;

   info = lookupHash(&memHash, tag);
   if(info != NULL)
   {
      info->count--;
      info->total_size -= tag->size;
      if(info->count == 0)
      {
         delHash(&memHash, tag);
         free(info);
      }
   }
}

/**
 * allocates memory from heap with tag appened before the actual data block
 * updates memory tracking information according to the allocation result
 * 
 * @param size size requested
 * @param file file where this routine is called
 * @param line line at which this routine is called
 * @return memory pointer or NULL in case it fails
 */
void*
debug_malloc(size_t size, char* file, int line)
{
   size_t total;
   total = sizeof(MemDebugTag) + size;
   void* ptr = malloc(total);
   void* data;
   MemDebugTag* tag;

   if(ptr == NULL)
   {
      return NULL;
   }

   memset(ptr, 0, total);

   tag = (MemDebugTag*)ptr;
   tag->file = file;
   tag->line = line;
   tag->size = size;

   checkMalloc(tag);
   data = (char*)ptr + sizeof(MemDebugTag);
   return data;
}

/**
 * allocates memory from heap with tag appened before the actual data block
 * updates memory tracking information according to the allocation result
 * 
 * @param n number of size element
 * @param size size requested
 * @param file file where this routine is called
 * @param line line at which this routine is called
 * @return memory pointer or NULL in case it fails
 */
void*
debug_calloc(int n, size_t size, char* file, int line)
{
   return debug_malloc(n * size, file, line);
}

/**
 * reallocates memory from heap with tag appened before the actual data block
 * updates memory tracking information according to the allocation result
 * 
 * @param ptr pointer to the previous heap memory
 * @param size size requested
 * @param file file where this routine is called
 * @param line line at which this routine is called
 * @return memory pointer or NULL in case it fails
 */
void*
debug_realloc(void* ptr, size_t size, char* file, int line)
{
   size_t total;
   total = sizeof(MemDebugTag) + size;
   void* ptr2;
   void* data;
   MemDebugTag* tag;

   if(ptr != NULL)
   {
      checkFree(ptr - sizeof(MemDebugTag));
      ptr2 = realloc(ptr - sizeof(MemDebugTag), total);
   }
   else
   {
      ptr2 = malloc(total);
   }

   tag = (MemDebugTag*)ptr2;
   tag->file = file;
   tag->line = line;
   tag->size = size;

   checkMalloc(tag);
   data = (char*)ptr2 + sizeof(MemDebugTag);
   return data;
}

/**
 * frees memory allocated from heap
 * updates memory tracking information according to the allocation result
 * 
 * @param ptr pointer to memory allocated
 * @param file file where this routine is called
 * @param line line at which this routine is called
 */
void
debug_free(void* ptr, char* file, int line)
{
   MemDebugTag* tag;

   tag = (MemDebugTag*)((char*)ptr - sizeof(MemDebugTag));
   checkFree(tag);

   free(tag);
}

/**
 * allocates memory for string and copies string
 * updates memory tracking information according to the allocation result
 * 
 * @param s string to copy
 * @param file file where this routine is called
 * @param line line at which this routine is called
 * @return pointer to string or NULL in case it fails
 */
char*
debug_strdup(char* s, char* file, int line)
{
   char* ptr;

   ptr = (char*)debug_malloc(strlen(s) + 1, file, line);
   strcpy(ptr, s);

   return ptr;
}
