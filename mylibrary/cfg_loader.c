////////////////////////////////////////////////////////////////////////////////////
//
// configuration file loader
//
// All rights reserved, hkim, 2012
//
// Revision History
// - Sep/5/2012 initial release by hkim
//
////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "cfg_loader.h"
#include "mem_tracker.h"

extern int yyparse(void);
extern void setCurrentConfig(ConfigCB* cfg);
extern void setInputFile(FILE* fp);

#define DEFAULT_TAB_STOP      "\t\t\t\t"

static void
__init_config(ConfigCB* cfg)
{
   INIT_LIST_HEAD(&cfg->parm_list);
   cfg->numParams = 0;
}

static ConfigParameter*
allocConfigParam(void)
{
   ConfigParameter* param;

   param = (ConfigParameter*)malloc(sizeof(ConfigParameter));
   if(param == NULL)
   {
      return NULL;
   }

   INIT_LIST_HEAD(&param->next);

   return param;
}

/**
 * allocates a memory for the parameter type having value
 *
 * @param type parameter type
 * @param value parameter value
 * @return ParamValue pointer or NULL on memory allocation fail
 */
ParamValue*
allocPValue(ParamType type, void* value)
{
   ParamValue* pv;

   pv = (ParamValue*)malloc(sizeof(ParamValue));
   if(pv == NULL)
   {
      return NULL;
   }

   pv->ptype   = type;
   INIT_LIST_HEAD(&pv->list);

   switch(type)
   {
   case Type_List:
      {
         int         count;
         ParamValue  *pos,
                     *n;
         struct list_head* l = (struct list_head*)value;

         // pass 1‥
         count = 0;
         list_for_each_entry(pos, l, list)
         {
            count++;
         }

         pv->array   = (ParamValue**)malloc(sizeof(ParamValue*)*count);
         pv->numParams = count;

         count = 0;

         // pass 2.
         list_for_each_entry_safe(pos, n, l, list)
         {
            list_del(&pos->list);
            pv->array[count++] = pos;
         }
      }
      break;

   case Type_String:
      pv->sval = *((char**)value);
      break;

   case Type_Integer:
      pv->ival = *((int*)value);
      break;

   case Type_Float:
      pv->fval = *((float*)value);
      break;

   case Type_Boolean:
      pv->ival = *((int*)value);
      break;
   }

   return pv;
}

static void
freePValue(ParamValue* pval)
{
   if(pval->ptype == Type_String)
   {
      free(pval->sval);
   }
   else if(pval->ptype == Type_List)
   {
      int i;

      for(i = 0; i < pval->numParams; i++)
      {
         freePValue(pval->array[i]);
      }
      free(pval->array);
   }
   free(pval);
}

static void
freeConfigParam(ConfigParameter* p)
{
   freePValue(p->val);
   free(p->name);
   free(p);
}

/**
 * add a new config parameter to Config Control Block
 * the parameter is added at the end of parameter list
 *
 * @param cfg configuration control block
 * @param param a new configuration parameter to add
 */
void
addParameter(ConfigCB* cfg, ConfigParameter* param)
{
   list_add_tail(&param->next, &cfg->parm_list);
   cfg->numParams++;
}

/**
 * delete a given config parameter from Config Control Block
 * parameter is removed from config parameter list and
 * its memory is deallocated 
 *
 * @param cfg configuration control block
 * @param param configuration parameter to delete
 */
void
delParameter(ConfigCB* cfg, ConfigParameter* param)
{
   list_del(&param->next);
   cfg->numParams--;
   freeConfigParam(param);
}

/**
 * add a new config parameter of name and value
 *
 * @param cfg configuration control block
 * @param name name of a new parameter
 * @param pval value of a new parameter
 */
void
addConfigParam(ConfigCB* cfg, char* name, ParamValue* pval)
{
   ConfigParameter* param;

   param = allocConfigParam();
   if(param == NULL)
   {
      return;
   }

   param->name       = name;
   param->val        = pval;

   addParameter(cfg, param);
}

/**
 * parse a configuration file and loads its data
 * to config control block
 *
 * @param cfg configuration control block
 * @param path configuration file path
 * @return 0 on success, -1 on error
 */
