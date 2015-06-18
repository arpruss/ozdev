There are two old chess programs included in this archive.

I have compiled and run them with Borland's free 16 bit DOS compiler,
Turbo C (available in the museum section of their web site) and they
appear to be working.

But first, a bit of history...

Both programs were originally written on an 8-bit system.  The Tandy
Color Computer 3, with 512k memory and running a 6809 cpu at 1.8mhz.
The os was Microware's OS9 Level 2, which was a genuine multi-tasking
OS.

The compiler used was Microwares own C compiler.  The only C compiler
available for the system.  It was wrttin in mid 1983 and was k&r style,
of course.  In addition to that, the compiler had quite a few quirks of
its own.  In these days of ANSI/ISO standard C and the C99 standard,
going back to code written for a compiler like that can be quite a
shock.  k&r code was always quirky, but this one really had a few more
of its own!

I've cleaned up some of the junk in the code that resulted from working
under such a system, but there is still a lot of cleaning left.


After going through all my old chess floppies from 9-12 years ago, it
looks like:


1) I went through a series of poor chess programs.  Most in Pascal.

2) I switched to C (and a new OS) and started over.

3) I wrote several small ones.  I also wrote a couple "How to write a
chess program" type of tutorials.

4) I wrote a very big one, with opening book, transposition table,
clocks, etc.  Pretty complete.  It required about 100k to run.  The
search was done as a background task in its own 64k address space.  It
was based on John Stackbacks PD 1987 version with a few ideas from GNU
chess (v3?)  It was very system specific.

5) I decided "small is beautiful" and "Portability is cool" and started
on an extremely small version.

6) I decided to make it compile with SMALL-C, which was a popular C
subset back in the 80's.

7) I wrote Kelly, versions 1-4.  Part way through I decided just to stay
with k&r C but keep it simple.  I kept it as self contained as possible
with little dependance on any compiler libraries.  The only OS
requirements were character I/O and a clock source.

8) I renamed to "ARChess" and went up to version 9.

I don't see anything that is beyond that for certain.



This archive contains two programs.

The first is "Bloody" and appears to be from early '92 and was the last
one I wrote in step 3 above.  There were a number of OS9 system specific
parts, such as the interface.  I've changed some of that so it behaves
tolerably well under DOS, but it would really need to be rewritten.
[This is not included here.]

The second chess program is "ARChess" and appears to be the last one I
wrote.  It was much more portable.  In fact, I went out of my way to
make it depend on only character I/O and a timer.  No other compiler
libraries were needed.

I've cleaned both of them up, but neither are perfect.  Partially it was
due to the system I was working with and partially because I never
really stabalized any program.  As soon as I got a new idea I started
coding it, rather than finishing the previous idea.

The programs were in a constant state of change.  But, as near as I can
tell from looking at these old disks, the two enclosed were the most
stable of the ones I could find.  That assumes I haven't missed any
disks.


ARChess 9 is a little bigger than Bloody is.  A few more features etc.
A lot of that is frills and the I/O, which can be removed or rewritten.

I'm not sure how much data space it took.  It looks like ARChess took
about 6k for basic play data, 3k for game history (so you can unmake
moves), plus 8k for the history heuristic (for move sorting), plus an
unknown amount of stack space.  Some of the space can be reduced by
reducing certain limits or manually removing features.

If you need to, you can remove all the references to the history[]
heuristic.  Just cut those lines out.  That will effect the quality of
play, but it'll still run.  And it'll take 8k less data space.


Both programs are compiled by just compling each module and then linking
them together.

Such as:   tcc -A -ms -echess *.c


Both programs are copyrighted freeware.  You can pretty much do what you
want to with them as long as you leave my name in there as the original
author.

The one big exception is that you may **NOT** put the GNU public license
(or any derivative) on it.  The GPL attempts to force code sharing by
'infecting' other code with it's own license.  If you've written your
own million line program and you include some GPL'ed code, then the GPL
forces itself onto your other code.

Me, personally, I don't care what you do with your code.  I don't have
the right to tell you what you should or should not do.  If you want to
make yours free, then fine.  If you don't, then fine.


Carey Bloodworth CareyBloodworth@yahoo.com November 11, 2001


[ And then Alexander Pruss ported this for the Wizard ]
