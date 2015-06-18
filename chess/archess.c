/* #define PROFILER /* */

/* Beta test AR-Chess V 1.0
   Copyright (c) November 30, 1992

***************   Compiling Notes   *****************
Compile the files seperately
The file ARVAR.C is used as a header, so it must be in the current
   data directory.  No other headers are used.
When you link the program, give it an extra 3k or so for the search recursion

*/

#define HEADER
#include "arvar.c"

extern uchar currow;

main(void)
{
#ifdef PROFILER
ozinitprofiler(); /* */
#endif
initprg();
initgame();

clrscr();
printf(archess_version);
printf("\xDE""Novembe\xF2""30\xAC""1992\n"
"\nMr\xAE""Care\xF9""Bloodworth\n"
"\nThi\xF3""progra\xED""ma\xF9""b\xE5""freely\n"
"copie\xE4""an\xE4""modifie\xE4""a\xF3""long\n"
"a\xF3""m\xF9""nam\xE5""i\xF3""presen\xF4""a\xF3""the\n"
"origina\xEC""author.\n\n"
"Shar\xF0""7x\xF8""port\xBA""\xDE""A.R\xAE""Pruss");
ozdelay64hz(200);

while (1)
  {
   if ( /* (breakpress) || */ (draw) || (mate) ||
        ((tomove==human) && (!bothsides)) ) inputcommand();
   printboard();
/*   if (pvsflag)
   {
    currow=7;
    atright();
    printf(pvsmsg);
   } */
   currow=8;
   atright();
   printf(status);
   currow=6;
   atright();
   printf(msg);
   if (! (mate || draw)) selectmove(tomove);
   /* if (! breakpress) */ tomove=chngcolor(tomove);
   };

return 0;
}

/*
   I've been involved in computer chess, to one degree or another, for
   almost 10 years.  I've written numerous Pascal chess programs with
   rather limited success.  In late 91 I deceded it was finally time
   to learn 'C' and as an education excersise I translated my Pascal
   chess program into rather Pascalish 'C'.  After a couple of rewrites
   over several months I decided to start over and use the GNU Chess 3.0
   evaluator and search routines.  This required writing the program
   as three concurrently running processes (I was using a multitasking
   OS called OS9 for the 6809 8/16 bit microprocessor with 512k of
   banked switched memory.  Because of the microprocessor, each process
   was limited to 64k total address space).  All things considered, it
   turned out fairly well, with a lot of options and features.

   However, it was rather nonportable and I didn't really like its
   size or complexity.  Some of the complexity came from the frills
   that I added, but most came from its size, which required me to
   split the search routine into a seperatly running process that
   communicated with the main process via signals and pipes.  (Nobody
   ever said that shoehorning a 150k program into 64k was easy).

   I recently went back to my last single process program.  I stripped
   a lot of the frills from it and tried to simplify it enough that
   it could be ported to many computers.  I simplified the screen
   down to a 42x24 size, I removed all of the debugging and stats
   features.  I even removed the load/save game feature.  I called it
   'kelly's chess.

   I had hoped to write kelly in the Small-C dialect (a limited subset
   of C that doesn't included multiple dimension arrays or structures).
   However, it proved to be more trouble than it was worth.  Mabye
   someday.

   I considered adding a simple task switcher to allow thinking on its
   opponents time and a displayed elapsed time clock, but I decided it
   was more trouble that it was worth right now.

   I also had planned on converting kelly to 6809 asm.  However, before
   I did this I decided there were a few things that needed fixing.

   This particular program is called 'AR-Chess'.  A pun on 'arches' and
   the old programs 'NuChess' from Northwester University, and
   'DuChess' from Duke University.  I have the misfortune of living in
   Arkansas (with Bill Clinton as Governor), so the program is called
   'AR-Chess'.

   The program is a bit improved over Kelly chess.  I've fixed a few
   quirks, added some search extensions, worked on the time control
   algorithm, etc.  No major improvement, just a number of minor fixes
   that combined, warrant giving it a new name.

   There are still a lot of things I haven't added, but I'm trying to
   keep this small enough it can run on a small 64k 8-bit computer,
   and simple enough to be 'easily' understood by somebody not famililar
   with the program.  I would have liked to have added an opening book,
   and game save/load options, but they would have used to much memory.
   The search routine is a bit more complicated than I like, but this
   is due to the time control features and my decision to handle illegal
   moves, stalemate, etc. in the search also, whereas most programs don't
   bother.  I also added draw by repetition, the 50 move rule (whic is
   now 75 for certain cases I think), and other things that make a basic
   program into an acceptable program.  The time control stuff in the
   search routine is far more complicated than I like.  It adds a little
   to the quality, but at a large apparent cost in complexity to the
   search routine.  Actually, it's the variable time control code that
   adds the complexity, regular time control is easy.  It adds to the
   quality of play by allowing it to sometimes know when to spend more
   time on a difficult position.  This is much better than spending
   the selected amount of time and then moving even though a few more
   seconds might help.

   The program has been somewhat optimized for an 8 bit computer.  For
   example, multiplications and divisions are kept out of the move
   generator, search routines, and evaluator where ever possible because
   on a small computer, these often have to be done with library routines.
   Because of recursion, it does require unrestricted stack size.
   A 6502, 8051 and such would not be able to run it because they are
   limited to 256 bytes of hardware stack (although I don't know why
   you would want to run it on an 8051.)  Pointers are used to index
   the move structure and the game history arrays instead of using
   an index that may require a multiplication (and hence a possible
   library call for the software multiplication).

   The program has been divided into several 'logical' sections: archess,
   search, I/O, standard library, and port.  Files concerned with general
   chess program operation are prefixed with AR.  File concerned with
   I/O are prefixed with io.  Files search.c and slib.c are concerned
   with the search itself.

   Since the only GENUINELY recursive routine is SEARCH() in search.c,
   and since this chess program is not multi-tasking I have elected
   to make a number of variables global rather than auto.  Some processors
   access global, statically located variables faster than stack vars.

   I'm not sure how portable it actually is.  It was written in the old
   K & R 'C' dialect.  Currently I have only run it on one computer.

   Feel free to use the program and give copies away.  The ONLY thing
   I ask that I be given credit by leaving my name in the source code
   and in the signon message.  You can modify it, sell it, throw it
   away, I don't care.  If you do make improvements I would like a
   copy.  Either a hardcopy, a source listing on a MS-DOS 360k disk,
   or a Color Computer disk.  And of course, I want to hear about
   any porting information you have.

   I do intend to keep improving the program (while at the same time,
   keeping its size small enough to run on even 8-bit micros).
   If you come up with an improvement or a bug fix, please let me know.

*/