int
parseConfig(ConfigCB* cfg, char* path)
{
   int ret;
   FILE* fp;

   __init_config(cfg);

   setCurrentConfig(cfg);

   fp = fopen(path, "r");
   if(fp == NULL)
   {
      return -1;
   }

   setInputFile(fp);

   ret = yyparse();

   {
      extern int yylex_destroy();

      yylex_destroy();
   }

   fclose(fp);

   if(ret != 0)
   {
      freeConfig(cfg);
      return -1;
   }
   return 0;
}

/**
 * deallcoates memory used by configuration control block
 *
 * @param cfg configuration control block
 */
void
freeConfig(ConfigCB* cfg)
{
   ConfigParameter   *p, *n;

   list_for_each_entry_safe(p, n, &cfg->parm_list, next)
   {
      list_del(&p->next);
      freeConfigParam(p);
   }
}

/**
 * iterate all the configuraion parameters in 
 * the config control block
 *
 * @param cfg config control block
 * @param it iteratation callback
 */
void
iterate_over_cfg(ConfigCB* cfg, cfg_iterator it)
{
   ConfigParameter* p;

   list_for_each_entry(p, &cfg->parm_list, next)
   {
      it(cfg, p);
   }
}

/**
 * iterate the configuraion parameters with given name in 
 * the config control block
 *
 * @param cfg configuration control block
 * @param name name of parameter to iterate to
 * @param it iteratation callback
 */
void
iterate_over_name(ConfigCB* cfg, char* name, cfg_iterator it)
{
   ConfigParameter* p;

   list_for_each_entry(p, &cfg->parm_list, next)
   {
      if(strcmp(p->name, name) == 0)
      {
         it(cfg, p);
      }
   }
}

/**
 * lookup a config parameter with give name
 * if there is multiple parameters with the given name,
 * the first one is returned
 *
 * @param cfg configuration control block
 * @param name name of parameter to look up
 * @return ConfigParameter pointer or NULL on error
 */
ConfigParameter*
lookupConfig(ConfigCB* cfg, char* name)
{
   ConfigParameter* p;

   list_for_each_entry(p, &cfg->parm_list, next)
   {
      if(strcmp(p->name, name) == 0)
      {
         return p;
      }
   }
   return NULL;
}

/**
 * lookup a config parameter with give name
 * after the current config parameter
 *
 * @param cfg configuration control block
 * @param current current configuration parameter
 * @name name of parameter to look up
 * @return ConfigParameter pointer or NULL on error
 */
ConfigParameter*
lookupNextConfig(ConfigCB* cfg, ConfigParameter* current, char* name)
{
   ConfigParameter* p = current;

   list_for_each_entry_continue(p, &cfg->parm_list, next)
   {
      if(strcmp(p->name, name) == 0)
      {
         return p;
      }
   }

   return NULL;
}

/**
 * add a new integer typed parameter
 *
 * @param cfg configuration control block
 * @name name of parameter to add
 * @value interger value of new parameter
 */
void
addIntegerValue(ConfigCB* cfg, char* name, int value)
{
   ConfigParameter* param;

   param = allocConfigParam();
   if(param == NULL)
   {
      return;
   }

   param->name = strdup(name);
   param->val  = allocPValue(Type_Integer, (void*)&value);
   addParameter(cfg, param);
}

/**
 * add a new string typed parameter
 *
 * @param cfg configuration control block
 * @name name of parameter to add
 * @vstr string value of new parameter
 */
void
addStringValue(ConfigCB* cfg, char* name, char* str)
{
   ConfigParameter* param;
   char* dup;

   dup = strdup(str);

   param = allocConfigParam();
   if(param == NULL)
   {
      free(dup);
      return;
   }

   param->name = strdup(name);
   param->val  = allocPValue(Type_String, (void*)&dup);
   addParameter(cfg, param);
}

/**
 * add a new float typed parameter
 *
 * @param cfg configuration control block
 * @name name of parameter to add
 * @value float value of new parameter
 */
