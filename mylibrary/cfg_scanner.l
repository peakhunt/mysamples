/*
//
// configuration file lexical scanner
//
// All rights reserved, hkim, 2012
//
// Revision History
// - Sep/5/2012 initial release by hkim
//
*/

%option yylineno
%option nounput

D     [0-9]
L     [a-zA-Z_]
H     [a-fA-F0-9]
E     [Ee][+-]?{D}+
FS    (f|F|l|L)
IS    (u|U|l|L)*

%{
#include <stdio.h>
#include "cfg_loader.h"
#include "cfg_parser.h"
#include "mem_tracker.h"

void comment(void);

static FILE* token_file;

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
   if ( (result = fread( (char*)buf, sizeof(char), max_size, token_file)) < 0) \
      YY_FATAL_ERROR( "read() in flex scanner failed");

%}

%%

"#"                     { comment();                                                   }
"="                     { return ASSIGNMENT;                                           }
"true"                  { yylval.ival = 1; return(BOOLEAN);                            }
"false"                 { yylval.ival = 0; return(BOOLEAN);                            }
"["                     { return(LIST_OPEN);                                           }
"]"                     { return(LIST_CLOSE);                                          }
","                     { return(COMMA);                                               }
{L}({L}|{D})*           { yylval.sval = strdup(yytext); return(PARAMETER);             }
0[xX]{H}+{IS}?          { yylval.ival = strtoul(yytext, NULL, 0); return(DECIMAL);     }
{D}+{IS}?		         { yylval.ival = atoi(yytext); return(DECIMAL);                 }
{D}*"."{D}+({E})?{FS}?	{ yylval.fval = atof(yytext); return(FLOAT);                   }
\"(\\.|[^\\"])*\"       {  
                           yytext[strlen(yytext)-1] = '\0';
                           yylval.sval = strdup(&yytext[1]);
                           return(STRING_LITERAL);
                        }
[ \t\n]+                { /* ignore white space */                                     }
.                       { /* ignore unknown characters */                              }

%%

int
yywrap()
{
   return 1;
}

void
comment()
{
   char c;

   while((c = input()) != '\n')
      ;
}

void
setInputFile(FILE* fp)
{
   token_file = fp;
}
