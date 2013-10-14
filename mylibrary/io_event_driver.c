//
// I/O Event Driver Framework
//
// All rights reserved, hkim, 2012
//
// Revision History
// - Nov/1/2012, initial release by hkim
// 
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "io_event_driver.h"

typedef struct
{
   struct list_head     next;
   int                  fd;
   io_event_callback    cb;
   void*                priv;
} IODriverElement;

#ifndef MAX
#define MAX(a,b)  a >= b ? a : b
#endif

////////////////////////////////////////////////////////////////////////////////
//
// static utilities
//
////////////////////////////////////////////////////////////////////////////////
static inline void
free_io_event_list(struct list_head* head)
{
   IODriverElement   *p, *n;

   list_for_each_entry_safe(p, n, head, next)
   {
      list_del_init(&p->next);
      free(p);
   }
}

static void
crash(void)
{
   char* ptr = NULL;

   *ptr = 0;
}

static inline IODriverElement*
get_io_event_element(IOEventDriver* driver, IOEventType type, int fd)
{
   IODriverElement*     p;

   list_for_each_entry(p, &driver->io_set[type], next)
   {
      if(p->fd == fd)
      {
         return p;
      }
   }

   list_for_each_entry(p, &driver->working_set[type], next)
   {
      if(p->fd == fd)
      {
         return p;
      }
   }
   return NULL;
}

static inline int
add_to_select_set(fd_set* set, IOEventDriver* driver, IOEventType type)
{
   IODriverElement*     p;
   int                  not_empty = 0;

   list_for_each_entry(p, &driver->io_set[type], next)
   {
      FD_SET(p->fd, set);
      driver->max_fd = MAX(driver->max_fd, p->fd);
      not_empty = 1;
   }
   return not_empty;
}

static inline void
check_select(IOEventDriver* driver, fd_set* set, IOEventType type)
{
   IODriverElement      *p, *n;

   list_for_each_entry_safe(p, n, &driver->io_set[type], next)
   {
      if(FD_ISSET(p->fd, set))
      {
         list_del(&p->next);
         list_add_tail(&p->next, &driver->working_set[type]);
      }
   }

   while(!list_empty(&driver->working_set[type]))
   {
      list_del(&p->next);
      list_add_tail(&p->next, &driver->io_set[type]);
      p->cb(driver, p->fd, type, p->priv);
   }
}

static inline int
diff_time_in_usec(struct timeval* start, struct timeval* now)
{
   int diff;

   diff = (now->tv_sec  - start->tv_sec)  * 1000000 +
          (now->tv_usec - start->tv_usec);

   return diff < 0 ? 0 : diff;
}
////////////////////////////////////////////////////////////////////////////////
//
// public utilities
//
////////////////////////////////////////////////////////////////////////////////
/**
 * initializes IO event driver
 *
 * @param driver IOEventDriver context block
 * @param poll_interval select poll interval in milliseconds
 * @return 0 on success, -1 on fail
 */
int
init_io_event_driver(IOEventDriver* driver, int poll_interval)
{
   driver->poll_interval   = poll_interval;
   driver->max_fd          = 0;

   INIT_LIST_HEAD(&driver->io_set[IO_EVENT_RX]);
   INIT_LIST_HEAD(&driver->io_set[IO_EVENT_TX]);
   INIT_LIST_HEAD(&driver->io_set[IO_EVENT_ERROR]);

   INIT_LIST_HEAD(&driver->working_set[IO_EVENT_RX]);
   INIT_LIST_HEAD(&driver->working_set[IO_EVENT_TX]);
   INIT_LIST_HEAD(&driver->working_set[IO_EVENT_ERROR]);
   return 0;
}

/**
 * deinitializes IO event driver
 *
 * @param driver IOEventDriver context block
 */
void
deinit_io_event_driver(IOEventDriver* driver)
{
   free_io_event_list(&driver->io_set[IO_EVENT_RX]);
   free_io_event_list(&driver->io_set[IO_EVENT_TX]);
   free_io_event_list(&driver->io_set[IO_EVENT_ERROR]);

   free_io_event_list(&driver->working_set[IO_EVENT_RX]);
   free_io_event_list(&driver->working_set[IO_EVENT_TX]);
   free_io_event_list(&driver->working_set[IO_EVENT_ERROR]);
}

/**
 * adds the specified fd to specified IO type set
 *
 * @param driver IOEventDriver context block
 * @param fd target file descriptor
 * @param type IO event type desired
 * @param cb call back function when the target IO type occurs on the fd
 * @param priv call back function parameter
 * @return 0 on success, -1 on fail
 */
int
listen_io_event(IOEventDriver* driver, int fd, IOEventType type, io_event_callback cb, void* priv)
{
   IODriverElement*     element;

   element = get_io_event_element(driver, type, fd);

   if(element != NULL)
   {
      element->cb    = cb;
      element->priv  = priv;
      return 0;
   }

   element = (IODriverElement*)malloc(sizeof(IODriverElement));
   if(element == NULL)
   {
      return -1;
   }

   element->fd    = fd;
   element->cb    = cb;
   element->priv  = priv;

   list_add_tail(&element->next, &driver->io_set[type]);
   return 0;
}

/**
 * removes the specified fd from specified IO type set
 *
 * @param driver IOEventDriver context block
 * @param fd target file descriptor
 * @param type IO event type desired
 * @return 0 on success, -1 on failure
 */
int
unlisten_io_event(IOEventDriver* driver, int fd, IOEventType type)
{
   IODriverElement*     element;

   element = get_io_event_element(driver, type, fd);

   if(element == NULL)
   {
      return -1;
   }

   list_del_init(&element->next);
   free(element);

   return 0;
}

/**
 * drives IO event driver
 *
 * a) set up select timer and fd sets
 * b) enter select
 * c) handle IO events 
 *
 * @param driver IOEventDriver context block
 */
void
drive_io_event(IOEventDriver* driver)
{
   fd_set            rset,
                     tset,
                     eset;
   int               ret,
                     r,
                     w,
                     e,
                     original = driver->poll_interval * 1000,
                     remain  = original;
   struct timeval    to,
                     start,
                     now;

   gettimeofday(&start, NULL);
loop:
   FD_ZERO(&rset);
   FD_ZERO(&tset);
   FD_ZERO(&eset);

   driver->max_fd = 0;

   r = add_to_select_set(&rset, driver, IO_EVENT_RX);
   w = add_to_select_set(&tset, driver, IO_EVENT_TX);
   e = add_to_select_set(&eset, driver, IO_EVENT_ERROR);

   to.tv_sec      = 0;
   //to.tv_usec     = driver->poll_interval * 1000;  // in micro second unit
   to.tv_usec     = remain;

   ret = select(driver->max_fd + 1, r > 0 ? &rset : NULL, w > 0 ? &tset : NULL, e > 0 ? &eset : NULL, &to);

   if(ret == 0)
   {
      return;
   }

   if(ret == -1)
   {
      if(errno != EINTR)
      {
         perror("select:");
         crash();
      }
      return;
   }

   check_select(driver, &rset, IO_EVENT_RX);
   check_select(driver, &tset, IO_EVENT_TX);
   check_select(driver, &eset, IO_EVENT_ERROR);

   gettimeofday(&now, NULL);
   remain -= diff_time_in_usec(&start, &now);

   if(remain > original || remain < 30)
   {
      return;
   }
   goto loop;
}
