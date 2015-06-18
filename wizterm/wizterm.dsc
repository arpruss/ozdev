There are instructions below, under the license.

BY DOWNLOADING YOU SIGNIFY YOU HAVE READ AND AGREED TO ALL OF THE
CONDITIONS LISTED HERE.  This is a C/assembly program.  It is possible
for a C/assembly program to damage data and even to make your organizer
inoperable.  USE AT YOUR OWN RISK.  NO GUARANTEES ARE MADE. By
downloading this program to your organizer you agree not to hold the
author(s) responsible for any damage to data, hardware or anything else.
IF ANY OF THESE CONDITIONS ARE NOT PERMITTED BY APPLICABLE LAW, YOU ARE
NOT LEGALLY PERMITTED TO DOWNLOAD OR USE THE PROGRAM. This program may
be freely distributed but only provided that this description is
unmodified.  Most of this program is copyright (c) 2000 Alexander R
Pruss, but some is by Benjamin Green (c) 2000 OZDEV.

It is recommended you back up your Wizard before running C/assembler
programs.

NOTE: If a C/assembly program hangs due to a bug, you may need to flip
the battery switch on the bottom of the Wizard back and forth.  If the
Wizard does not respond to this, taking the batteries out for half a
minute should help.  If this doesn't help, you've read the WARNING
above.  None of this should be necessary, but no guarantees are made.

This program may not have been sufficiently tested.  Report bugs and
suggestions to: pruss@imap.pitt.edu.

BASIC INSTRUCTIONS: The configuration should be stored in a memo
entitled "zz:wizterm.config" (no quotes, case significant).  Press "MY
PROGRAMS" to exit, lower-MENU for online help, upper-MENU to adjust baud
rate.  For special keys see the online help.

SOME NOTES: The "lines" option in the configuration file takes two
values: 13 and 16.  The 16 line mode is very cramped, and underlining is
disabled, but may be useful.  The "columns" option takes the values of
59 and 60.  Set it to 60 unless the "bleeding" effects at the right hand
margin due to inverse characters (and the @ sign, should it happen to be
at the right hand margin) annoy you.  You can also change the screen
layout with the upper MENU key.  

The "key" option in the configuration file takes two arguments.  The
first is the soft-key name, which can be category-x or 2nd-x, where x is
any alphanumeric character not used by the Wizard or Wizterm for a
special character, and the second argument is the key definition which
can use C-style escape sequences like \r or \xAB.

The 2nd-v key types the contents of the Wizard clipboard into the
terminal.  (The Wizard clipboard is the one you copy into with 2nd-c in
the memo editor and in other applications.)

In Wizterm, the 2nd-c key copies the current screen to a memo entitled
"Wizterm at hh:mm:ss".

To connect your wizard to an external modem, use a null-modem cable in
addition to the PC-link, or make your own cable (see
http://www.ozdev.com/knowledge/howto/connectivity/cables.htm).  Ensure
that the modem is set ignore DTR (on some modems there is a DTR override
switch, and on others there may be a command like at&d0) and that
hardware handshaking is turned off (see modem manual).  Not all modems
will work, but I think most full-size external modems should.  All such
connections are at your own risk--I will not be responsible for damage
to Wizard, modem or anything else.

NOTE: Any alarms going off while Wizterm is running will be missed.  The
auto power-off won't work either.

TECHNICAL INFORMATION: This program is written in Hi-Tech C, with some
assembly bits for i/o.  Source code is available in the latest Wizard
Hi-Tech C distribution on www.ozdev.com under "win32 downloads".
