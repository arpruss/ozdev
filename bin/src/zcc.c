/*zcc v1.05  15/10/02 */

/* Compiler shell for HTC ozlib compiler */

#if !__PACIFIC__
#error This program must be compiled using the Hi-Tech Pacific DOS Compiler
#endif

#if !LARGE_MODEL
#error This program must be compiled with the LARGE memory model
#endif

#include <sys.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stat.h>
#include "zcc.h"
#include <unixio.h>
#include <time.h>


/* Global Flags Taken from Command Line */
BOOL handle_alloca = FALSE;
BOOL compile =       FALSE;
BOOL irrel   =       TRUE;
BOOL optim   =       FALSE; 
BOOL ccopt   =       FALSE; 
BOOL link    =       FALSE;
BOOL linkLibFiles =  FALSE;
BOOL linkLibfFiles = FALSE;

BOOL wizard  =       FALSE;
BOOL newer   =       FALSE;
BOOL exitonerror =   TRUE;
BOOL keepasofile =   FALSE;
BOOL verbose =       1;
BOOL usejonas =      FALSE;   /* - use j flag when assembling .as files */
BOOL linklibf =      FALSE;
BOOL startdl  =      FALSE;
BOOL profile =       FALSE;
int  linkModel = LINKAUTO;


/* arg and file lists to be passed to programs */
char cppArgs[SMALLBUFSIZE] = CPPARGS;
char p1Args[SMALLBUFSIZE] = "";
char cgenArgs[SMALLBUFSIZE] = "";
char linkArgs[SMALLBUFSIZE] = "";
char linkFiles[BIGBUFSIZE] = "";
char linkLibs[BIGBUFSIZE] = "";
char outfilename[MAXFILENAMELEN + 1] = "";
char wizardid[SMALLBUFSIZE] = "";
char wizardinfofile[SMALLBUFSIZE] = "";
char wizarddescfile[SMALLBUFSIZE] = "";
char wizardiconfile[SMALLBUFSIZE] = "";
char obj16k[SMALLBUFSIZE];
char obj32k[SMALLBUFSIZE];
char obj48k[SMALLBUFSIZE];
char argsBuf[ARGSBUFSIZE];


/* ZAS and ZXLINK appear to like to have the PATH set correctly, so
   the following will hold enviromental PATH setting for use with spawnle */

char * envpath[2] = {"" , NULL};
char binpath[MEDBUFSIZE];

/* hold program name with leading \ and .exe stripped */

char progName[SMALLBUFSIZE];

/* hold start time of program */

time_t startTime;

/* HTC provides _fullpath when compiled with large model 
   argv[0] does not provide full path in HTC */
   
extern char *   _fullpath;

/* _getargs declarations */  

char ** _getargs(char * buf, char * name);
extern int _argc_;


/* proto the functions */
void Usage(void);
char ** readArgs(int argc , char *argv[]);
void addFileToArgs (FILE * fp);
void addToArgs (char * s);
void doArgs(int argc,char *argv[]);
void doSwitchArg(char * s);
void _doSwitchArg(char * s);
int  doFileArg(char * s);
void buildLinkLibFilesStr (void);
void addHtcLibFile (char * libname , ...);
void setModelObj (void);
int  fileType (char * filename);
int  zxcc (char * comfile , char * args , char * infile , char * outfile);
char * tmpFile (int action);
char * tmpcat (char * arg , ...);
void printCopyright(void);
char * htcPath (char * addPath);
char * fileName(char * fullname);
void removeTmpFiles(void);    
int  excpp(char * infile);
int  exirrel (void);
int  exp1 (void);
int  excgen(void);
int  exoptim (void);
int  exccopt (void);
int  exzas (char * infile , char * outfile);
int  exlink (void);
int  _exlink (void);
int  linker(char * args , char * filename , char * objmodel , char * objfiles , char * libfiles);
int  checksym(char * file , char * args);
int  checkModel (char * file);
int  exalloca (void);
int  exminiopt (void);
int  exminioptf (void);
int  _exminiopt (char * miniopt , char * args);
void addLibFile (char * libname);
int  exmakewzd(void);
long fileModTime (char * filename);
void deleteFile (char * fname);
void (togglestderr) (void);
void zexit (char * msg , int exitcode);

/*macro to control verbosity*/


