This program lets you load expenses from Wizard to PC.  There are
instructions after the license, below.

BY DOWNLOADING YOU SIGNIFY YOU HAVE READ AND AGREED TO ALL OF THE
CONDITIONS LISTED HERE.  This is a C/assembly program.  It is possible
for a C/assembly program to damage data and even to make your organizer
inoperable.  USE AT YOUR OWN RISK.  NO GUARANTEES ARE MADE. By
downloading this program to your organizer you agree not to hold the
author(s) responsible for any damage to data, hardware or anything else.
IF ANY OF THESE CONDITIONS ARE NOT PERMITTED BY APPLICABLE LAW, YOU ARE
NOT LEGALLY PERMITTED TO DOWNLOAD OR USE THE PROGRAM. This program may
be freely distributed but only provided that this description is
unmodified.  This program is copyright (c) 2000 Alexander R. Pruss.

REQUIREMENTS: OZ/ZQ-700PC/730PC/750PC/770PC.  Will NOT work on the
multilingual ZQ-700M or 750M.

It is recommended you back up your Wizard before running C/assembler
programs. NOTE: If a C/assembly program hangs due to a bug, you may need
to flip the battery switch on the bottom of the Wizard back and forth.
If the Wizard does not respond to this, taking the batteries out for
half a minute should help.  If this doesn't help, you've read the
WARNING above.  None of this should be necessary, but no guarantees are
made. The auto power off is disabled when expdump is running.  Report
bugs and suggestions to: pruss@imap.pitt.edu.

BASIC INSTRUCTIONS:  Start up EXPDUMP.  It will load data and ask you to
press a key to dump it.  Don't press a key yet.  Start up Hyperterm on
Windows system.  Go to FILE | PROPERTIES.  Configure Hyperterm as
follows.  Connect using "Direct to COMx" (where COMx is the COM port
your Wizard is connected to).  Press CONFIGURE.  Set "Bits per second"
to 9600, "Data bits" to 8, "Parity" to None, "Stop bits" to 1, "Flow
control" to None.  Save these settings (e.g., in a file called
"Organizer").  Now, assuming you are in Hyperterm with all the right
settings, choose TRANSFER | CAPTURE TEXT.  Choose where you want to save
your expenses, giving an extension of .CSV (comma-separated values).
Then, finally, press a key on the Wizard to dump the data.  You will see
the data scrolling by on screen.  Then do "TRANSFER | CAPTURE TEXT |
STOP" on PC.  You now have a .CSV file with all your expenses which you
can load into Excel.

LIMITATIONS:  This assumes you have no more than 3000 expense entries.
Currently, the program dumps all entries you have (up to the maximum)
and does not allow any selection.

TECHNICAL INFORMATION: This program is written in Hi-Tech C.
