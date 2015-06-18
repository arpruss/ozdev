#======================================================================
#
# Tcl Source File -- Created with SAPIEN Technologies PrimalSCRIPT(TM)
#
# NAME: <filename>
#
# AUTHOR: Lawrence Chitty , n/a
# DATE  : 21/03/02
#
# COMMENT: <comment>
#
#======================================================================

proc serialDataProcess {} {
	global comms
	while (1) {
		if {[catch {read $comms(handle) 1} inChar]} {
			set comms(statusMsg) [fconfigure $comms(handle) -lasterror]
			incr comms(errCount)
			puts "Serial Err $comms(errCount) - $comms(statusMsg)"
			return
		}
		switch -- $inChar {
			"@" {
				set comms(buffer) {}
				set comms(charCount) 0
				break
			}
			{} {
				return
			}
			default {
				if {$comms(charCount) < 4} {
					append comms(buffer) $inChar
					if {([incr comms(charCount)] == 4) && ($comms(serialPause) == 0)} {
						catch {addGraphVal [expr {"0x$comms(buffer)"}]}
					}
				}
			}
		}
	}				
}


proc menus {} {
	global filter
	. configure -menu .menu
	menu .menu -activeborderwidth 1 -borderwidth 1 
    .menu add cascade -menu .menu.file -label File  
    menu .menu.file -tearoff 0
    .menu.file add command -label Open -command openMapFile
    .menu.file add command -label Reload -command {start $comms(fileName)} -state disabled	
    .menu.file add separator
    .menu.file add command -label Exit -command exit
    
    .menu add cascade -menu .menu.config -label Configure
    menu .menu.config -tearoff 0
    .menu.config add command -label Comms -command commSet
  	.menu.config add command -label Filters -command displayFilterScreen

    .menu add cascade -menu .menu.console -label Console
    menu .menu.console -tearoff 0
    .menu.console add command -label {Open Console} -command {console show}  
}


proc start {filename} {
	global symbols
	global graphVal
	global symTree
	global totalSamples
	global comms
	global options
	if {[catch {open $filename} fhandle]} {
		return
	}
	set totalSamples 0
	catch {unset graphVal}
	parseMap $fhandle
	close $fhandle
	Tree:clear .f.t
	update
	buildSymTreeList
	foreach symList $symTree {
		foreach {lib obj symbol addr} $symList {
			if {$lib != "nolib"} {set libext .lib} else {set libext {}}
			Tree:newitem .f.t "/$lib$libext" -graphTag  "$lib lib" -textTag "#$lib"
			Tree:newitem .f.t "/$lib$libext/$obj.obj" -graphTag  "$lib@$obj obj" -textTag "#$lib@$obj"
			Tree:newitem .f.t "/$lib$libext/$obj.obj/$symbol" -graphTag  "$lib@$obj@$symbol sym" -textTag "#$lib@$obj@$symbol"
			set graphVal($lib) 0
			set graphVal($lib@$obj) 0				
			set graphVal($lib@$obj@$symbol) 0
		}
	}
	Tree:buildwhenidle .f.t
	wm title . "Profiling - [file tail [file rootname $filename]]"
	set comms(mapLoaded) 1
	set comms(fileName) $filename
	.menu.file entryconfigure Reload -state normal
}

proc buildSymTreeList {} {
	global symbols
	global symTree
	global filter
	set lastPsect {}
	set symbols [lsort -integer -index 1 [lsort -index 0 $symbols]]
	foreach symList $symbols {
		foreach {symType addr symbol psect} $symList {
			if {[info exist filter(exclPsect:$psect)]} {
				continue
			}
			if {$symType == "lib"} {
				set lib $symbol
			} elseif {$symType == "obj"} {
				set obj $symbol
				if {"$symbol$psect" != $lastPsect} {
					lappend psectList $lib $obj $psect $addr
					set lastPsect $symbol$psect
				}
			} elseif {[regexp $filter(regexp) $symbol] \
					&& (![info exist filter(exclude:$symbol)])} {
				lappend symTree [list $lib $obj $symbol $addr]
				set psectCheck($addr) {}
			}
		}
	}
	if {$filter(opt:psect)} {
		foreach {lib obj symbol addr} $psectList {
			if {![info exist psectCheck($addr)]} {
				lappend symTree [list $lib $obj [string toupper $symbol] $addr]
			}
		}
		set symTree [lsort -integer -index 3 $symTree]
	}
}
		
	
		

