Decus cpp is a public-domain implementation of the C preprocessor.  It
runs on VMS native (Vax C), VMS compatibilty mode (Decus C), RSX-11M,
RSTS/E, P/OS, and RT11, as well as on several varieties of Unix,
including Ultrix.  Decus cpp attempts to implement features in the ANSI
Standard for the C language.

I would be happy to receive fixes to any problems you encounter.  As I
do not maintain distribution kit base-levels, bare-bones diff listings
without sufficient context are not very useful.  It is unlikely that I
can find time to help you with other difficulties.

			Acknowledgements

I received a great deal of help from many people in debugging cpp.  Alan
Feuer and Sam Kendall used "state of the art" run-time code checkers to
locate several errors.  Ed Keiser found problems when cpp was used on
machines with different int and pointer sizes.  Dave Conroy helped with
the initial debugging, while Arthur Olsen and George Rosenberg found
(and solved) several problems in the first USENET release.

Martin Minow
decvax!minow

                         Modifications

This package is modified for lcc by Michael Haardt.  There is a new
manual page for unix systems, and the above README was edited for the
unix environment.  cpp now supports ANSI token concatenation and
quoting, and the ANSI-draft token quoting is removed.  It also supports
-P now and eliminates \<newline> from the output.  See the manual page
for bugs.

Michael Haardt
michael@cantor.informatik.rwth-aachen.de
