//
// a memory tracker to detect memory leak
//
// all rights reserved, hkim, 2012
//
// Revision History
// - Oct/22/2012 initial release by hkim
//
//
#ifndef __MEM_TRACKER_DEF_H__
#define __MEM_TRACKER_DEF_H__

#include <stdlib.h>
#include <stdio.h>

extern void initMemTracker(void);
extern void* debug_malloc(size_t size, char* file, int line);
extern void* debug_calloc(size_t n, size_t size, char* file, int line);
extern void* debug_realloc(void* ptr, size_t, char* file, int line);
extern void debug_free(void* ptr, char* file, int line);
extern void printMemStat(FILE* fp);
extern char* debug_strdup(char* s, char* file, int line);

#if 0
#undef malloc
#undef realloc
#undef calloc
#undef free
#undef strdup

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define calloc(n, size) debug_calloc(n, size, __FILE__, __LINE__)
//#define calloc(s)          debug_malloc(s, __FILE__, __LINE__)
#define realloc(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)
#define strdup(s) debug_strdup(s, __FILE__, __LINE__)
#endif

#endif //!__MEM_TRACKER_DEF_H__