void
addFloatValue(ConfigCB* cfg, char* name, float value)
{
   ConfigParameter* param;

   param = allocConfigParam();
   if(param == NULL)
   {
      return;
   }

   param->name = strdup(name);
   param->val  = allocPValue(Type_Float, (void*)&value);
   addParameter(cfg, param);
}

/**
 * add a new bool typed parameter
 *
 * @param cfg configuration control block
 * @name name of parameter to add
 * @value bool value of new parameter
 */
void
addBoolValue(ConfigCB* cfg, char* name, int value)
{
   ConfigParameter* param;

   param = allocConfigParam();
   if(param == NULL)
   {
      return;
   }

   param->name = strdup(name);
   param->val  = allocPValue(Type_Boolean, (void*)&value);
   addParameter(cfg, param);
}

/**
 * add a list typed parameter
 * the variable arguments must be in
 * [int type, type value] sequence or your program will crash
 * and the sequence must end with -1 to signify the end of list
 *
 * @param cfg configuration control block
 * @name name of parameter to add
 */
void
addListValue(ConfigCB* cfg, char* name, ...)
{
   ConfigParameter*     param;
   struct list_head     list;
   va_list              args;
   int                  type;
   int                  ival;
   float                fval;
   char                 *sval;
   ParamValue*          val;

   param = allocConfigParam();
   if(param == NULL)
   {
      return;
   }

   INIT_LIST_HEAD(&list);

   va_start(args, name);
   while((type = va_arg(args, int)) != -1)
   {
      switch(type)
      {
      case Type_String:
         sval = strdup(va_arg(args, char*));
         val = allocPValue(Type_String, (void*)&sval);
         break;

      case Type_Float:
         fval = (float)va_arg(args, double);
         val = allocPValue(Type_Float, (void*)&fval);
         break;

      case Type_Integer:
         ival = va_arg(args, int);
         val = allocPValue(Type_Integer, (void*)&ival);
         break;

      case Type_Boolean:
         ival = va_arg(args, int);
         val = allocPValue(Type_Boolean, (void*)&ival);
         break;
      }
      list_add_tail(&val->list, &list);
   }
   va_end(args);

   param->name = strdup(name);
   param->val  = allocPValue(Type_List, (void*)&list);
   addParameter(cfg, param);
}

static void
writeValue(FILE* fp, ParamValue* val)
{
   switch(val->ptype)
   {
   case Type_String:
      fprintf(fp, "\"%s\"", val->sval);
      break;

   case Type_Integer:
      fprintf(fp, "%d", val->ival);
      break;

   case Type_Float:
      fprintf(fp, "%f", val->fval);
      break;

   case Type_Boolean:
      fprintf(fp, "%s", val->ival == 0 ? "false" : "true");
      break;

   default:
      break;
   }
}

static int
writeParameter(FILE* fp, ConfigParameter* p)
{
   ParamValue* val;

   val = p->val;

   switch(val->ptype)
   {
   case Type_String:
      fprintf(fp, "%-20s= \"%s\"\n", p->name, val->sval);
      break;

   case Type_Integer:
      fprintf(fp, "%-20s= %d\n", p->name, val->ival);
      break;

   case Type_Float:
      fprintf(fp, "%-20s= %f\n", p->name, val->fval);
      break;

   case Type_Boolean:
      fprintf(fp, "%-20s= %s\n", p->name, val->ival == 0 ? "false" : "true");
      break;

   case Type_List:
      fprintf(fp,"%-20s= [ ", p->name);
      {
         int i;

         for(i = 0; i < val->numParams; i++)
         {
            writeValue(fp, val->array[i]);
            if(i < val->numParams - 1)
            {
               fprintf(fp, " , ");
            }
         }
      }
      fprintf(fp," ]\n");
      break;

   default:
      return 1;
   }
   return 0;
}

/**
 * write the contents of configuraion control block to file
 *
 * @param path file path
 * @param cfg configuration control block
 * @return 0 on success, -1 on failure
 */