proc Tree:clear {w} {
	global Tree
	foreach branch $Tree($w:/:children) { 
		Tree:delitem $w "/$branch"
	}
}

#
# Delete element $v from the tree $w.  If $v is /, then the widget is
# deleted.
#
#
# Delete element $v from the tree $w.  If $v is /, then the widget is
# deleted.
#
proc Tree:delitem {w v} {
  global Tree
  if {![info exists Tree($w:$v:open)]} return
  if {[string compare $v /]==0} {
    # delete the whole widget
    catch {destroy $w}
    foreach t [array names Tree $w:*] {
      unset Tree($t)
    }
  }
  foreach c $Tree($w:$v:children) {
    catch {Tree:delitem $w $v/$c}
  }
  unset Tree($w:$v:open)
  unset Tree($w:$v:children)
  unset Tree($w:$v:graphTag)
  unset Tree($w:$v:textTag)
  set dir [file dirname $v]
  set n [file tail $v]
  set i [lsearch -exact $Tree($w:$dir:children) $n]
  if {$i>=0} {
    set Tree($w:$dir:children) [lreplace $Tree($w:$dir:children) $i $i]
  }
  Tree:buildwhenidle $w
}

proc Tree:buildwhenidle w {
  global Tree
  if {![info exists Tree($w:buildpending)]} {
    set Tree($w:buildpending) 1
    after idle "catch \{Tree:build $w\}"
  }
}

proc serialOpen {} {
	global options
	global symTree
	global comms

	if {$options(comPort) == "" || $options(comBaud) == ""} {
		tk_messageBox -type ok -icon error -message {Com Port and/or Baud Not Set}
		return -1
	}
	if {$symTree == ""} {
		tk_messageBox -type ok -icon error -message {Symbol Map Not Loaded}
		return -1
	}
	if {[catch {open $options(comPort) r} comms(handle)]} {
		tk_messageBox -type ok -icon error -message "Could Not Open $options(comPort)"
		return -1
	}
	fconfigure $comms(handle) -mode "$options(comBaud),n,8,1" -blocking 0 -buffering none -translation binary
	fileevent $comms(handle) readable {serialDataProcess}
	set comms(charCount) 4
	set comms(errCount) 0
	startGraphUpdate	
}

proc updateGraph {} {
	global totalSamples
	global graphVal
	global graphx
	if ($totalSamples) {
		foreach item [array name graphVal {*}] { 
			if {$graphVal($item)} {
				set percent [expr {double($graphVal($item)) / $totalSamples}] 
				set coords [.f.t coords $item]
				set y1 [lindex $coords 1]
				set y2 [lindex $coords 3]
				set graphx2 [expr {$graphx + ceil(400.0 * $percent)}]
				.f.t coords $item $graphx $y1 $graphx2 $y2
				.f.t itemconfigure $item -width 0 
				.f.t coords "#$item" [expr {20 + $graphx2}]	[lindex [.f.t coords "#$item"] 1]
				.f.t itemconfigure "#$item" -text "[expr {int($percent*1000)/10.0}]% / $graphVal($item)"
			}
		}
	}
	.f.t itemconfigure lib -fill blue
	.f.t itemconfigure obj -fill green
	.f.t itemconfigure sym -fill red
}



proc resetGraph {} {
	global totalSamples
	global graphVal 
	global comms
	foreach item [array names graphVal] {
		set graphVal($item) 0
	}
	set totalSamples 0
	if [info exists comms(mapLoaded)] { 
		Tree:build .f.t
	}
}

