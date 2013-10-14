/*******************************************************************************

   log.h : simple log library, initial release by hkim

   revision hsitory
   - June/14/2012 - initial release
*******************************************************************************/

#ifndef __LOG_DEF_H__
#define __LOG_DEF_H__

/**
 * predefined log levels
 */
#define MESS_REALDEBUG	   1        /** realtime debug level   */
#define MESS_DEBUG	      2        /** debug level            */
#define MESS_VERBOSE	      3        /** verbose level          */
#define MESS_NORMAL	      4        /** normal level           */
#define MESS_ERROR	      5        /** error level            */
#define MESS_FATAL	      6        /** fatal error level      */
#define MESS_CRASH         7        /** application crash      */
#define MESS_NO_LOG        8        /** disable all the logs   */

#define LOG_TIMES	(1 << 0)

extern void logMessage(int level, char *format, ...);

extern void logSetErrorFile(FILE * f);
extern void logSetMessageFile(FILE * f);
extern void logSetFlags(int flags);
extern void logClearFlags(int flags);
extern void logSetLevel(int level);
extern FILE* logGetErrorFile(void);
extern FILE* logGetMessageFile(void);

#endif //!__LOG_DEF_H__
