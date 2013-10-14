//
// primitive circular buffer
//
// all rights reserved, hkim, 2012
//
// Revision History
//
// - Oct/31/2012, initial release by hkim
//
#ifndef __CIRC_BUFFER_DEF_H__
#define __CIRC_BUFFER_DEF_H__

#ifndef TRUE
#define TRUE         1
#endif

#ifndef FALSE
#define FALSE        0
#endif

/**
 * circular buffer structure
 */
typedef struct
{
   char*       buffer;        /** buffer pointer to store data    */
   int         size;          /** size of buffer                  */
   int         data_size;     /** size of data in buffer          */
   int         begin;         /** buffer begin index              */
   int         end;           /** buffer end index                */
} CircBuffer;

extern int init_circ_buffer(CircBuffer* cb, int size);
extern void deinit_circ_buffer(CircBuffer* cb);
extern int put_circ_buffer(CircBuffer* cb, char* buf, int size);
extern int get_circ_buffer(CircBuffer* cb, char* buf, int size);
extern int peek_circ_buffer(CircBuffer* cb, char* buf, int size);
extern int get_circ_buffer_no_copy(CircBuffer* cb, int size);

/*
 * reset circular buffer
 *
 * @param cb   circular buffer
 */
static inline void 
reset_circ_buffer(CircBuffer* cb)
{
   cb->begin      = 0;
   cb->end        = 0;
   cb->data_size  = 0;
}

/*
 * get data size in circular buffer
 *
 * @param cb   circular buffer
 * @return data size
 */
static inline int
get_circ_buffer_data_size(CircBuffer* cb)
{
   return cb->data_size;
}

/*
 * get buffer size of circular buffer
 *
 * @param cb   circular buffer
 * @return buffer size
 */
static inline int
get_circ_buffer_size(CircBuffer* cb)
{
   return cb->size;
}

/*
 * check if circular buffer is full
 *
 * @param cb   circular buffer
 * @return TRUE when full, FALSE otherwise
 */
static inline int
is_circ_buffer_full(CircBuffer* cb)
{
   if(cb->data_size == cb->size)
   {
      return TRUE;
   }
   return FALSE;
}

/*
 * check if circular buffer is empty
 *
 * @param cb   circular buffer
 * @return TRUE when empty, FALSE otherwise
 */
static inline int
is_circ_buffer_empty(CircBuffer* cb)
{
   if(cb->data_size == 0)
   {
      return TRUE;
   }
   return FALSE;
}

#endif //!__CIRC_BUFFER_DEF_H__