proc snapShot {} {
	global snapShotCnt
	global graphVal
	global snapShot
	global totalSamples
	global comms
	incr snapShotCnt
	set snapShot($snapShotCnt) {}	
	foreach sym [array names graphVal *@*@*] {
		if {$graphVal($sym)} {
			lappend snapShot($snapShotCnt) [list [string map {@ { }} $sym] $graphVal($sym)]
		}
	}
	if {[llength $snapShot($snapShotCnt)] == 0} {
		tk_messageBox -type ok -icon error -message {Nothing to snapshot}
		incr snapShotCnt -1
		return
	}
	set snapShot(totalSamples$snapShotCnt)	$totalSamples
	set snapShot(date$snapShotCnt) [clock format [clock scan now] -format {%H:%M:%S   %d-%b-%y}]
	set snapShot(mapFile$snapShotCnt) $comms(fileName)
	set w ".snapShot$snapShotCnt"
	toplevel $w
	grid rowconfigure $w 1 -weight 1
	grid columnconfigure $w 0  -weight 1
	
	frame $w.f1
	grid columnconfigure $w.f1 {0 1} -weight 1	
	button $w.f1.b1 -text {Sort by symbol} -command "snapShotDisplay .snapShot$snapShotCnt.c $snapShotCnt 0"
	button $w.f1.b2 -text {Sort by frequency} -command "snapShotDisplay .snapShot$snapShotCnt.c $snapShotCnt 1"
	grid $w.f1 -columnspan 2 -sticky ew
	grid $w.f1.b1 $w.f1.b2 -sticky ew
	
	canvas $w.c -bg white -width 600 -height 400 -yscrollcommand "$w.sb set"
	scrollbar $w.sb -orient vertical -command "$w.c yview" 
	grid $w.c $w.sb -sticky nsew
	set text "Total samples - $snapShot(totalSamples$snapShotCnt)    \
			 $snapShot(mapFile$snapShotCnt)     \
			 $snapShot(date$snapShotCnt)"
	label $w.l  -text $text -anchor w -relief sunken
	grid $w.l -columnspan 2 -sticky ew -pady 2
	snapShotDisplay  .snapShot$snapShotCnt.c $snapShotCnt 1 
}
	
proc snapShotDisplay {w v sortIndex} {
	puts "$w   $v  $sortIndex"
	global snapShot
	global Tree
	set x 5
	$w delete all
	if {[info exist snapShot($v)]} {
		if {$sortIndex == 0} {  
			set snapShot($v) [lsort -index 0 $snapShot($v)]
		} else {
			set snapShot($v) [lsort -index 1 -integer -decreasing $snapShot($v)]
		}
		for {set c 0} {$c < 3} {incr c} {
			set y 10
			foreach symList $snapShot($v) {
				foreach {sym val}  $symList {
					$w create text $x $y -text [lindex $sym $c] -font $Tree(font) -anchor w
					incr y 19
				}
			}
			set x [expr {[lindex [$w bbox all] 2] + 10}]
		}
		set y 11
		set s $snapShot(totalSamples$v)
		foreach symList $snapShot($v) {
			foreach {sym val}  $symList {
				set percent [expr {double($val) / $s}] 
				set x2 [expr {$x + ceil(400.0 * $percent)}]
				$w create rectangle $x [expr {$y-4}] $x2 [expr {$y+4}] -fill red -width 0
				$w create text [expr {$x2 + 20}] $y -text "[expr {int($percent*1000)/10.0}]% / $val" -anchor w
			incr y 19
			}
		}
		bind $w <Destroy> "destroySnapshot $v" 
	}
}


proc destroySnapshot {v} {
	global snapShot
	unset snapShot($v)
	unset snapShot(totalSamples$v)
	unset snapShot(date$v)
	unset snapShot(mapFile$v)	
	
	
}
	 
proc drawScreen {} {
	grid rowconfigure . {1} -weight 1
	grid columnconfigure . 0  -weight 1

	frame .b 
	checkbutton .b.serialControl -text {Serial On/Off} -indicator 0 -selectcolor SystemButtonFace\
			-variable ::comms(serialState) -command {serialControl .b.serialControl}
	checkbutton .b.serialPause -text {Pause} -indicator 0 -selectcolor SystemButtonFace\
			-variable ::comms(serialPause)
	button .b.resetGraph -text {Reset Graph} -command resetGraph
	button .b.snapShot -text {Snapshot} -command snapShot
	grid .b -sticky ew
	grid columnconfigure .b {0 1 2 3}  -weight 1
	grid .b.serialControl  .b.serialPause .b.resetGraph .b.snapShot -sticky ew
	
	frame .f  
	
	Tree:create .f.t  -yscrollcommand {.f.sb set} -width 600 -height 400
	scrollbar .f.sb -orient vertical -command {.f.t yview}
	grid rowconfigure .f 0 -weight 1
	grid columnconfigure .f 0  -weight 1	
	grid .f -sticky nsew
	grid .f.t .f.sb -sticky nsew
	
	label .s -relief sunken
	label .s.s1 -text Samples 
	label .s.s2 -textvariable totalSamples -width 10
	grid .s -sticky w -pady 2
	pack .s.s1 .s.s2 -side left
}	
			
proc loadImage file {
	global image
	if {![info exist image($file)]} {
		set image($file) [image create photo -file $file]
	}
	return  $image($file)
}
 
source ozProfFilters.tcl
set comms(fileName) {}
set options(comPort) com3
set options(comBaud) 19200
set snapShotCnt 0
after 50 console hide
