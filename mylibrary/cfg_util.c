#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cfg_util.h"

#define lock_config()		
#define unlock_config()	

#define CONFIG_LOCKED_OPERATION(lock, stmt)\
   if(lock)\
   {\
      lock_config();\
      (stmt);\
      unlock_config();\
   }\
   else\
   {\
      (stmt);\
   }


/**
 * opens and parses a BIM system.cfg
 * first it tries to open system.cfg from /flash/config
 * if it fails, the function tries to open system.cfg 
 * from /etc
 *
 * @param cfg Config Control Block that will hold system configuration data
 * @param lock 1 to lock before opening, 0 to bypass the lock
 * @return 0 on succes, -1 on failure
 */
int
open_bim_system_cfg(ConfigCB* cfg, int lock)
{
   int ret = 0;

   if(lock)
   {
      lock_config();
   }

   if(parseConfig(cfg, BIM_SYSTEM_CONFIG_ACTIVE) != 0)
   {
      if(parseConfig(cfg, BIM_SYSTEM_CONFIG_BACKUP) != 0)
      {
         ret = -1;
      }
   }

   if(lock)
   {
      unlock_config();
   }
   return ret;
}

/**
 * a generic interface to update BIM configuration file.
 *
 * @param config_path configuration file path
 * @param ..., parameter input. it must be in the form of
 *             <char* param_name, <type> parameter value>
 *             and NULL to signify the end of parameter list
 * @return 0 on success, -1 on failure
 */
int
update_bim_cfg(int lock, char* config_path, ...)
{
   ConfigCB          cfg;
   ConfigParameter*  param;
   int               ret;
   va_list           args;
   char              *pname,
                     *sval;
   int               ival;
   double            fval;

   CONFIG_LOCKED_OPERATION(lock, ret = parseConfig(&cfg, config_path));
   
   if(ret != 0)
   {
      return -1;
   }

   va_start(args, config_path);
   while((pname = va_arg(args, char*)) != NULL)
   {
      param = lookupConfig(&cfg, pname);
      if(param == NULL)
      {
         goto fail;
      }

      switch(param->val->ptype)
      {
      case Type_String:
         sval = va_arg(args, char*);
         free(param->val->sval);
         param->val->sval = strdup(sval);
         break;

      case Type_Float:
         fval = va_arg(args, double);
         param->val->fval = fval;
         break;

      case Type_Integer:
      case Type_Boolean:
         ival = va_arg(args, int);
         param->val->ival = ival;
         break;

      default:
         goto fail;
      }
   }
   va_end(args);

   CONFIG_LOCKED_OPERATION(lock, ret = writeConfig(config_path, &cfg));
   freeConfig(&cfg);
   return ret;

fail:
   va_end(args);
   freeConfig(&cfg);
   return -1;
}

/**
 * a generic interface to read BIM configuration file.
 *
 * @param config_path configuration file path
 * @param ..., parameter input. it must be in the form of
 *             <char* param_name, <type> parameter value>
 *             and NULL to signify the end of parameter list
 * @return 0 on success, -1 on failure
 */
int
read_bim_cfg(int lock, char* config_path, ...)
{
   ConfigCB          cfg;
   ConfigParameter*  param;
   int               ret;
   va_list           args;
   char              *pname,
                     *sval;
   int               *ival;
   double            *fval;

   CONFIG_LOCKED_OPERATION(lock, ret = parseConfig(&cfg, config_path));

   if(ret != 0)
   {
      return -1;
   }

   va_start(args, config_path);
   while((pname = va_arg(args, char*)) != NULL)
   {
      param = lookupConfig(&cfg, pname);
      if(param == NULL)
      {
         goto fail;
      }

      switch(param->val->ptype)
      {
      case Type_String:
         sval = va_arg(args, char*);
         strcpy(sval, param->val->sval);
         break;

      case Type_Float:
         fval = va_arg(args, double*);
         *fval = param->val->fval;
         break;

      case Type_Integer:
      case Type_Boolean:
         ival = va_arg(args, int*);
         *ival = param->val->ival;
         break;

      default:
         goto fail;
      }
   }
   va_end(args);

   freeConfig(&cfg);
   return ret;

fail:
   va_end(args);
   freeConfig(&cfg);
   return -1;
}
