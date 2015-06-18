#======================================================================
#
# Tcl Source File -- Created with SAPIEN Technologies PrimalSCRIPT(TM)
#
# NAME: ozProfFilters.tcl
#
# AUTHOR: Lawrence Chitty , n/a
# DATE  : 23/03/02
#
# COMMENT: <comment>
#
#======================================================================

proc displayFilterScreen {} {
	global filter
	set w .filter
	if {[winfo exist $w]} {
		centreWindow . $w
		return
	}
	set filter(opt) [array get filter opt:*]
	toplevel $w
	wm resizable $w 0 0
	wm transient $w .
	wm protocol $w WM_DELETE_WINDOW "array set filter \$filter(opt) ; destroy $w " 	

	set wf $w.f
	frame $wf 	
	grid $wf -padx 5 -pady 5 -columnspan  2
	
	grid columnconfigure $wf 0 -weight 1
	checkbutton $wf.cb1 -text "Filter symbols not beginning with '_'" -variable filter(opt:filter_)
	checkbutton $wf.cb2 -text "Filter symbols not beginning with '$'" -variable filter(opt:filter$)
	frame $wf.sp1 -relief sunken -bd 1 -height 2
	grid $wf.cb1 -sticky w
	grid $wf.cb2 -sticky w
	grid $wf.sp1 -pady 2 -sticky ew
	
	checkbutton $wf.cb3 -text "Use standard symbol filter list" -variable filter(opt:std)
	checkbutton $wf.cb4 -text "Use custom symbol filter list" -variable filter(opt:cust) 
			
	frame $wf.e1
	label $wf.e1.spacer -width 2 
	entry $wf.e1.e -textvariable filter(opt:file)
	button $wf.e1.e.b  -borderwidth 1 -cursor arrow -command browseExcludeFile
	grid $wf.cb3 -sticky w
	grid $wf.cb4 -sticky w
	grid $wf.e1 -sticky ew
	grid columnconfigure $wf.e1  1 -weight 1
	grid $wf.e1.spacer  $wf.e1.e -sticky ew 
	place $wf.e1.e.b -relx 1.0 -rely 0.0 -anchor ne -relheight 1.0
	
	frame $wf.sp2 -relief sunken -bd 1 -height 2
	grid $wf.sp2 -pady 2 -sticky ew
	
	checkbutton $wf.cb5 -text "Use psect if no symbol at address" -variable filter(opt:psect)
	grid $wf.cb5 -sticky w
	
	button $w.b1 -text OK -width 8 -command "buildFilters ; destroy $w ; start \$comms(fileName)"
	button $w.b2 -text Cancel -width 8 -command "array set filter \$filter(opt) ; destroy $w "
	grid $w.b1 $w.b2
	centreWindow . $w
	trace variable filter(opt:cust) w "disableFileBrowse $wf.e1.e \$filter(opt:cust)"
	set filter(opt:cust) $filter(opt:cust) 
}

proc disableFileBrowse {w val var nul nul } {
	if {$val} {
		$w configure -state normal -fg black
		$w.b configure -image [loadImage open.gif] -state normal
	} else {
		$w configure -state disabled -foreground grey
		$w.b configure -image [loadImage opendisabled.gif] -state disabled
	}
	
}
		   

proc browseExcludeFile {} {
	global filter
	set filename [tk_getOpenFile -parent .filter \
			-filetypes {{{Symbol file} {.sym}} 
		      			{{Text file} {.txt}}
						{{All files} {*}}}]
	if {$filename != ""} {
		set filter(opt:file) $filename
	}    	
}


proc buildFilters {} {
	global filter
	set tmpList {}
	if {$filter(opt:filter_)} {
		lappend tmpList {^_.*}
	}
	if {$filter(opt:filter$)} {
		lappend tmpList {^\$.*}
	}
	if {[llength $tmpList]} {
		set filter(regexp) [join $tmpList |] 
	} else {
		set filter(regexp) {^.*}
	}
	
	foreach name [array name filter exclude:*] {
		unset filter($name)
	}
	if {$filter(opt:std)} {
		foreach sym $filter(standardExcludeList) {
			set filter(exclude:$sym) {}
		}
	}
	if {$filter(opt:cust)} {
		if {![catch {open $filter(opt:file)} fid]} {
			foreach sym [read $fid] {
				set filter(exclude:$sym) {}
			}
			close $fid
		} else {
			tk_messageBox -type ok -icon error -message "Cannot open custom symbol filter file"
		}
	}
}

set filter(opt)         {} 
set filter(opt:cust)    0
set filter(opt:file)    {} 
set filter(opt:filter$) 1
set filter(opt:filter_) 1
set filter(opt:psect)   1
set filter(opt:std)     1
set filter(exclPsect:\(abs\)) {}
set filter(exclPsect:bss) {}
set filter(exclPsect:data) {}
set filter(standardExcludeList) \
	[list  __Hbeforebss  __Hbss  __Hdata  __Hhighpage __Hlowpage \
		   __Hmidpage  __Htext   __Lbeforebss  __Lbss  __Ldata \
           __Lhighpage  __Llowpage  __Lmidpage  __Ltext \
    ]
    
buildFilters