void Usage(void) {
    printf( "Usage :  %s [flags..] *.c.. *.as[m].. *.obj..  *.lib..\n" , progName);
    printf( "-a              Support alloca().\n"
	        "-c              Compile to *.obj file only.\n"
            "-dDEFINES       Define DEFINES.\n"
            "-f              Continue compiling files after error (Linking will be aborted).\n"
            "-iDIRPATH       Add path to additional include files directory.\n"
            "-j              Convert JP to JR operands on assembly files.\n"
	        "-k              Disable removal of unused prototypes during compilation.\n"
            "-l[f][16|32|48] Link files. Use 'f' to link in floating point lib. Model size\n"
            "                is auto-selected to 32k or 48k unless specified by 16|32|48.\n" 
            "-n              Only compile files newer than the .obj if a .obj exists.\n"
            "-o[0|1|2|3|x]   Optimize [1-compiler (default) , 2-miniopt , 3-both , x-full]\n"
            "-oFILENAME      Use FILENAME for .bin and .wzd (default is 1st .c or .as file).\n"
            "-p              Support profiling. Also adds #define OZPROFILER\n" 
	        "-q              Quiet mode (only copyright and errors displayed).\n"
            "-s              Keep assembly output in *.aso files.\n"
            "-uDEFINES       Undefine DEFINES.\n"
            "-v              Verbose mode.\n"
            "-w[0...9]       Compiler warning level (0 - all to 10 - none).\n"          
            "-z[f][16|32|48] Link as for -l and make *.wzd file.\n"
            "-ziFILENAME / -zdFILENAME / -zzFILENAME\n"
            "                Info / description / icon file name (see makewzd.txt).\n"
            "-zo[65535]      Owner ID for wizard file.\n"
            "-zs             Start the downloader after making Wizard file.");
}


void printCopyright(void) {
    printf("HI-TECH C COMPILER (CP/M-80) V3.09 \n" 
           "Copyright (C) 1984-87 HI-TECH SOFTWARE\n");
}

    
/* reads in all the args from command line and @files, concatenating them for passing to _getargs(); */

char ** readArgs(int argc , char *argv[]) {
    int c;
    FILE * fp;
    if(argc == 1) {
        /* is there anything redirected to stdin ? . There must be a better way to check. 
           else , is there an @args file*/
        fseek(stdin , 0L , 2);
        if (ftell(stdin)) {
            fseek(stdin , 0L , 0);
	        addFileToArgs(stdin);
        } else {
            printf("Usage:  %s [flags...] files.c...  (%s -h for more help)" , progName , progName);
            exit (1);
        }
    } else {
        for (c = 1 ; c < argc ; c++) {
            if (argv[c][0] != '@') {
	            addToArgs(&argv[c][0]);
	        } else {
	            if(!(fp = fopen(&argv[c][1], "r"))) {
                    printf("Args file %s does not exit" , &argv[c][1]);
                    exit (1);
                }
	            addFileToArgs(fp);
		        fclose(fp);
		    }
		    addToArgs(" ");
		}
	}
	argv  = _getargs(argsBuf,argv[0]);
    argc = _argc_;
	doArgs(argc , argv);
	return (argv);
}


void addFileToArgs (FILE * fp) {
    int contp = -1 , c , lineEnd;
    char buf[BIGBUFSIZE];
    while (fgets(buf , sizeof(buf) , fp)) {
        lineEnd  = FALSE;
        for (c=0 ; c < BIGBUFSIZE ; c++) {    
	        switch (buf[c]) {
	            case '\\':
                    contp = c;
			        break;
		    	case '\r':
			    case '\n':
			        if (contp > -1 ) {
				        buf[contp] = '\0';
				    } else {
					    buf[c] = ' ';
					    buf[++c] = '\0';
				    }
				    lineEnd = TRUE;
				    break;
			    case '\0':
				    lineEnd = TRUE;
			 	case '\t':
				case ' ':
				 	break;
				default:
					contp = -1;
			}
			if (lineEnd) {
			    addToArgs(buf);
				break;
			}
		}
	}
}
		  


void addToArgs (char * s) {
    static int bufSize;
    
    if ((bufSize += strlen(s)) > (ARGSBUFSIZE-1)) {
        printf("Args list too long");
	    exit(1);
	}
	strcat(argsBuf , s);
}
	
        


void doArgs(int argc,char *argv[]) {
    int i = 1 , c;
    char * s;
    char envopts[BIGBUFSIZE];

	if (s = getenv("ZCCOPTS")) {
	    while (s = strchr(s , '-')) {
	        for(c = 0 ; *s != ' ' && *s != '\0' ; s++ , c++) envopts[c] = *s;
		    envopts[c] = '\0';
		    doSwitchArg(envopts);
		}    
	}	    
	while (*argv[i] == '-' && i < argc) {
	    doSwitchArg(argv[i]);
	    argv[i++] = NULL;  /* finished with the switch, so NULL it out to leave files only */
	}
	while (i < argc) { 
	    if (doFileArg(argv[i])) argv[i] = NULL;
		i++;
	}
}
    

