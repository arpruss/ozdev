/*zcc v1.05  15/10/02 */

#ifndef BOOL
#define BOOL    unsigned char
#define FALSE   0
#define TRUE    1
#endif

#define MAXFILENAMELEN   10
#define SMALLBUFSIZE     256
#define MEDBUFSIZE       512
#define BIGBUFSIZE       1024
#define ARGSBUFSIZE      1024*5

/* Default Args to modules */
#define CPPARGS   "-DCPM -DHI_TECH_C -Dz80 -DOZ7XX"
#define LINKARGS  "-X"
#define ZASARGS   "--X --N"
#define ZASJARG  "--j"

/* Paths and verbose names from htc\ */

#define CPPEXE         "bin\\dcpp.exe"
#define CPPVERBOSE     "dcpp"
#define ZXCCEXE        "BIN\\ZXCC.EXE"
#define ZXCCVERBOSE    "zxcc"
#define IRRELEXE       "bin\\irrel.exe"
#define IRRELVERBOSE   "irrel"
#define ZXLINKEXE      "bin\\zxlink.exe"
#define ZXLINKVERBOSE  "zxlink"
#define HLINKEXE       "bin\\hlink.exe"
#define HLIN1EXE       "bin\\hlin1.exe"
#define HLINKVERBOSE   "hlink"
#define CHECKSYMEXE    "bin\\checksym.exe"
#define MINOPT         "bin\\miniopt.exe"
#define MINOPT0        "bin\\miniopt0.exe"
#define MINOPT1        "bin\\miniopt1.exe"
#define MINOPT2        "bin\\miniopt2.exe"
#define MAKEWZDEXE     "bin\\makewzd.exe"
#define FIXALLOCA      "bin\\falloca.exe"
#define MINIOPTF       "bin\\minioptf.exe"

#define LIBPATH        "compiler\\"
#define INCLUDEPATH    "compiler\\"


/* lib files to link in as standard. Linker requires double inclusion. Leave a space at the end of the string */
   
#define STANDARDLIBS   "libc0.lib" , "liboz.lib" , "libc0.lib" , "liboz.lib" , NULL
#define FLOATLIBS      "libf.lib" , "libf.lib" ,  NULL
   
/* Used when checking .map file for model */

#define PSECT48KONLY   "__Lbeforebss"


/* 16K model */

#define LINK16K          1
#define OBJ16KDEFAULT           "compiler\\16k.obj"
#define LINKARGS16K      "-X -C8000h -plowpage=8000h,midpage,text,data,bss"
#define CHECKSYM16K      "__Hlowpage le a000 __Hbss le c000 __Lbeforebss ge c000" 


/* 32K Model *  (checksym on _Lbeforebss is not required as this is picked up by link model check. Any additional
                 checksym args will need to go on new line as this is near the max length arg that can be spawned. */

#define LINK32K          2
#define OBJ32KDEFAULT           "compiler\\32k.obj"
#define LINKARGS32K      "-X -C8000h -plowpage=8000h,midpage,text,data,bss"
#define CHECKSYM32K_1    "__Hlowpage le a000 __Lhighpage ge c000 __Hmidpage le c000"
#define CHECKSYM32K_2		 " __Hhighpage le f200 __Hbss le f980"

/*  48K Model */

#define LINK48K          3
#define OBJ48KDEFAULT           "compiler\\32k.obj"
#define LINKARGS48K_1    "-X -C8000h -plowpage=8000h,midpage,text,data,beforebss,bss"
#define CHECKSYM48K_1_1  "__Hlowpage le a000 __Hmidpage le c000 __Hbss le f980"
#define CHECKSYM48K_1_2  "__Lbeforebss ge c000"
#define LINKARGS48K_2    "-X -C8000h -plowpage=8000h,midpage,text,data,beforebss=0c000h,bss"   
#define CHECKSYM48K_2    "__Hlowpage le a000 __Hmidpage le c000 __Hbss le f980 __Lbeforebss ge c000"

/*  Auto select link model */

#define LINKAUTO         4

/* Temp filenames (used by tmpFile()) */
#define TMPFILE1     "$TMP1$.$$$"
#define TMPFILE2     "$TMP2$.$$$"
#define NULLFILE     "$NULL$.$$$"
#define ARGSFILE     "$EXEC$.$$$"

/* tmpFile() actions */
#define TRANSPOSE      1
#define IN          2
#define OUT         3

/*File types (taken from file extention)*/
#define UNKNOWNFILE   0
#define CFILE         1
#define ASFILE        2
#define OBJFILE       3
#define LIBFILE       4
#define PARENTDIR     5     /* .. */

/*Profiler bits*/
#define PROFILEROBJ    "compiler\\profiler.obj"
#define PROFILERDEF    "-DOZPROFILER"

/* Any other useful constants */

#define CTR_C_RETURN 255
#define CHECKMODELFAIL -111 




