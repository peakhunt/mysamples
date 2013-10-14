//
// a very generic timer engine
//
// all rights reserved, hkim, 2012
//
// Revision History
// - Nov/1/2012, initial release by hkim
//
//
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "timer.h"

/**
 * initialize a timer manager
 *
 * @param timer timer manager context block
 * @param tick_rate desired tick rate
 * @param n_buckets number of buckets desired
 * @return 0 on success, -1 on failure
 */
int
init_timer(Timer* timer, int tick_rate, int n_buckets)
{
   int i;

   timer->num_buckets         = n_buckets;
   timer->tick_rate           = tick_rate;
   timer->tick_rate_times_3   = tick_rate * 3;
   timer->tick                =      0;

   timer->buckets = (struct list_head*)malloc(sizeof(struct list_head) * timer->num_buckets);
   if(timer->buckets == NULL)
   {
      return -1;
   }

   for(i = 0; i < timer->num_buckets; i++)
   {
      INIT_LIST_HEAD(&timer->buckets[i]);
   }

   timer->accumulated = timer->tick_rate;
#ifdef __USE_HIGH_RESOLUTION_TIMER
   clock_gettime(CLOCK_MONOTONIC, &timer->prev);
#else
   gettimeofday(&timer->prev, NULL);
#endif
   return 0;
}

/**
 * deinitialize a timer manager
 *
 * @param timer timer context block
 */
void
deinit_timer(Timer* timer)
{
   free(timer->buckets);
}

/**
 * initialize a timer element before using it
 *
 * @param elem timer element to initialize
 */
void
init_timer_elem(TimerElem* elem)
{
   INIT_LIST_HEAD(&elem->next);
}

/**
 * start a stopped timer element by adding it to timer manager
 *
 * @param timer timer manager context block
 * @param elem new timer element to add to timer manager
 * @param expires desired timeout value in milliseconds
 */
void
add_timer(Timer* timer, TimerElem* elem, int expires)
{
   int target;

   if(is_timer_running(elem))
   {
      char* crash = NULL;
      *crash = 0;
   }

   INIT_LIST_HEAD(&elem->next);

   elem->tick     = timer->tick + get_tick_from_milsec(timer, expires);
   target         = elem->tick % timer->num_buckets;

   list_add_tail(&elem->next, &timer->buckets[target]);
}

/**
 * stop a running timer element by deleting it from timer manager
 *
 * @param timer tmier manager context block
 * @param elem timer elemen to delete
 */
void
del_timer(Timer* timer, TimerElem* elem)
{
   if(!is_timer_running(elem))
   {
      return;
   }
   list_del_init(&elem->next);
}

static void
timer_tick(Timer* timer)
{
   int               current;
   TimerElem         *p, *n;
   struct list_head  timeout_list = LIST_HEAD_INIT(timeout_list);

   current = timer->tick % timer->num_buckets;

   //
   // be careful with this code..
   // Here is the logic behind this
   // 1. once timer expires, it should be able to re-add the same timer again
   // 2. when a timer expires, it should be able to remove
   //    other timers including ones timed out inside the timeout handler
   //
   list_for_each_entry_safe(p, n, &timer->buckets[current], next)
   {
      if(p->tick == timer->tick)
      {
         list_del(&p->next);
         list_add_tail(&p->next, &timeout_list);
      }
   }

   timer->tick++;

   while(!list_empty(&timeout_list))
   {
      p = list_first_entry(&timeout_list, TimerElem, next);
      list_del_init(&p->next);
      p->cb(p);
   }
}

/**
 * drive a given timer manager
 * this routine should be called every tick rate as close as possible
 * On non-realtime systems,  some late timeout is just inevitable
 *
 * @param timer timer manager context block
 */
void
drive_timer(Timer* timer)
{
   long              mtime,
                     seconds,
                     mseconds;
#ifdef __USE_HIGH_RESOLUTION_TIMER
   struct timespec   now;

   clock_gettime(CLOCK_MONOTONIC, &now);

   seconds  = now.tv_sec - timer->prev.tv_sec;
   mseconds = (now.tv_nsec - timer->prev.tv_nsec) / 1000000.0;

   timer->prev = now;
#else
   struct timeval    now;

   gettimeofday(&now, NULL);

   seconds  = now.tv_sec   - timer->prev.tv_sec;
   mseconds = (now.tv_usec  - timer->prev.tv_usec) / 1000.0;

   timer->prev = now;
#endif

   mtime = (seconds * 1000 + mseconds);

   // defensive guard against sudden time change
   mtime = mtime < 0 ? 0 : mtime;
   mtime = mtime >= timer->tick_rate_times_3  ? timer->tick_rate : mtime;

   timer->accumulated += mtime;

#ifdef NO_TICK_LOSS_COMPENSATION
   if(timer->accumulated >= timer->tick_rate)
#else
   while(timer->accumulated >= timer->tick_rate)
#endif
   {
      timer->accumulated -= timer->tick_rate;
      timer_tick(timer);
   }
}
