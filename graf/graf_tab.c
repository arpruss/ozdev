
/*  A Bison parser, made from graf.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	FUN	258
#define	NUM	259
#define	END	260
#define	PI	261
#define	NEG	262

#line 1 "graf.y"

union yystype
{
  double x;
  double (*f)(double);
};

#define YYSTYPE union yystype
#include <oz.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "graf.h"
#include "graf_inc.c"
static char bad_comb[]="Unsupported combination of variables";
static void enter_op(char op);
static void enter_number(double x);
static void enter_func(func f);
static void enter_var(char c);
static void check_graph_type(void);
static byte seth;
static byte setr,setx,sety;
static char last_var;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		53
#define	YYFLAG		-32768
#define	YYNTBASE	24

#define YYTRANSLATE(x) ((unsigned)(x) <= 262 ? yytranslate[x] : 27)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    22,
    23,    11,     9,     7,    10,     2,    12,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     8,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    14,     2,     2,     2,     2,     2,    15,
    20,     2,     2,    19,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    18,     2,    21,     2,     2,     2,    17,
    16,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,    13
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     8,    11,    16,    20,    24,    28,    32,    34,
    37,    39,    41,    45,    49,    53,    57,    60,    62,    64,
    66,    68,    72,    76
};

static const short yyrhs[] = {    26,
     5,     0,    26,     7,    26,     5,     0,    25,     5,     0,
    25,     7,    25,     5,     0,    16,     8,    26,     0,    17,
     8,    26,     0,    18,     8,    26,     0,    19,     8,    26,
     0,     4,     0,     4,    15,     0,    20,     0,     6,     0,
    26,     9,    26,     0,    26,    10,    26,     0,    26,    11,
    26,     0,    26,    12,    26,     0,    10,    26,     0,    17,
     0,    16,     0,    19,     0,    21,     0,    26,    14,    26,
     0,    22,    26,    23,     0,     3,    26,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    40,    41,    42,    43,    45,    46,    47,    48,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
    62,    63,    64,    65
};

static const char * const yytname[] = {   "$","error","$undefined.","FUN","NUM",
"END","PI","','","'='","'+'","'-'","'*'","'/'","NEG","'^'","'d'","'y'","'x'",
"'r'","'h'","'e'","'t'","'('","')'","line","assg","expr",""
};
#endif

static const short yyr1[] = {     0,
    24,    24,    24,    24,    25,    25,    25,    25,    26,    26,
    26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
    26,    26,    26,    26
};

static const short yyr2[] = {     0,
     2,     4,     2,     4,     3,     3,     3,     3,     1,     2,
     1,     1,     3,     3,     3,     3,     2,     1,     1,     1,
     1,     3,     3,     2
};

static const short yydefact[] = {     0,
     0,     9,    12,     0,    19,    18,     0,    20,    11,    21,
     0,     0,     0,    19,    18,    20,    24,    10,    17,     0,
     0,     0,     0,     0,     3,     0,     1,     0,     0,     0,
     0,     0,     0,     5,     6,     7,     8,    23,     0,     0,
     0,     0,     0,    13,    14,    15,    16,    22,     4,     2,
     0,     0,     0
};

static const short yydefgoto[] = {    51,
    12,    13
};

static const short yypact[] = {    30,
    50,   -14,-32768,    50,    -6,     8,    29,    35,-32768,-32768,
    50,    37,    68,-32768,-32768,-32768,    -3,-32768,    -3,    50,
    50,    50,    50,    -5,-32768,     7,-32768,    50,    50,    50,
    50,    50,    50,    74,    74,    74,    74,-32768,    -6,     8,
    35,    40,     3,    27,    27,    -3,    -3,    -3,-32768,-32768,
    55,    57,-32768
};

static const short yypgoto[] = {-32768,
     9,    -1
};


#define	YYLAST		88


static const short yytable[] = {    17,
    18,    20,    19,    29,    30,    31,    32,    50,    33,    24,
    33,    29,    30,    31,    32,    21,    33,    38,    34,    35,
    36,    37,    39,    40,     7,    41,    43,    44,    45,    46,
    47,    48,     1,     2,    42,     3,    22,    31,    32,     4,
    33,    25,    23,    26,    49,     5,     6,     7,     8,     9,
    10,    11,     1,     2,    52,     3,    53,     0,     0,     4,
     0,     0,     0,     0,     0,    14,    15,     0,    16,     9,
    10,    11,    27,     0,    28,     0,    29,    30,    31,    32,
     0,    33,    29,    30,    31,    32,     0,    33
};

static const short yycheck[] = {     1,
    15,     8,     4,     9,    10,    11,    12,     5,    14,    11,
    14,     9,    10,    11,    12,     8,    14,    23,    20,    21,
    22,    23,    16,    17,    18,    19,    28,    29,    30,    31,
    32,    33,     3,     4,    26,     6,     8,    11,    12,    10,
    14,     5,     8,     7,     5,    16,    17,    18,    19,    20,
    21,    22,     3,     4,     0,     6,     0,    -1,    -1,    10,
    -1,    -1,    -1,    -1,    -1,    16,    17,    -1,    19,    20,
    21,    22,     5,    -1,     7,    -1,     9,    10,    11,    12,
    -1,    14,     9,    10,    11,    12,    -1,    14
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || defined(OZ7XX)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#elif defined(OZ7XX)
#include <string.h>
#define __yy_memcpy memcpy
#else
        /* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  static int yystate;
  static int yyn;
  register short *yyssp;
  static YYSTYPE *yyvsp;
  static int yyerrstatus;      /*  number of tokens to shift before error messages enabled */
  static int yychar1;              /*  lookahead token as an internal (translated) token number */

  static short yyssa[YYINITDEPTH];     /*  the state stack                     */
  static YYSTYPE yyvsa[YYINITDEPTH];   /*  the semantic value stack            */

  static short *yyss = yyssa;          /*  refer to the stacks thru separate pointers */
  static YYSTYPE *yyvs = yyvsa;        /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  static YYLTYPE yylsa[YYINITDEPTH];   /*  the location stack                  */
  static YYLTYPE *yyls;
  static YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  static int yystacksize;