/* wrapper round _doSwitchArg to handle multi-flag switches */

void doSwitchArg (char * s) {
    char * f = " \0";
    char * sc;
    
    sc = ++s;
    while (*s && strchr("ACFJKNPQSV" , toupper(*s))) s++;
    if (*s) 
	    _doSwitchArg(sc);
    else 
        while ((*f = *--s) != '-') _doSwitchArg(f);
}


void _doSwitchArg (char * s) {
    char * sc;
    sc = s;
    switch (toupper(*s)) { 
        case '?':
        case 'H':
            Usage();
            exit(0);
            break;
        case 'J':
            usejonas = TRUE;
            break;
        case 'P':
	        if (profile == FALSE) {
                strcat(linkFiles , htcPath(PROFILEROBJ));
	            strcat(linkFiles , " ");
		    	strcat(cppArgs , tmpcat(" " , PROFILERDEF , NULL));
			    profile = TRUE;
			}
			break;	        
	    case 'K':
            irrel = FALSE;
            break;
        case 'A':
            handle_alloca = TRUE;
	        break;
        case 'Z':
            wizard = TRUE;
            link = TRUE;
            compile = TRUE;
	        linkLibFiles = TRUE;
            s++;
            if (toupper(*s) == 'O') {
                s++;
                strcpy(wizardid , s);
                break;
            }
            else if (toupper(*s) == 'S') {
                startdl = TRUE;
                break;
            }
            else if (toupper(*s) == 'I') {
                s++;
                if (fileModTime(s) == -1) {
                    printf ("Wizard info file %s not found\n" , s);
                    exit(1);
                }
                strcpy(wizardinfofile , s);
                break;
            }
            else if (toupper(*s) == 'D') {
                s++;
                if (fileModTime(s) == -1) {
                    printf ("Wizard description file %s not found\n" , s);
                    exit(1);
                }
                strcpy(wizarddescfile , s);
                break;
            }
            else if (toupper(*s) == 'Z') {
                s++;
                if (fileModTime(s) == -1) {
                    printf ("Zifnab icon file %s not found\n" , s);
                    exit(1);
                }
                strcpy(wizardiconfile , s);
                break;
            }
	        s--;
        case 'L':     
            link = TRUE;
            compile = TRUE;
	        linkLibFiles = TRUE;
            s++;
            if (toupper(*s) == 'F') {
                linkLibfFiles = TRUE;
                s++;
            }
            if (!(*s))
                linkModel = LINKAUTO;
            else if (!strcmp(s , "16")) 
                linkModel = LINK16K;
            else if (!strcmp(s , "32"))
                linkModel = LINK32K;
            else if (!strcmp(s , "48"))
                linkModel = LINK48K;
            else {
                printf("Invalid options in -%s flag" , sc);
                exit(1);
            }
            break; 
        case 'C':
            compile = TRUE;
            break;
        case 'D':
        case 'U':
        case 'I':
            *s = toupper(*s);
            strcat(cppArgs , tmpcat(" -" , s , NULL));
            break;
        case 'O':
            s++;
	        if (*s == '0' && strlen(s) ==1) {
		        optim = FALSE;
			    ccopt = FALSE;
			}
            else if (*s  == '\0' || (*s == '1' && strlen(s) == 1)) {
	            optim = TRUE;
		        ccopt = FALSE;
			}
            else if (*s == '2' && strlen(s) == 1) {
	            optim = FALSE;
		        ccopt = TRUE;
			}
            else if ((*s == '3' || *s == 'x' || *s == 'X') && strlen(s) == 1) {
                optim = TRUE;
                ccopt = TRUE;
            } else {
                if (sc = strchr(s , '.'))
                    *sc = '\0';
                strncpy (outfilename , s , MAXFILENAMELEN); 
            }
            break;  
        case 'F':
            exitonerror = FALSE;
            break;
        case 'V':
            verbose = 2;    
            break;
        case 'W':
            sprintf(p1Args , "--w%s " , s + 1);
            strcpy (cgenArgs , p1Args);
            break;
        case 'N':
            newer = TRUE;
            break;
        case 'S':
           keepasofile = TRUE;
           break;
	    case 'Q':
	        verbose = 0;
		    break;
        default:
            printf("Invalid flag -%s\n",s);
            exit(1);
    }
}


