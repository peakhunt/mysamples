/*******************************************************************************

   log.c : simple log library, initial release by hkim

   revision hsitory
   - June/14/2012 - initial release
*******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "log.h"
#include "list.h"

//
// just a bunch of dummy globals. not a big deal - hk 
//
static int     logLevel = MESS_DEBUG;
static FILE    *errorFile = NULL;
static FILE    *messageFile = NULL;
static int     flags = 0;

/**
 * set a new log leven
 *
 * @param level log level
 */
void
logSetLevel(int level)
{
   logLevel = level;
}

/**
 * sets file for error logs
 *
 * @param f file pointer for error logs
 */
void
logSetErrorFile(FILE * f)
{
   errorFile = f;
}

/**
 * gets file pointer for error logs
 *
 * @return file pointer
 */
FILE*
logGetErrorFile(void)
{
   if(errorFile == NULL)
   {
      return stdout;
   }
   return errorFile;
}

/**
 * sets file pointer for normal logs
 *
 * @param f file pointer for normal logs
 */
void
logSetMessageFile(FILE * f)
{
   messageFile = f;
}

/**
 * gets file pointer for normal logs
 *
 * @return file pointer for normal logs
 */
FILE*
logGetMessageFile(void)
{
   return messageFile;
}

/**
 * turns on log flags
 *
 * @param newFlags log flags to turn on
 */
void
logSetFlags(int newFlags)
{
   flags |= newFlags;
}

/**
 * turns off log flags
 *
 * @param newFlags log flags to turn off
 */
void
logClearFlags(int newFlags)
{
   flags &= ~newFlags;
}

/**
 * logs a log message with level and format
 *
 * a) for MESS_REALDEBUG
 *        MESS_DEBUG
 *        MESS_NORMAL
 *        MESS_VERBOSE, it logs the message to normal log file
 *
 * b) for everything else, it logs the message to error log file
 *
 * c) if LOG_TIMES flag is set, it prepends time before the message
 * d) for MESS_CRASH level, it crashes the currently logging process
 * e) for MESS_FATAL level, it exits the currently logging process with exit value 1
 *
 * @param level log leven desired
 * @param format message format
 * @param ... parameters
 */
void
logMessage(int level, char *format, ...)
{
   va_list args;
   FILE *where = NULL;

   if(errorFile == NULL)
   {
      errorFile = stderr;
   }

   if(messageFile == NULL)
   {
      messageFile = stderr;
   }

   where = errorFile;

   if (level >= logLevel)
   {
      switch (level)
      {
      case MESS_REALDEBUG:
      case MESS_DEBUG:
         where = messageFile;
         break;

      case MESS_NORMAL:
      case MESS_VERBOSE:
         where = messageFile;
         break;

      default:
         where = errorFile;
         break;
      }
      
      if ((flags & LOG_TIMES))
      {
         fprintf(where, "%ld:", (long) time(NULL));
      }
      
      va_start(args, format);
      vfprintf(where, format, args);
      va_end(args);

      fflush(where);

      if(level == MESS_CRASH)
      {
         // intentional crash to get coredump
         char* a = NULL;
         *a = 0;
      }

      if(level == MESS_FATAL)
      {
         exit(1);
      }
   }
}
