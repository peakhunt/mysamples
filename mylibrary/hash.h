//
// a simple generic bucket hash implementation
//
// all rights reserved, hkim, 2012
//
// Revision History
// - Oct/22/2012 initial release by hkim
//
//
#ifndef __HASH_DEF_H__
#define __HASH_DEF_H__

#include <stdio.h>
#include "list.h"

/**
 * a structure for hash element
 */
typedef struct hash_element
{
   struct list_head  lh;         /** a list for buck hash list management */
} HashElement;

/**
 * common hash prototype 
 */
typedef unsigned int (*hash_func)(unsigned char* key, int key_size);

/**
 * a hash context
 */
typedef struct hash_context
{
   int               numBuckets;       /** number of buckets                  */
   int               offset;           /** offset of hash element             */
   int               key_offset;       /** hash key offset                    */
   int               key_size;         /** hash key size                      */
   hash_func         calc_hash;        /** hash function to use               */
   struct list_head* buckets;          /** bucket list                        */
} HashContext;

extern void initHash(HashContext* hash, int numBuckets, int hash_offset,
      int key_offset, int key_size, hash_func func);
extern void deinitHash(HashContext* hash);
extern int addHash(HashContext* hash, void* element);
extern void* lookupHash(HashContext* hash, void* key);
extern int delHash(HashContext* hash, void* key);

#endif //!__HASH_DEF_H__
