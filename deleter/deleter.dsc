Instructions are found below.

BY DOWNLOADING YOU SIGNIFY YOU HAVE READ AND AGREED TO ALL OF THE
CONDITIONS LISTED HERE.  This is a C/assembly program.  It is possible
for a C/assembly program to damage data and even to make your organizer
inoperable.  USE AT YOUR OWN RISK.  NO GUARANTEES ARE MADE. By
downloading this program to your organizer you agree not to hold the
author(s) responsible for any damage to data, hardware or anything else.
IF ANY OF THESE CONDITIONS ARE NOT PERMITTED BY APPLICABLE LAW, YOU ARE
NOT LEGALLY PERMITTED TO DOWNLOAD OR USE THE PROGRAM. This program is
copyright (c) 2001 Alexander R. Pruss.  It may be freely distributed but
only provided that this description is unmodified.

This is a alpha test program that has undergone only very limited
preliminary tests.  In general, it is recommended you back up your
Wizard before running C/assembler programs.

NOTE: If a C/assembly program hangs due to a bug, you may need to flip
the battery switch on the bottom of the Wizard back and forth.  If the
Wizard does not respond to this, taking the batteries out for half a
minute should help.  If this doesn't help, you've read the WARNING
above.  None of this should be necessary, but no guarantees are made.

Report bugs to: pruss@imap.pitt.edu.

INSTRUCTIONS: This lets you delete appointments or TO DO entries within
a given date range.  Dates must be specified in MM/DD/YYYY format.  The
TO DO deleter deletes UNDONE items whose due date is in the indicated
range.  It does not delete DONE items.

NOTE 1: The TO DO deleter has not been tested in any significant way.

NOTE 2: As you may know, some programs (those using the C-based file
system) store data in special January 1, 1901 schedule entries.  Deleter
leaves those schedule entries alone, so you don't need to worry.
Moreover, Deleter does something handy for them.  Sync'ing a Wizard that
uses the C-based file system with DayTimer can produce dummy January 1,
1901 entries that do not actually store any data.  These dummy entries
will be deleted if Deleter is run with start date 01/01/1901.

NOTE 3: The auto power off feature is disabled when Deleter is running.

NOTE 4: The default date range is from January 1, 1901 to 60 days before
the current date.