int doFileArg (char * s) {
    int filetype;
    struct stat sb;
    
    /* check that each arg is a file with a .c or .as or .asm extension.
    Nullify any directory entries.
    .. fails stat but is valid, so we have to check separatly.
     */
    
    filetype = fileType(s);
    if (stat(s , &sb)) {
        if (filetype == PARENTDIR) {
            return (1);
        }
        else {
            printf("%s - file not found" , s);
            exit(1);
        }
    }       
    if (sb.st_mode & S_IFDIR) {   /* is file a directory entry from wildcard expansion */
        return (1);
    }
    switch (filetype) {
        case UNKNOWNFILE:
            printf("%s is not a valid file type - must be .c , .as or .asm" , s);
            exit(1);
        case OBJFILE:
            strcat(linkFiles , tmpcat(s , " " , NULL));
	        return(1); 
        case CFILE:
        case ASFILE:
            strcat(linkFiles , tmpcat(fileName(s) , ".obj" , " " , NULL));
            if ((*outfilename == '\0') && (link == TRUE))
                strcpy(outfilename , fileName(s));
            break;
        case LIBFILE: 
            strcat(linkLibs , tmpcat(s , " " , NULL));
            return(1);
    }
    return (0);
}




void buildLinkLibFilesStr (void) {
	char * s;
	
	/* double up the users included lib files */
	strcat(linkLibs , tmpcat(linkLibs , NULL));
	
	/* use libs in ZCCLIBS or normal libs*/ 
	if (s = getenv("ZCCLIBS")) {
	    strcat(linkLibs , s);
	    strcat (linkLibs , " ");   
    } else {
        if (linkLibfFiles == TRUE)
	        addHtcLibFile(FLOATLIBS);
		if (linkLibFiles == TRUE) 
		    addHtcLibFile(STANDARDLIBS);
    }
}

/* add htc library libnames in arg list to linkLibs. Arg list must end in null */ 

void addHtcLibFile (char * libname , ...) {
    char ** lbn;
    lbn = &libname;
    while (*lbn != NULL) {
        strcat(linkLibs , tmpcat(htcPath(LIBPATH) , *lbn , " " , NULL));
        *lbn++;
    }
}

void setModelObj (void) {
    strcpy (obj16k , getenv("ZCC16KOBJ") ? getenv("ZCC16KOBJ") : htcPath(OBJ16KDEFAULT));
    strcpy (obj32k , getenv("ZCC32KOBJ") ? getenv("ZCC32KOBJ") : htcPath(OBJ32KDEFAULT));
    strcpy (obj48k , getenv("ZCC48KOBJ") ? getenv("ZCC48KOBJ") : htcPath(OBJ48KDEFAULT));
}

