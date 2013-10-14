//
// I/O Event Driver Framework
//
// All rights reserved, hkim, 2012
//
// Revision History
// - Nov/1/2012, initial release by hkim
// 
#ifndef __IO_EVENT_DRIVER_DEF_H__
#define __IO_EVENT_DRIVER_DEF_H__

#include "list.h"
#include "timer.h"

/**
 * event type enumeration for IO Event Driver
 */
typedef enum
{
   IO_EVENT_RX = 0,     /** Receive                   */
   IO_EVENT_TX,         /** Transmit                  */
   IO_EVENT_ERROR,      /** Error or Exceptional case */
} IOEventType;

/**
 * IO Event Driver Control Block
 */
typedef struct
{
   int               poll_interval;    /** poll interval for select call         */
   int               max_fd;           /** maximum fd value calculated per loop  */
   struct list_head  io_set[3];        /** registered IO events for RX/TX/Error  */
   struct list_head  working_set[3];
} IOEventDriver;

/**
 * a callback by IO Event Driver
 */
typedef void (*io_event_callback)(IOEventDriver* driver, int fd, IOEventType type, void* priv);

extern int init_io_event_driver(IOEventDriver* driver, int poll_interval);
extern void deinit_io_event_driver(IOEventDriver* driver);
extern int listen_io_event(IOEventDriver* driver, int fd, IOEventType type, io_event_callback cb, void* priv);
extern int unlisten_io_event(IOEventDriver* driver, int fd, IOEventType type);
extern void drive_io_event(IOEventDriver* driver);

#endif //!__IO_EVENT_DRIVER_DEF_H__