#ifdef YYPURE
  static int yychar;
  static YYSTYPE yylval;
  static int yynerrs;
#ifdef YYLSP_NEEDED
  static YYLTYPE yylloc;
#endif
#endif

  static YYSTYPE yyval;                /*  the variable used to return         */
				/*  semantic values from the action	*/
				/*  routines				*/

  static int yylen;

  yystacksize = YYINITDEPTH;
  yychar1=0;
#ifdef YYLSP_NEEDED
  yyls = yylsa;
#endif

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 40 "graf.y"
{ if(last_var=='h') enter_op('R'); else if(last_var=='y') enter_op('X'); else enter_op('Y'); check_graph_type(); ;
    break;}
case 2:
#line 41 "graf.y"
{ enter_op('Y'); enter_op('X'); check_graph_type(); ;
    break;}
case 3:
#line 42 "graf.y"
{ check_graph_type(); ;
    break;}
case 4:
#line 43 "graf.y"
{ check_graph_type(); ;
    break;}
case 5:
#line 45 "graf.y"
{enter_op('Y');;
    break;}
case 6:
#line 46 "graf.y"
{enter_op('X');;
    break;}
case 7:
#line 47 "graf.y"
{enter_op('R');;
    break;}
case 8:
#line 48 "graf.y"
{enter_op('H');;
    break;}
case 9:
#line 50 "graf.y"
{ enter_number(yyval.x); ;
    break;}
case 10:
#line 51 "graf.y"
{ enter_number(yyvsp[-1].x * FROM_DEG); ;
    break;}
case 11:
#line 52 "graf.y"
{ enter_number(2.718281828459); ;
    break;}
case 12:
#line 53 "graf.y"
{ enter_number(M_PI); ;
    break;}
case 13:
#line 54 "graf.y"
{ enter_op('+');    ;
    break;}
case 14:
#line 55 "graf.y"
{ enter_op('-');    ;
    break;}