void main(int argc , char ** argv)
{
    int i;
    int e;
    int compNOk;
    int filetype;
    char * fname;
    char * cp;
    int exitcode = 0;

    startTime = time(NULL);
    
    /* strip out progName from argv[0] for use with usage messages. */
    
    strcpy (progName  , argv[0] + 1);
    *strrchr(progName , '.') = '\0';

    /* setup binpath for use with spawnle */

    sprintf(binpath , "PATH=%s" , _fullpath);
    *(strrchr(binpath , '\\') + 1)  = '\0';
    envpath[0] = binpath;
    
    argv = readArgs(argc , argv);
    argc = _argc_;
    
    buildLinkLibFilesStr();
    
    /* Check that we have actually asked the compiler to do something */
    
    if ((compile == FALSE) && (keepasofile == FALSE)) {
        printf ("Error - compiler action has not been specified, use -c, -s , -l or -z flags");
        exit(1);
    }

    /* All args OK, so print copyright and proceed */
        
    printCopyright();
    
    /* Add an include path to the header files to the cpp args */
    
    strcat (cppArgs ,  tmpcat(" -I" , htcPath(INCLUDEPATH) , NULL));

    /* remove tmp files (shouldn't be there anyway) */  
        
     removeTmpFiles();     
    
    /* call the routines for the compile */
    
    for (i = 1 ; i < argc ; i++) {
        if (argv[i]) {
            if ((newer == TRUE) && (fileModTime(argv[i]) < fileModTime(tmpcat(fileName(argv[i]) , ".obj" , NULL)))) {
                if (verbose) printf("Skipping %s (newer .obj exists)\n" , argv[i]);
                continue;
            }
            if (verbose) printf("Compiling %s\n" , argv[i]);
            fname = fileName(argv[i]);
            filetype = fileType(argv[i]);
            if (filetype == CFILE) {
                /* ansi c should evalute line below from left to right. Lazy evaluation will cause failure in any part 
                   to abort compilation and set compNOk to 1 and e to return code from compilation stage called. */
                compNOk = ((e=excpp(argv[i])) || (e=exirrel()) || (e=exp1()) || (e=excgen())
                || (e=exminioptf())
                || (e=exalloca())
                || (e=exoptim()) || (e=exminiopt()) || (e=exzas(tmpFile(OUT) , fname)));
                if (keepasofile) {
                    deleteFile(tmpcat(fname , ".aso" , NULL));
                    rename(tmpFile(OUT) , tmpcat(fname , ".aso" , NULL));
                }
            }
            else if (filetype == ASFILE)
                compNOk = (e = exzas(argv[i] , fname));
            if (e == CTR_C_RETURN) 
                zexit("Ctr-C Escape - Program Terminated\n" , 255);                      
            if (compNOk) {
                deleteFile(tmpcat(fname , ".obj" , NULL));
                deleteFile(tmpcat(fname , ".aso" , NULL));
                if (exitonerror)
                    zexit("" , 1);
                else
                    exitcode = 1;
                    link = FALSE;
                    wizard = FALSE;
            }
        }
    }
    if (link == TRUE)
        if (exlink()) 
            zexit("Linking Failed\n" , 1);
    if (wizard == TRUE) {
        if (exmakewzd()) 
            zexit("Makewzd failed\n" , 1);
        else if (startdl == TRUE) {
            cp = getenv("OS");
            if (cp && !strcmp(cp , "Windows_NT"))
                cp = " cmd /c start ";
            else 
                cp = " start ";
            if (verbose) printf ("Attempting to start Wizard downloader. If this fails, refer to zcc.txt.\n");
            system(tmpcat(cp  , outfilename , ".wzd" , NULL));
        }
    }      
    zexit("" , exitcode);
}


/* Routines to spawn of various componemts of the compiler.
/* A pair of tmp files are used, with the tmp file names 
/* being transposed after each spawn so that the outfile from 
/* the previous spawn becomes the infile fo the next spawn.
***********************************************************/   
   

int excpp (char * infile) {
    tmpFile(TRANSPOSE);
    if (verbose == 2)
        printf("%s %s %s %s\n" , CPPVERBOSE , cppArgs , infile , tmpFile(OUT)); 
    return (spawnle(htcPath(CPPEXE) , "" , cppArgs , infile , tmpFile(OUT) , NULL , envpath));
}

int exirrel (void) {
    if (irrel) {
        if (verbose == 2) 
            printf("%s %s\n" , IRRELVERBOSE , tmpFile(OUT));
        return (spawnle(htcPath(IRRELEXE) , "" , tmpFile(OUT) , NULL , envpath));
    }
    return (0);
}


int exp1 (void) {
    tmpFile(TRANSPOSE);
    return (zxcc ("p1" , p1Args , tmpFile(IN) , tmpFile(OUT)));
}
    
int excgen(void)    {
    tmpFile(TRANSPOSE);
    return (zxcc ("cgen" , cgenArgs ,  tmpFile(IN) ,  tmpFile(OUT)));
}

int exoptim (void) {
    if (optim) {
        tmpFile(TRANSPOSE);                    
        return (zxcc ("optim" , "" ,  tmpFile(IN) ,  tmpFile(OUT)));
    }
    return(0);
}


int exminiopt (void) {
    int e = 0;
    
    if (!ccopt)
        return 0;
	
    if (verbose != 2) togglestderr();
        if (profile == TRUE) 
            (e=_exminiopt(MINOPT0,"")) || (e=_exminiopt(MINOPT,"")) || (e=_exminiopt(MINOPT1,"")) || (e=_exminiopt(MINOPT2,"p"));
	    else
            (e=_exminiopt(MINOPT0,"")) || (e=_exminiopt(MINOPT,"")) || (e=_exminiopt(MINOPT1,"")) || (e=_exminiopt(MINOPT2,""));	    
    if (verbose != 2) togglestderr();
    return(e);
}

int exalloca (void) {
    if (!handle_alloca)
        return 0;
	if (verbose == 2)
	    printf ("alloca <%s >%s\n" , tmpFile(OUT) , tmpFile(IN));
    return _exminiopt(FIXALLOCA,"");
}


int exminioptf (void) {
    if (!ccopt)
        return 0;
	if (verbose == 2)
        printf ("minioptf <%s >%s\n" , tmpFile(OUT) , tmpFile(IN));
    return _exminiopt(MINIOPTF,"");
}



