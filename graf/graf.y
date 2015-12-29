%{
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
%}
/* BISON Declarations */
%token FUN
%token NUM
%token END
%token PI
%left ','
%left '='
%left '+' '-'
%left '*' '/'
%left NEG     /* negation */
%right FUN    /* function call */
%right '^'
%right 'd'
/* grammar */
%%
line: expr END { if(last_var=='h') enter_op('R'); else if(last_var=='y') enter_op('X'); else enter_op('Y'); check_graph_type(); }
      | expr ',' expr END { enter_op('Y'); enter_op('X'); check_graph_type(); }
      | assg END { check_graph_type(); }
      | assg ',' assg END { check_graph_type(); }
;
assg:      'y' '=' expr     {enter_op('Y');}
        |  'x' '=' expr     {enter_op('X');}
        |  'r' '=' expr     {enter_op('R');}
        |  'h' '=' expr     {enter_op('H');}
;
expr:     NUM                 { enter_number($$.x); }
        | NUM 'd'             { enter_number($1.x * FROM_DEG); }
        | 'e'                 { enter_number(2.718281828459); }
        | PI                  { enter_number(M_PI); }
        | expr '+' expr       { enter_op('+');    }
        | expr '-' expr       { enter_op('-');    }
        | expr '*' expr       { enter_op('*');    }
        | expr '/' expr       { enter_op('/');    }
        | '-' expr  %prec NEG { enter_op('n');    }
        | 'x'                 { enter_var('x');    }
        | 'y'                 { enter_var('y');   }
        | 'h'                 { enter_var('h');   }
        | 't'                 { enter_var('t');   }
        | expr '^' expr       { enter_op('^');    }
        | '(' expr ')'        { ;         }
        | FUN expr  %prec FUN { enter_func($1.f); }
;
%%
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