int
writeConfig(char* path, ConfigCB* cfg)
{
   FILE* fp;
   ConfigParameter* p;
   int i;

   fp = fopen(path, "w");

   //
   // by default, add a few blank comments and blank line
   //
   for(i = 0; i < 3; i++)
   {
      fprintf(fp, "#\n");
   }
   fprintf(fp,"\n");

   list_for_each_entry(p, &cfg->parm_list, next)
   {
      if(writeParameter(fp, p) != 0)
      {
         fclose(fp);
         return -1;
      }
   }

   fsync(fileno(fp));
   fclose(fp);
   return 0;
}

/**
 * look up an interger parameter with given name
 *
 * @param cfg configuration control block
 * @param name name of the parameter
 * @return parameter value or 0 when not found
 */
int
lookupInt(ConfigCB* cfg, char* name)
{
   ConfigParameter* p;

   p = lookupConfig(cfg, name);
   if(p == NULL)
   {
      return 0;   // defualt
   }
   return p->val->ival;
}

/**
 * look up a string parameter with given name
 *
 * @param cfg configuration control block
 * @param name name of the parameter
 * @return parameter value or NULL when not found
 */
char*
lookupStr(ConfigCB* cfg, char* name)
{
   ConfigParameter* p;

   p = lookupConfig(cfg, name);
   if(p == NULL)
   {
      return NULL;   // defualt
   }
   return p->val->sval;
}

/**
 * look up an float parameter with given name
 *
 * @param cfg configuration control block
 * @param name name of the parameter
 * @return parameter value or 0.0f when not found
 */
float
lookupFloat(ConfigCB* cfg, char* name)
{
   ConfigParameter* p;

   p = lookupConfig(cfg, name);
   if(p == NULL)
   {
      return 0.0f;   // defualt
   }
   return p->val->fval;
}

/**
 * set the string value of a parameter identified by name
 * if the parameter doesn't exist, it is added to the configuration
 *
 * @param cfg config control block
 * @param name name of parameter
 * @param value new string value of parameter
 */
void
setParamString(ConfigCB* cfg, char* name, char* value)
{
   ConfigParameter* param;

   param = lookupConfig(cfg, name);

   if(param == NULL)
   {
      addStringValue(cfg, name, value);
   }
   else
   {
      free(param->val->sval);
      param->val->sval = strdup(value);
   }
}

/**
 * set the integer value of a parameter identified by name
 * if the parameter doesn't exist, it is added to the configuration
 *
 * @param cfg config control block
 * @param name name of parameter
 * @param value new integer value of parameter
 */
void
setParamInteger(ConfigCB* cfg, char* name, int value)
{
   ConfigParameter* param;

   param = lookupConfig(cfg, name);

   if(param == NULL)
   {
      addIntegerValue(cfg, name, value);
   }
   else
   {
      param->val->ival = value;
   }
}

/**
 * set the bool value of a parameter identified by name
 * if the parameter doesn't exist, it is added to the configuration
 *
 * @param cfg config control block
 * @param name name of parameter
 * @param value new bool value of parameter
 */
void
setParamBool(ConfigCB* cfg, char* name, int value)
{
   ConfigParameter* param;

   param = lookupConfig(cfg, name);

   if(param == NULL)
   {
      addBoolValue(cfg, name, value);
   }
   else
   {
      param->val->ival = value;
   }
}

/**
 * load a parameters from configuration file to memory
 *
 * @param cfg config control block
 * @param defs parameter definition array
 * @param num_elem number of element in parameter definition array
 * @memory memory array to store parameters to
 * @return 0 on success, -1 on failure
 */
int
load_parameter(ConfigCB* cfg, ConfigStructDef* defs, int num_elem, char* memory)
{
   ConfigParameter*  p;
   int               i;
   char*             ptr;

   for(i = 0; i < num_elem; i++)
   {
      p = lookupConfig(cfg, defs[i].name);
      if(p == NULL)
      {
         return -1;
      }
      else
      {
         ptr = memory + defs[i].offset;

         switch(defs[i].type)
         {
         case Type_Integer:
            *((int*)ptr) = p->val->ival;
            break;

         case Type_String:
            *((char**)ptr) = strdup(p->val->sval);
            break;
         }
      }
   }
   return 0;
}