int _exminiopt (char * miniopt , char * args) {
    int oldstdinhandle;
    int oldstdouthandle;
    int filestdin;
    int filestdout;
    int res;
    
    tmpFile(TRANSPOSE);
    
    /* set up redirections so that stdin comes from tmpFile(IN) and stdout goes to 
    tmpFile(OUT). freopen() not used as it is not reversible */
    
    oldstdinhandle = dup(fileno(stdin));
    filestdin = open(tmpFile(IN) , 0 );
    dup2 (filestdin , fileno(stdin));
    close (filestdin);

    oldstdouthandle = dup (fileno(stdout));
    filestdout = creat(tmpFile(OUT) , S_IREAD | S_IWRITE);
    dup2 (filestdout , fileno(stdout));
    close (filestdout);
            
    
    res = spawnle(htcPath(miniopt) , "" , args , NULL , envpath);

    /* reverse the redirections */

    dup2 (oldstdinhandle , fileno(stdin));
    close (oldstdinhandle);

    dup2  (oldstdouthandle , fileno(stdout));
    close (oldstdouthandle);
    return (res);
}    
   
   
    
int exzas (char * infile , char * outfile) {
    char * cmdargs;
    
    if (compile) {
        remove (tmpcat(outfile , ".obj" , NULL));
        cmdargs = tmpcat(ZASARGS , " --o" , outfile , ".obj " , NULL);      
        if (usejonas || (fileType(infile) != ASFILE)) 
            strcat(cmdargs , ZASJARG);
        return (zxcc ("zas" , cmdargs ,  infile ,  ""));
    }
    return(0);
}


/* spawn a command using the zxcc CPM emulator */

int  zxcc (char * comfile , char * args , char * infile , char * outfile) {
    static char zxccPath[SMALLBUFSIZE] = "\0";
    /* cache the path as there will be a few calls to this proc */
    if (!(*zxccPath))
        strcpy(zxccPath , htcPath(ZXCCEXE));
    if (verbose == 2)
        printf("%s %s %s %s\n" ,comfile , args , infile , outfile);
    return (spawnle(zxccPath , "zxcc.exe"  , comfile , args , infile , outfile , NULL , envpath));
}


/* Linker Routines 
******************/

int exlink (void) {
    int res;
    setModelObj();
    remove (tmpcat(outfilename , ".map" , NULL));
    if (linkModel != LINKAUTO) { 
        res = _exlink();
        if (res == CHECKMODELFAIL) 
            printf("Incorrect link model selected\n");
    } else {
        linkModel = LINK32K;
	    res = _exlink();
        if (res  == CHECKMODELFAIL) {
            if (verbose) printf("Relinking to 48K model\n");     
            linkModel = LINK48K;
            res = _exlink();
        }
    }
    if (res)
        remove (tmpcat(outfilename , ".bin" , NULL));
    return(res);
}
 
int _exlink (void) {
    int res;

    switch (linkModel) {
        case LINK16K:
            if (res = linker(LINKARGS16K , outfilename , obj16k , linkFiles , linkLibs))
                break;
            res = checksym(outfilename , CHECKSYM16K);
            break;
        
        case LINK32K:     
            if (res = linker(LINKARGS32K , outfilename , obj32k , linkFiles , linkLibs))
                break;
            if (checkModel(outfilename) != 32) {
                res = CHECKMODELFAIL;
                break;
            }
            if (!(res = checksym(outfilename , CHECKSYM32K_1)));
				res = checksym(outfilename , CHECKSYM32K_2);
            break;

        case LINK48K:
            if (res = linker(LINKARGS48K_1 , outfilename , obj48k , linkFiles , linkLibs))
                break;
            if (checkModel(outfilename) != 48) {
                res = CHECKMODELFAIL;
                break;
            }
            if (res = checksym(outfilename , CHECKSYM48K_1_1))
                break;
            if (!(res = checksym(outfilename , CHECKSYM48K_1_2)))
                break;
            if (verbose) printf ("Re-linking to force segment beforebss into its proper place\n");
            if (res = linker(LINKARGS48K_2 , outfilename , obj48k , linkFiles , linkLibs))
                break;
            res = checksym(outfilename , CHECKSYM48K_2);
            break;
            
    }
    return (res);
} 
            

