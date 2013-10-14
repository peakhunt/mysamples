/*//////////////////////////////////////////////////////////////////////////////////
//
// configuration file parser
//
// All rights reserved, hkim, 2012
//
// Revision History
// - Sep/5/2012 initial release by hkim
//
//////////////////////////////////////////////////////////////////////////////////*/
%{
   #include <stdio.h>
   #include "cfg_loader.h"
   #include "mem_tracker.h"

   extern int yylineno;
   extern int yylex(void);
   extern void yyerror(const char* s);
   static ConfigCB* _cfg;
   static struct list_head _g_list;
%}

/**
 * context value for parser
 */
%union
{
   char        *sval;      /** string value        */
   int         ival;       /** integer value       */
   float       fval;       /** floating type value */
   ParamValue  *pval;      /** list structore      */
}

%token <sval> PARAMETER
%token <ival> DECIMAL
%token <sval> STRING_LITERAL
%token <fval> FLOAT
%token <ival> BOOLEAN
%token ASSIGNMENT
%token LIST_OPEN
%token LIST_CLOSE
%token COMMA

%type  <pval> values
%type  <pval> unit_value
%type  <pval> list

%start cfg_list

%%

cfg_list: 
         cfg_expression 
      |  cfg_list cfg_expression
;
            
cfg_expression: PARAMETER ASSIGNMENT values { addConfigParam(_cfg, $1, $3); }
;

values:
         unit_value           { $$ = $1; }
      |  list                 { $$ = $1; }
;

unit_value:
         STRING_LITERAL       { $$ = allocPValue(Type_String, (void*)&$1);    }
      |  DECIMAL              { $$ = allocPValue(Type_Integer, (void*)&$1);   }
      |  FLOAT                { $$ = allocPValue(Type_Float, (void*)&$1);     }
      |  BOOLEAN              { $$ = allocPValue(Type_Boolean, (void*)&$1);   }
;

list:
         LIST_OPEN            { INIT_LIST_HEAD(&_g_list); }
         value_sequence
         LIST_CLOSE           { $$ = allocPValue(Type_List, &_g_list); INIT_LIST_HEAD(&_g_list); }
;

value_sequence:
         unit_value           { list_add_tail(&$1->list, &_g_list); } 
         COMMA
         value_sequence 
      |  unit_value           { list_add_tail(&$1->list, &_g_list); }
;

%%

void
yyerror(const char* s)
{
#if 1
   extern char* yytext;

   printf("error : %d, %s, %s\n", yylineno, s, yytext);
#endif
}

void
setCurrentConfig(ConfigCB* cfg)
{
   _cfg = cfg;
}
