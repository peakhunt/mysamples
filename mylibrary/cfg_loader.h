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
#ifndef __CFG_DEF_H__
#define __CFG_DEF_H__

#include <stdio.h>
#include "list.h"

/**
 * configuration parameter type
 */
typedef enum
{
   Type_String,   /** string type, must be "xxx" format */
   Type_Float,    /** floating point type, 123.45 */
   Type_Integer,  /** integer type, 123 */
   Type_Boolean,  /** boolean type, true or false */
   Type_List,     /** list type, [ 123, "456", false ]. a list cannot embed a list inside it */
} ParamType;

/**
 * parameter value structure
 */
typedef struct _paramValue
{
   ParamType            ptype;         /** parameter type */
   struct list_head     list;          /** a list used for linking list values when parsing */
   int                  numParams;     /** number of parameters, for primitive types, it's 1
                                           for list parameter, it's the number of parameters inside
                                           the list */
   /** union of parameter values */
   union
   {
      int                  ival;       /** integer or boolean value  */
      char*                sval;       /** string value              */
      float                fval;       /** floating point value      */
      struct _paramValue** array;      /** list value array          */
   };
} ParamValue;

/**
 * a structore for a configuration parameter
 */
typedef struct
{
   struct list_head  next;             /** a list used for linking config parameter values for a given configuration */
   char              *name;            /** name of a config parameter */
   ParamValue        *val;             /** value of a config parameter */
} ConfigParameter;

/**
 * a structure for a configuration parameters
 */
typedef struct
{
   struct list_head  parm_list;        /** list head for a list of config parameters */
   int               numParams;        /** number of config parameters */
} ConfigCB;

/**
 * a structure for loading configuration files automatically
 */
typedef struct
{
   char*    name;
   int      type;
   int      offset;
} ConfigStructDef;

/**
 * parse a configuration file
 */
extern int parseConfig(ConfigCB* cfg, char* path);
/**
 * free memory used by ConﬁigCB
 */
extern void freeConfig(ConfigCB* cfg);
/**
 * add a new config parameter to configuration control block
 */
extern void addConfigParam(ConfigCB* cfg, char* name, ParamValue* pval);
/**
 * allocate a new parameter value
 */
extern ParamValue* allocPValue(ParamType t, void* val);
/**
 * configuration iterate callback
 */
typedef void (*cfg_iterator)(ConfigCB* cfg, ConfigParameter* param);
/**
 * iterate over a configuration 
 */
extern void iterate_over_cfg(ConfigCB* cfg, cfg_iterator it);
/**
 * iterate over a configuration  for a agiven parameter name
 */
extern void iterate_over_name(ConfigCB* cfg, char* name, cfg_iterator it);
/**
 * look up a config parameter for a given name
 */
extern ConfigParameter* lookupConfig(ConfigCB* cfg, char* name);
/**
 * look up a next config parameter for a give name after current
 */
extern ConfigParameter* lookupNextConfig(ConfigCB* cfg, ConfigParameter* current, char* name);
/**
 * add a new integer config parameter
 */
extern void addIntegerValue(ConfigCB* cfg, char* name, int value);
/**
 * add a new string config parameter
 */
extern void addStringValue(ConfigCB* cfg, char* name, char* str);
/**
 * add a new float config parameter
 */
extern void addFloatValue(ConfigCB* cfg, char* name, float value);
/**
 * add a new boolean config parameter
 */
extern void addBoolValue(ConfigCB* cfg, char* name, int value);
/**
 * write current configuration memory to file
 */
extern int writeConfig(char* path, ConfigCB* cfg);
/**
 * lookup a integer parameter for a given name
 */
extern int lookupInt(ConfigCB* cfg, char* name);
/**
 * lookup a string parameter for a given name
 */
extern char* lookupStr(ConfigCB* cfg, char* name);
/**
 * lookup a float parameter for a given name
 */
extern float lookupFloat(ConfigCB* cfg, char* name);
/**
 * add a new list type parameter
 */
extern void addListValue(ConfigCB* cfg, char* name, ...);
/**
 * add a new parameter
 */
extern void addParameter(ConfigCB* cfg, ConfigParameter* param);
/**
 * delete the parameter 
 */
extern void delParameter(ConfigCB* cfg, ConfigParameter* param);

/**
 * set the parameter 
 */
extern void setParamString(ConfigCB* cfg, char* name, char* value);
extern void setParamInteger(ConfigCB* cfg, char* name, int value);
extern void setParamBool(ConfigCB* cfg, char* name, int value);


extern int load_parameter(ConfigCB* cfg, ConfigStructDef* defs, int num_elem, char* memory);

#endif //!__CFG_DEF_H__