int linker(char * args , char * filename , char * objmodel , char * objfiles , char * libfiles) {
    FILE * fp;
    char  linkexe[SMALLBUFSIZE];
    char hlinkargs[10] = "";
    char * linkverbose;
    int res;
    if ((fileModTime(htcPath(HLINKEXE)) != -1) && (fileModTime(htcPath(HLIN1EXE)) != -1)) {
        strcpy (linkexe , htcPath(HLIN1EXE));
        linkverbose = HLINKVERBOSE;
	    if (verbose != 2) strcpy (hlinkargs , "-w9");  /* this is required to stop the warning about psect */
    }
    else {
        strcpy (linkexe , htcPath(ZXLINKEXE));
        linkverbose = ZXLINKVERBOSE;
    }
    if (verbose == 2) 
        printf("%s %s %s -m%s.map -o%s.bin %s %s %s\n", linkverbose , hlinkargs , args , filename , filename , objmodel , objfiles , libfiles);  
    else if ((linkverbose == HLINKVERBOSE) && verbose)
        printf ("Linking...\n");
	remove(ARGSFILE); 	    
    if (!(fp = fopen(ARGSFILE , "w"))) {
        printf("Cannot create file %s\n" , ARGSFILE);
        res = 1;
    } else {
 		remove(tmpcat(filename , ".map" , NULL));
		remove(tmpcat(filename , ".bin" , NULL));   
        fprintf(fp , "%s %s -m%s.map -o%s.bin %s %s %s\n", hlinkargs ,  args , filename , filename , objmodel , objfiles , libfiles);  
        fclose(fp);
        res = spawnle(linkexe , "" , tmpcat("@" , ARGSFILE , NULL) , NULL , envpath);
    }
    fflush(stdout);
    fflush(stderr);
    return(res);
}

int checkModel (char * file) {
    int model;
    char sym1[SMALLBUFSIZE] = "";
    char sym2[SMALLBUFSIZE] = "";
    char line[MEDBUFSIZE]; 
    FILE * fp;
    if (verbose == 2) 
        printf("Checking link model\n"); 
    if (!(fp=fopen(tmpcat(file , ".map" , NULL),"r")))
        zexit(tmpcat("Cannot open " , file , ".map\n" , NULL) , 1);
    while(fgets(line,MEDBUFSIZE,fp)) {
        if(!strcmp(line , "                                  Symbol Table\n")) {
            break;
        }
    }
    model = 32;
    while(fgets(line,MEDBUFSIZE,fp)) {
        sscanf(line,"%s %*s %*s %s" , sym1 , sym2);
        if (!(strcmp(sym1 , PSECT48KONLY) && (strcmp(sym2 , PSECT48KONLY)))) {
            model = 48;
            break;
        }
    }
    fclose(fp);
    return(model);
}      
 

/* checksym routine
*******************/
   
