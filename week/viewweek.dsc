(Instructions are below the license.)

BY DOWNLOADING YOU SIGNIFY YOU HAVE READ AND AGREED TO ALL OF THE
CONDITIONS LISTED HERE.  This is a C/assembly program.  It is possible
for a C/assembly program to damage data and even to make your organizer
inoperable.

USE AT YOUR OWN RISK.  NO GUARANTEES ARE MADE. By downloading this
program to your organizer you agree not to hold the author(s)
responsible for any damage to data, hardware or anything else. IF ANY OF
THESE CONDITIONS ARE NOT PERMITTED BY APPLICABLE LAW, YOU ARE NOT
LEGALLY PERMITTED TO DOWNLOAD OR USE THE PROGRAM. This program may be
freely distributed but only provided that this description is
unmodified.  This program is copyright (c) 2000 Alexander R. Pruss,
except for some i/o code by Benjamin Green.

It is recommended you back up your Wizard before running C/assembler
programs.

NOTE: If a C/assembly program hangs due to a bug, you may need to flip
the battery switch on the bottom of the Wizard back and forth.  If the
Wizard does not respond to this, taking the batteries out for half a
minute should help.  If this doesn't help, you've read the WARNING
above.  None of this should be necessary, but no guarantees are made.
The auto power off is disabled when viewweek is running.  Also, the
battery low detection is off during viewweek--make sure your batteries
are OK before starting viewweek.

This program may not yet been sufficiently tested.  Report bugs and
suggestions to: pruss@imap.pitt.edu.

BASIC INSTRUCTIONS:  Press I or MENU for help.  When you start the
program, you see as much of the current week's schedule (including, by
default, appointments, anniversaries and those to-dos that have due
dates within this week and are not yet done) as fits on the screen
(starting with today's date).  Use up/down arrows and page-up/page-down
keys to move within the week.  Little arrows in the upper right or lower
right hand corners will appear if there is more data above or below
(respectively) the current screen.  Press F to change font size.  Press
R to make entries repeat or to activate the deletion key, and the press
ENTER after selecting an entry to repeat the entry or DELETE to delete
the entry.  Press N and P for next and previous weeks.  Press left and
right arrow to move week start back or forward by a day.  Press E to
toggle display of empty days, H to toggle display of day headers, S to
toggle inclusion of schedules, T to toggle inclusion of TODOs, A to
toggle inclusion of birthdays/anniversaries (all three included in view
by default;  press a help (MENU/I) key to be told what is currently
being viewed), and press 2 to toggle 12/24 hour mode.  Press Q or MY
PROGRAMS to quit.  You can set options permanently by having a memo
entitled "zz:viewweek.config" (without quotes and all lower case)--a
sample should be available from the same source as you got this
program--and putting a 1 in the file turns on an option and 0 turns it
off.  One new option for the config memo is forceautorun.  If you put
the line:

  forceautorun 1

in your zz:viewweek.config file, and then run ViewWeek, ViewWeek will be
automatically set to auto run.  This is handy, because then if you
upgrade you just need to run ViewWeek to make the new version be
auto-run, instead of actually setting it.

LIMITATIONS:  There is a limit of 115 entries per day.  If you are
running up against this limit, contact me and I'll try to increase it.
Appointments/TODOS/anniversaries longer than about 980 characters will
be truncated to about 980 characters when repeated.  Any alarms
scheduled to go off while viewweek is running will be missed.

PAYMENT:  If you find this program useful, you are encouraged, but not
required, to make a $5 donation to a pro-life organization.  Press MENU
and then D in viewweek for more information.

TECHNICAL INFORMATION: This program is written in Hi-Tech C, with some
assembly stuff for i/o.  See www.ozdev.com for information on
programming, and source code (perhaps out of date) under "win32
software".
