//
// a very generic timer engine
//
// all rights reserved, hkim, 2012
//
// Revision History
// - Nov/1/2012, initial release by hkim
//
//
#ifndef __TIMER_DEF_H__
#define __TIMER_DEF_H__

#include <sys/time.h>
#include "list.h"

//#define __USE_HIGH_RESOLUTION_TIMER

struct _timer_elem;

/**
 * timer callback function
 */
typedef void (*timer_cb)(struct _timer_elem*);

/**
 * timer element representing one timer
 */
typedef struct _timer_elem
{
   struct list_head  next;       /** a list head for next timer element in the bucket  */
   timer_cb          cb;         /** timeout callback                                  */
   unsigned int      tick;       /** absolute timeout tick count                       */
   void*             priv;       /** private argument for timeout callback             */
} TimerElem;

/**
 * a context block for timer manager
 */
typedef struct _timer
{
   int                  num_buckets;         /** number of buckets for timer management         */
   int                  tick_rate;           /** tick rate 100 means a tick per 0.1 sec         */
   int                  tick_rate_times_3;   /** pre calculated tick_rate * 3                   */
   unsigned int         tick;                /** current tick                                   */
   struct list_head*    buckets;             /** bucket array                                   */
#ifdef __USE_HIGH_RESOLUTION_TIMER
   struct timespec      prev;
#else
   struct timeval       prev;                /** previous tick time                             */
#endif
   long                 accumulated;         /** time accumulated so far after previous timck   */
} Timer;

extern int init_timer(Timer* timer, int tick_rate, int n_buckets);
extern void deinit_timer(Timer* timer);
extern void init_timer_elem(TimerElem* elem);
extern void add_timer(Timer* timer, TimerElem* elem, int expires);
extern void del_timer(Timer* timer, TimerElem* elem);
extern void drive_timer(Timer* timer);

/**
 * check if a given timer element is currently running
 *
 * @param elem timer element to check with
 * @return 0 if timer elemnt is not running, 1 if running
 */
static inline int
is_timer_running(TimerElem* elem)
{
   if(elem->next.next == &elem->next && elem->next.prev == &elem->next)
   {
      return 0;
   }
   return 1;
}

/**
 * calculate timer tick count from given milli second
 *
 * @param timer timer manager context block
 * @param milsec desired millisecond
 * @return the closes tick count for a given tick rate
 */
static inline unsigned int
get_tick_from_milsec(Timer* timer, int milsec)
{
   unsigned int   tick,
                  mod;

   tick = milsec / timer->tick_rate;
   mod  = milsec % timer->tick_rate;

   if(mod != 0 && milsec >= timer->tick_rate)
   {
      tick += 1;
   }
   return tick;
}

#endif //!__TIMER_DEF_H__
