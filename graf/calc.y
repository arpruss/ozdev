/* Infix notation calculator--calc */
%{
#define YYSTYPE double
#include <oz.h>
#include <alloca.h>
#include <ctype.h>
#include <math.h>
#include <ctype.h>
%}
/* BISON Declarations */
%token NUM
%token END
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation        */
/* Grammar follows */
%%
input:    /* empty string */
        | input line
;
line:     END
        | exp END  { output($1); }
;
exp:      NUM                { $$ = $1;         }
        | exp '+' exp        { $$ = $1 + $3;    }
        | exp '-' exp        { $$ = $1 - $3;    }
        | exp '*' exp        { $$ = $1 * $3;    }
        | exp '/' exp        { $$ = $1 / $3;    }
        | '-' exp  %prec NEG { $$ = -$2;        }
        | exp '^' exp        { $$ = pow ($1, $3); }
        | '(' exp ')'        { $$ = $2;         }
;
%%
/* Lexical analyzer returns a double floating point
   number on the stack and the token NUM, or the ASCII
   character read if not a number.  Skips all blanks
   and tabs, returns 0 for END. */

static char inline[120];
static char *ptr;
static char end;

yylex ()
{
  static char c;
  register char *p=ptr;
  if(end) return 0;
  /* skip white space  */
  while ((c=*p++)== ' ') ;
  /* process numbers   */
  if (c == '.' || isdigit (c))
    {
      sscanf(p-1,"%lf", &yylval);
      while((c=*p) && (isdigit(c) || c=='.' || c=='e' ||
        (p>inline && p[-1]=='e' && (c=='-' || c=='+')))) p++;
      ptr=p;
      return NUM;
    }
  ptr=p;
  if (c == 0)
  {
    end=1;
    return END;
  }
  /* return single chars */
  return c;                                
}

yyerror(s)
char *s;
{
  ozputs(0,20,s);
}

output(x)
double x;
{
  static char s[20];
  sprintf(s,"%g",x);
  ozputs(0,20,s);
  ozgetch();
}

main()
{
  ozeditline(0,10,inline,120,239);
  ptr=inline;
  yyparse();
}