int checksym(char * file , char * args) {
    
    if (verbose == 2) printf("checksym %s.map %s\n" , file , args);
    else if (verbose == 1) printf ("Checking crucial symbol locations...\n");
        
    return (spawnle (htcPath(CHECKSYMEXE) , "" , tmpcat(file , ".map " ,  NULL) , args , NULL , envpath);
    
}

/* makewzd file routine 
  *********************/

int exmakewzd(void) {
    char cmd[MEDBUFSIZE] = "";
    
    if (*wizardid != '\0')
            strcat(cmd , tmpcat("-" , wizardid , " " , NULL));
    strcat (cmd , tmpcat(outfilename , " " , NULL));  /* add filename to use */
    
    if (*wizardinfofile != '\0') 
            strcat(cmd , tmpcat("@" , wizardinfofile , " " , NULL));
    else  {         
            strcat(cmd , tmpcat(outfilename , " " , NULL));  /* add title */
        if (*wizarddescfile != '\0')
            strcat(cmd , tmpcat("@" , wizarddescfile , " " , NULL));  /* add @descfile*/
        else
            strcat(cmd , tmpcat("\"" , outfilename , "\" " , NULL)); /* add outfile as description */
        strcat(cmd , wizardiconfile);                               /* add iconfile name */
    }       
    
    if (verbose == 2) 
        printf ("makewzd %s\n" , cmd);
    else 
        if (verbose) printf("Making wizard file %s.wzd\n" , outfilename);
	remove(tmpcat(outfilename , ".wzd" , NULL));
    return (spawnle(htcPath(MAKEWZDEXE) , "" , cmd , NULL , envpath));
}  
    



/* Miscellaneous Rotines
************************/

/* Return the file type */

int fileType (char * filename) {
    int filetype;
    char * cp;
    cp = filename + strlen(filename);
    if (!strcmp(cp - 2 , ".."))
        filetype = PARENTDIR;
    else if ((!strcmp(cp - 3 , ".as")) || (!strcmp(cp - 3 , ".AS")))
        filetype = ASFILE;
    else if ((!strcmp(cp - 4 , ".asm")) || (!strcmp(cp - 4 , ".ASM")))
        filetype = ASFILE;  
    else if ((!strcmp(cp - 2 , ".c")) || (!strcmp(cp - 2 , ".C")))
        filetype = CFILE;
    else if ((!strcmp(cp - 4 , ".obj")) || (!strcmp(cp - 4 , ".OBJ")))
        filetype = OBJFILE;
    else if ((!strcmp(cp - 4 , ".lib")) || (!strcmp(cp -4 , ".LIB")))
        filetype = LIBFILE;
    else
        filetype = UNKNOWNFILE;
    return (filetype);
}


/* return the modifcation time of a file, or -1 if file does not exist. 
   Also be used to check that a file exists */

long fileModTime (char * filename) {
    struct stat sb;
    if (stat(filename , &sb) == -1)
        return (-1);
    return (sb.st_mtime);
}
    
            

/* concat the addPath to the htc directory path and return a pointer to the result*/

char * htcPath (char * addPath) {
    static char path[SMALLBUFSIZE] = "\0";
    static char * htcPathEnd = 0;

    if (!(*path)) {
        strcpy(path , _fullpath); 
        *(strrchr(path ,'\\')) = '\0';
        htcPathEnd = (strrchr(path ,'\\') + 1);
    }
    
    *htcPathEnd = '\0';
    strcat(path , addPath);
    
    return (path);
}


/* returns a filename alternating between TMPFILE1 and TMPFILE2 
   if alt = 0 return previous name
   if alt = 1 return alternate name */

char * tmpFile (int action) {
    static BOOL tsel = 0;
    switch (action) {
        case TRANSPOSE:
		    remove (tsel ? TMPFILE1 : TMPFILE2); 
	        tsel = !tsel;
            break;
        case IN:
            return (tsel ? TMPFILE1 : TMPFILE2);    
    }
    return (tsel ? TMPFILE2 : TMPFILE1);
}


/* concat all args into a single buffer. Last arg must be a NULL.
   Returns a pointer to the buffer. */

char * tmpcat (char * arg , ...) {
    static char buf[BIGBUFSIZE];
    char  ** a;
    *buf = '\0';
    a =  &arg;
    while (*a != NULL) {
        strcat(buf , *a);
        *a++;
    }
    return (buf);
}


/* Strip away path and extension from a filename */

char * fileName(char * fullname) {
    static char buf[MAXFILENAMELEN + 1];
    char * cp;
    
    buf[MAXFILENAMELEN] = '\0';
    if (!(cp = strrchr(fullname , '\\'))) 
        cp = fullname - 1;
    strncpy(buf ,++cp , MAXFILENAMELEN);
    if (cp = strrchr(buf , '.'))
        *cp = '\0';
    return(buf);
} 
            
        
                                        
void removeTmpFiles(void) {
    deleteFile (TMPFILE1);
    deleteFile (TMPFILE2);
    deleteFile (NULLFILE);      
    deleteFile (ARGSFILE);
}   
  

void deleteFile (char * fname) {
    if ((verbose == 2) && (fileModTime(fname) != -1))
            printf ("Deleting %s\n" , fname);
    remove (fname);
}    

/* redirect stderr to a file. freopen() cannot be used for
    this as it is not then possible to reverse the redirection */

void (togglestderr) (void) {
    static BOOL stderrOn = TRUE;
    static int oldstderrhandle;
    static int filestderr;
        
	switch (stderrOn) {
	    case TRUE:
            oldstderrhandle = dup (fileno(stderr));      
            filestderr = creat(NULLFILE , S_IREAD | S_IWRITE);
            dup2 (filestderr , fileno(stderr));
            close (filestderr);
            break;
        
	    case FALSE: 
            dup2 (oldstderrhandle , fileno(stderr));
            close (oldstderrhandle);
    }
    stderrOn = !stderrOn; 
           
}



/* zexit prints msg, prints compilation result and time
   and cleans up tmp files */

void zexit (char * msg , int exitcode)  {
    if (verbose) printf("%s" , msg);
    removeTmpFiles();
    if (exitcode)
        printf("Compile failed\n");
    else 
        if (verbose) printf("Compile successful (time = %ld secs)\n" , time(NULL) - startTime );
    exit(exitcode);
}      
    
    
    
    
        
        
                

    
