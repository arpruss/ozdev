#include <oz.h>
#include "same.h"

static byte y;

void line(char *s)
{
    ozputs(0,y,s);
    y+=8;
}

void help0(void)
{
y=0;
ozcls();
line("This is a Wizard version of the Macintosh SameGame");
line("game.  This version is by Alexander R. Pruss.");
line("You're given a board filled with pieces.  You must");
line("try to remove as many pieces as possible from the board.");
line("You can only remove pieces when they are beside other");
line("pieces of the same shape, and then the whole connected");
line("set of pieces including the piece you selected gets");
line("removed, and the other pieces shift down and/or left to");
line("fill in the gaps.");
line("(ESC to exit help, page down to navigate help.)");
}

void help1(void)
{
y=0;
ozcls();
line("The game ends when you can't remove any more pieces");
line("or you've removed them all.");
line("Use the arrow keys to move the cursor. Press ENTER to");
line("select a piece and all pieces of the same shape beside");
line("them.  This will highlight all these pieces. If you press");
line("ENTER again on any one of these pieces, all of these");
line("pieces will be removed.  Press C to clear highlighting, or");
line("just press ENTER on another unhighlighted piece.  SPACE");
line("is the same as two ENTERs.  ESC quits the game.");
line("(ESC to exit help, page down/up to navigate help.)");
}

void help2(void)
{
y=0;
ozcls();
line("You get a much bigger score if you remove larger");
line("connected areas at once. In fact, for removing N pieces");
line("at the same time, you get (N-2)^2 (the square of N-2)");
line("points.  At the end of the game, you lose one point for");
line("every pieces left on the board.  If you've cleared");
line("the board, you get a bonus of five points for every piece");
line("that the board started out with.");
line("In the game, you can press MENU for this help text.");
line("");
line("(ESC to exit help, page up to navigate help.)");
}

void help(void)
{
    ozsavescreen();
HELP0:
    help0();
    switch(ozgetch())
    {
        case KEY_LOWER_ESC:
        case KEY_UPPER_ESC:
        case 27:
            goto TERMINATE;
    }
HELP1:
    help1();
    switch(ozgetch())
    {
        case KEY_LOWER_ESC:
        case KEY_UPPER_ESC:
        case 27:
            goto TERMINATE;
        case KEY_UP:
        case KEY_PAGEUP:
            goto HELP0;
    }
HELP2:
    help2();
    switch(ozgetch())
    {
        case KEY_UP:
        case KEY_PAGEUP:
            goto HELP1;
    }
TERMINATE:
    ozrestorescreen();
    return;
}