case 15:
#line 56 "graf.y"
{ enter_op('*');    ;
    break;}
case 16:
#line 57 "graf.y"
{ enter_op('/');    ;
    break;}
case 17:
#line 58 "graf.y"
{ enter_op('n');    ;
    break;}
case 18:
#line 59 "graf.y"
{ enter_var('x');    ;
    break;}
case 19:
#line 60 "graf.y"
{ enter_var('y');   ;
    break;}
case 20:
#line 61 "graf.y"
{ enter_var('h');   ;
    break;}
case 21:
#line 62 "graf.y"
{ enter_var('t');   ;
    break;}
case 22:
#line 63 "graf.y"
{ enter_op('^');    ;
    break;}
case 23:
#line 64 "graf.y"
{ ;         ;
    break;}
case 24:
#line 65 "graf.y"
{ enter_func(yyvsp[-1].f); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 67 "graf.y"

static char *ptr;
static char end;
static byte err;
static byte graphtype;

int yylex(void)
{
  static char c;
  static byte i;
  register char *p=ptr;
  if(end) return 0;
  /* skip white space */
  while ((c=*p++)==' ');
  ptr=p;
  /* process numbers   */
  if (c == '.' || isdigit (c))
    {
      yylval.x=atof(p-1);
      while((c=*p) && (isdigit(c) || c=='.' || c=='e' ||
	(p>ptr && p[-1]=='e' && (c=='-' || c=='+')))) p++;
      ptr=p;
      return NUM;
    }
  for(i=0;i<FUNCTION_TABLE_SIZE;i++)
  {
    if(!strncmp(function_table[i].name,p-1,function_table[i].length))
    {
       yylval.f=function_table[i].f;
       ptr=p+function_table[i].length-1;
       return FUN;
    }
  }
  if(!strncmp(p-1,"pi",2))
  {
     ptr=p+2-1;
     return PI;
  }
  ptr=p;
  if (c == 0)
  {
    end=1;
    return END;
  }
  return c;
}

yyerror(s)
char *s;
{
  if(err) return; /* give only one error message at a time */
  ozsavescreen();
  err=1;
  ozwarn(s,anykey);
  ozrestorescreen();
}

static void enter_number(double x)
{
   compiled[num_steps++]='#';
   values[num_values++]=x;
}

static void enter_op(char op)
{
   if(op!='x')
   {
     if(num_steps>1 && compiled[num_steps-1]=='#' && compiled[num_steps-2]=='#')
     {
	num_steps--;
	num_values--;
	switch(op)
	{
	   case '+':
		values[num_values-1]+=values[num_values]; return;
	   case '-':
		values[num_values-1]-=values[num_values]; return;
	   case '*':
		values[num_values-1]*=values[num_values]; return;
	   case '/':
		values[num_values-1]/=values[num_values]; return;
	   case '^':
                values[num_values-1]=mypow(values[num_values-1],values[num_values]);
		return;
	}
	num_steps++;
	num_values++;
     }
     if(op=='n' && num_steps>0 && compiled[num_steps-1]=='#')
     {
	   values[num_values-1]=-values[num_values-1];
	   return;
     }
   }
   switch(op)
   {
     case 'X': setx=1; if(seth || setr) yyerror(bad_comb);
               break;
     case 'Y': sety=1; if(seth || setr) yyerror(bad_comb);
               break;
     case 'H': seth=1; if(setx || sety) yyerror(bad_comb);
               break;
     case 'R': setr=1; if(setx || sety) yyerror(bad_comb);
               break;
   }
   compiled[num_steps++]=op;
}

static void enter_var(char c)
{
   if(last_var==0) last_var=c;
   else if(last_var!=c)
   {
      yyerror(bad_comb);
      return;
   }
   enter_op('x');
}

static void enter_func(func f)
{
  if(num_steps>0 && compiled[num_steps-1]=='#')
  {
    values[num_values-1]=f(values[num_values-1]);
  }
  else
  {
    compiled[num_steps++]='f';
    functions[num_functions++]=f;
  }
}

static double var,top,beg,delta;
static double x_scale;
static double y_scale;
static double screen_x,screen_y;



void eval(double x)
{
   static byte sp, step, fun, val;
   setx=sety=setr=seth=sp=step=fun=val=0;
   __fperr=0;
   for(;step<num_steps && !__fperr;step++)
   {
      switch(compiled[step])
      {
	 case '#': stack[sp++]=values[val++]; break;
         case '+': --sp; stack[sp-1]+=stack[sp]; break;
         case '-': --sp; stack[sp-1]-=stack[sp]; break;
         case '*': --sp; stack[sp-1]*=stack[sp]; break;
	 case '/': --sp;
                stack[sp-1]/=stack[sp];
		break;
         case '^': --sp; stack[sp-1]=mypow(stack[sp-1],stack[sp]); break;
	 case 'n': stack[sp-1]=-stack[sp-1]; break;
	 case 'x': stack[sp++]=x; break;
	 case 'f': stack[sp-1]=(*(functions[fun++]))(stack[sp-1]); break;
         case 'X': value_x=stack[--sp]; break;
         case 'Y': value_y=stack[--sp]; break;
         case 'H': value_h=stack[--sp]; break;
         case 'R': value_r=stack[--sp]; break;
      }
      if(__fperr) return;
   }
   switch(graphtype)
   {
      case XOFY:
        value_y=x; break;
      case YOFX:
        value_x=x; break;
      case ROFH:
        value_h=x;
      case RHOFT:
        value_x=value_r*cos(value_h);
        value_y=value_r*sin(value_h);
   }
   screen_x=_frndint((value_x-x_min)*x_scale);
   screen_y=_frndint(79.-(value_y-y_min)*y_scale);
}

static void check_graph_type(void)
{
  if(setx && !sety && (last_var=='y' || !last_var)) graphtype=XOFY;
   else if(sety && !setx && (last_var=='x' || !last_var)) graphtype=YOFX;
    else if(setx && sety && (last_var=='t' || !last_var)) graphtype=XYOFT;
     else if(setr && !seth && (last_var=='h' || !last_var)) graphtype=ROFH;
      else if(setr && seth && (last_var=='t' || !last_var)) graphtype=RHOFT;
       else yyerror(bad_comb);
}

byte compile(char *s)
{
  ptr=s;
  last_var=__fperr=end=err=0;
  setx=sety=setr=seth=0;
  num_steps=num_values=num_functions=0;
  yyparse();
  if(__fperr)
     yyerror("Arithmetic error");
  return err;
}

byte setupgraph(char *s)
{
  if(compile(s)) return 1;
  x_scale=238./(x_max-x_min);
  y_scale=79./(y_max-y_min);
  switch(graphtype)
  {
     case YOFX:
        var=x_min;
        top=x_max;
        delta=(x_max-x_min)/238.;
        break;
     case XOFY:
        var=y_min;
        top=y_max;
        delta=(y_max-y_min)/79.;
        break;
     case XYOFT:
     case RHOFT:
        var=t_min;
        top=t_max;
        delta=t_delta;
        break;
     case ROFH:
        var=h_min;
        top=h_max;
        delta=h_delta;
        break;
  }
  beg=var;
  return 0;
}

byte graph(char *s)
{
  static byte x0,x1;
  static byte y0,y1;
#define INVALID_Y 127
  static unsigned k;
  if(setupgraph(s)) return 1;
  y0=INVALID_Y;
  interrupted=0;
  for(;var<=top;var+=delta)
  {
     eval(var);
     if(__fperr
      || screen_y<0 || screen_y>=80
      || screen_x<0 || screen_x>=239)
     {
       y0=INVALID_Y;
     }
     else
     {
       x1=(int)screen_x;
       y1=(int)screen_y;
       if(y0!=INVALID_Y && (abs(y1-y0)>1 || abs(x1-x0)>1))
       {
          _ozline(x0,y0,x1,y1,1);
       }
       else
          _ozpoint(x1,y1,1);
       x0=x1;
       y0=y1;
     }
     if(ozkeyhit())
     {
	k=ozgetch();
	if(k==KEY_BACKLIGHT) oztogglelight();
	else
	if(k=='s') togglesound();
	else
	{
	   interrupted=1;
	   ozungetch(k);
	   getoption();
	   return 0;
	}
     }
  }
  return 0;
}

unsigned flash(int _x,int _y)
{
  static byte state;
  static unsigned k;
  static int x,y;
  x=_x;
  y=_y;
  state=1;
  do
  {
     ozpoint(x-2,y,state);
     ozpoint(x-1,y,state);
     ozpoint(x+1,y,state);
     ozpoint(x+2,y,state);
     ozpoint(x,y-1,state);
     ozpoint(x,y-2,state);
     ozpoint(x,y+1,state);
     ozpoint(x,y+2,state);
     state=!state;
  }
  while(!(k=ozkeydelay64hz(16)));
  return k;
}

void trace(char *s)
{
  static int x0,y0;
  static unsigned k;
  static byte is2nd;
  static char c;
  setupgraph(s);
  is2nd=0;
  while(1)
  {
     eval(var);
     if(screen_y<-10 || screen_y > 90 || screen_x < -10 || screen_x > 250)
     {
        x0=-10;
        y0=-10;
     }
     else
     {
        x0=(int)screen_x;
        y0=(int)screen_y;
     }
     ozsavescreen();
     if(__fperr)
     {
        c=0;
        switch(graphtype)
        {
          case RHOFT:
          case XYOFT:
                c='t';
                break;
          case XOFY:
                c='y';
                break;
          case YOFX:
                c='x';
                break;
       }
       if(c) sprintf(miscstring,"error at %c=%.4g",c,var);
        else /* ROFH */
          sprintf(miscstring,"error at \xE9=%.4g=%.4g\xF8",var,var*TO_DEG);
     }
     else
     {
        switch(graphtype)
        {
           case RHOFT:
                sprintf(miscstring,"(t=%.4g,\xE9=%.4g=%.4g\xF8,r=%.4g)",var,value_h,value_h*TO_DEG,value_r);
                break;
           case ROFH:
                sprintf(miscstring,"(\xE9=%.4g=%.4g\xF8,r=%.4g)",var,value_h*TO_DEG,value_r);
                break;
           case XOFY:
           case YOFX:
                sprintf(miscstring,"(x=%.4g,y=%.4g)",value_x,value_y);
                break;
           case XYOFT:
                sprintf(miscstring,"t=%.4g,x=%.4g,y=%.4g",var,value_x,value_y);
                break;
         }
     }
     ozputs(0,0,miscstring);
     if(__fperr) k=ozgetch();
     else
	k=flash(x0,y0);
     ozrestorescreen();
     switch(k)
     {
	case KEY_MYPROGRAMS:
	case KEY_MAIN:
	case KEY_SCHEDULE:
	case KEY_TELEPHONE:
	case KEY_MEMO:
		ozexitto(k);
        case 'i':
                ozsnap();
                return;
        case 'z':
                zoom(value_x,value_y,.5);
                redraw();
                return;
        case 'u':
                zoom(value_x,value_y,2.);
                redraw();
                return;
	case 's':
		togglesound();
		break;
	case KEY_BACKLIGHT:
		oztogglelight();
		break;
	case KEY_LEFT:
                       if(var>beg) var-=delta;
		       break;
	case KEY_RIGHT:
                       if(var<top) var+=delta;
		       break;
	case KEY_PAGEDOWN:
                       var+=40*delta;
                       if(var>top) var=top;
		       break;
	case KEY_PAGEUP:
                       var-=40*delta;
                       if(var<beg) var=beg;
		       break;
        case KEY_LEFT|MASK2ND: var=beg; break;
        case KEY_RIGHT|MASK2ND: var=top; break;
	case KEY_LOWER_ENTER:
	case KEY_UPPER_ENTER:
	case KEY_RETURN:
	case KEY_LOWER_ESC:
	case KEY_UPPER_ESC:
		return;
	default:
		ozungetch(k);
		getoption();
		if(do_special>=0) return;
     }
  }
}

