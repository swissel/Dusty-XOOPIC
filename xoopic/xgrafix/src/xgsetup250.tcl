source [info library]/init.tcl
if { $tk_version < 8.4 } { source $tk_library/tk.tcl }

source $XGrafixPath/xginit250.tcl
source $XGrafixPath/xgdialogs250.tcl

set tcl_interactive 0

proc Run { } \
{
	global WasRunning
	
	if { $WasRunning == 0 } {
		set WasRunning 1
		C_Run
		.lbframe.run configure -text Stop
	} else {
		set WasRunning 0
		C_Stop
		.lbframe.run configure -text Run
	}
}

proc InitMainMenu { WasInputFileGiven File } \
{
	global time
	global WaitVariable
	global XGrafixIcon
	global defaultMainMenuX
	global defaultMainMenuY
	global defaultMainMenuWidth
	global WasRunning
	global InputFile

        append WinTitle "Control Panel"
        wm title . $WinTitle
        wm iconbitmap . $XGrafixIcon
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol . WM_DELETE_WINDOW [list PopupQuit]

	set InputFile $File

	#########################
	# The Label and Buttons
	
	frame .lbframe
        frame .lbframe.inputf
        label .lbframe.inputf.label1 -text  [expr {([string length $InputFile] < 17) ? "" : "..."}] -anchor e -width [expr {([string length $InputFile] < 17) ? 0 : 3}]
label .lbframe.inputf.label2 -text $InputFile -anchor e -width [expr {([string length $InputFile] < 17) ? [string length $InputFile] : 17}]
        pack .lbframe.inputf.label1 .lbframe.inputf.label2 -side left

	frame .lbframe.lframe -relief ridge -bd 2
	label .lbframe.lframe.label1 -text "Time =" -width 5 \
		-anchor w
	label .lbframe.lframe.label2 -textvariable time -width 14 \
		-anchor w
	pack .lbframe.lframe.label1 .lbframe.lframe.label2 -side left

	button .lbframe.diags -padx 0 -pady 0 -highlightthickness 0 -text "Close Diagnostics" \
		-command { PopupDiagWin }
	button .lbframe.run -padx 0 -pady 0 -highlightthickness 0 -text Run -command { Run }
	button .lbframe.step -padx 0 -pady 0 -highlightthickness 0 -text Step -command { C_Step }
	button .lbframe.save -padx 0 -pady 0 -highlightthickness 0 -text Save -command { PopupSave }
	button .lbframe.quit -padx 0 -pady 0 -highlightthickness 0 -text Quit -command { PopupQuit }
	button .lbframe.special -padx 0 -pady 0 -highlightthickness 0 -text Special -command { C_Special }
	pack .lbframe.inputf .lbframe.lframe .lbframe.diags .lbframe.run \
		.lbframe.step .lbframe.save -fill both \
		-expand true -ipady 3

	if { $WasInputFileGiven == 1 } {
		button .lbframe.view -padx 0 -pady 0 -highlightthickness 0 -text "Open Input File" -command \
			{ ViewInputFile }
		pack .lbframe.view -fill both -expand true -ipady 3
	}

	pack .lbframe.quit -fill both -side bottom -expand true -ipady 3
	pack .lbframe -side left -fill y

	CreateRescaleDialog 3D
	CreateRescaleDialog 2D
	CreatePrintDialog
	Create3dDialog
	CreateSaveDialog
	CreateQuitDialog
	CreateListBox $defaultMainMenuX $defaultMainMenuY

	bind . <Enter> { focus %W; break}
}

proc ViewInputFile { } \
{
	global InputFile
	global InputFileCreated
	global WasRunning

	if { $InputFileCreated == 0 } {
		if { $WasRunning == 1 } {
			C_Stop
		}
		CreateInputFileDialog
		.lbframe.view configure -text "Close Input File"
		update idletasks
		if { $WasRunning == 1 } {
			C_Run
		}
	} else {
		if { [ wm state .inputfile ] == "withdrawn" } {
			wm deiconify .inputfile
			raise .inputfile
			.lbframe.view configure -text "Close Input File"
		} else {
			wm withdraw .inputfile
			.lbframe.view configure -text "Open Input File"
		}
	}
}

proc ScrollProc args  \
{
     #C_printf $args
     #C_printf [llength $args]
     if { [llength $args] == "3" } {
         set a [ lindex $args 0 ]
         set b [ lindex $args 1 ]
         set c [ lindex $args 2 ]
	 #C_printf $a
	 #C_printf $b
	 #C_printf $c
	 .list.lframe.openbox yview $a $b $c
         .list.lframe.listbox yview $a $b $c
     } else {
         set a [ lindex $args 0 ]
         set b [ lindex $args 1 ]
         #C_printf $a
	 #C_printf $b
	 .list.lframe.openbox yview $a $b
         .list.lframe.listbox yview $a $b
     }
 }
proc EntryKeyPressProc { key1 key2 } \
{
	switch $key1 {
	"Return" 	{ .list.buttons.open invoke ; return }
	"Up"            { ListboxScroll single up ; return }
	"Down"          { ListboxScroll single down ; return }
"Prior" { ListboxScroll page prior ; return }
"Next" { ListboxScroll page next ; return }
	"Shift_L"	-
	"Shift_R"	-
	"Control_L"	-
	"Control_R"	-
	"Escape"	-
	"Tab"		{ return }
	"Left"		{ .list.top.entry icursor [expr [.list.top.entry \
				index insert] - 1] 
			   return }
	"Right"		{ .list.top.entry icursor [expr [.list.top.entry \
				index insert] + 1]
			  return }
	"Delete" 	-
	"BackSpace"	{ .list.top.entry delete [expr [.list.top.entry \
				index insert] - 1]
			  ListboxSearch
			  return }

	}

#	.list.top.entry insert index $key2
	.list.top.entry insert insert $key2
        ListboxSearch
}


##############################
# The listbox of diagnostics 

proc CreateListBox { WinPosX WinPosY } {
	global XGrafixIcon

	toplevel .list
	wm title .list Diagnostics
	wm minsize .list 0 0
	wm iconbitmap .list $XGrafixIcon
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol .list WM_DELETE_WINDOW [list PopupDiagWin]

	####################
	# The search entry  

	frame .list.top -relief raised -bd 2
	label .list.top.label -text Search:
	entry .list.top.entry -highlightthickness 0 -relief sunken -textvariable SearchVar

	pack .list.top.label -side left -pady 3
	pack .list.top.entry -side right -fill x -expand true -pady 3
	pack .list.top -fill both
	
	focus .list.top.entry
	bind .list.top.entry <KeyPress> { EntryKeyPressProc %K %A; break }
	#bind .list.top.entry <KeyPress> { ListboxSearch }
	bind .list.top.entry <Control-KeyPress> { 
		EntryControlProc %W %K 
		if { "%K" == "d" } {
			ListboxSearch
		}; break
	}

	#############################
	# The listbox and scrollbar

	frame .list.lframe
##	listbox .list.lframe.openbox -highlightthickness 0 -relief raised -yscroll { .list.lframe.scroll set } -geometry 2x10
        #listbox .list.lframe.openbox -highlightthickness 0 -relief raised -yscroll { .list.lframe.scroll set } -width 2 -height 10
        listbox .list.lframe.openbox -highlightthickness 0 -relief raised -width 2 -height 10
	#bind .list.lframe.openbox <1> { }
	#bind .list.lframe.openbox <B1-Motion> { }
	#bind .list.lframe.openbox <B2-Motion> { }	

	listbox .list.lframe.listbox -selectmode extended -highlightthickness 0 -relief raised -yscroll { .list.lframe.scroll set }
	#bind .list.lframe.listbox <B2-Motion> { }

	scrollbar .list.lframe.scroll -highlightthickness 0 -relief raised \
 -command { ScrollProc }
        bind .list.lframe.scroll <B1-Motion> { tkScrollDragCuston %W %x %y; break}
        bind .list.lframe.scroll <B2-Motion> { tkScrollDragCuston %W %x %y; break}
	bind .list.lframe.scroll <Home> { break }
	bind .list.lframe.scroll  <End> { break }
	pack .list.lframe.openbox -side left -fill y
	pack .list.lframe.listbox -side left -fill both -expand true
	pack .list.lframe.scroll -side right -fill y
	pack .list.lframe -fill both -expand true

	frame .list.buttons -relief raised -bd 2

	#################################
	# The open and closeall buttons

	button .list.buttons.open -padx 0 -pady 0 -highlightthickness 0 -text "Open" -command { PopupDiag }
	button .list.buttons.closeall -padx 0 -pady 0 -highlightthickness 0 -text "Close All" \
		-command { CloseAllDiags }

	pack .list.buttons.closeall .list.buttons.open -side right -padx 3 \
		-pady 3 -ipadx 3
	pack .list.buttons -fill x

	bind .list.lframe.listbox <Double-1> { PopupDiag; break }
	bind .list.lframe.listbox <3> [bind Listbox <1>]
	bind .list.lframe.listbox <Double-3> { CloseDiag; break }
	bind .list.lframe.listbox <Shift-1> { }
	bind .list <Enter> { focus .list.top.entry; break }
}

proc ListboxScroll { type direction } \
{
	set scrollUnits [ .list.lframe.scroll get ]
	set totalUnits [ lindex $scrollUnits 0 ]
	set windowUnits [ lindex $scrollUnits 1 ]
	set firstUnit [ lindex $scrollUnits 2 ]
	set lastUnit [ lindex $scrollUnits 3 ]
	#if { [ .list.lframe.listbox curselection ] == "" } {
	#	.list.lframe.listbox select adjust 0
	#}

	if {$type == "single"} {
		set increment 1
	} else {
		set increment $windowUnits
	}

	switch $direction {
	"down" { .list.lframe.listbox selection clear 0 [.list.lframe.listbox size]
	        .list.lframe.listbox activate [expr \
		[.list.lframe.listbox index active] + $increment] 
	        .list.lframe.listbox selection set active
	        .list.lframe.listbox yview active 
		.list.lframe.openbox yview [.list.lframe.listbox index active]}
	"up" { .list.lframe.listbox selection clear 0 [.list.lframe.listbox size] 
		.list.lframe.listbox activate [expr \
		[.list.lframe.listbox index active] -  $increment]
                .list.lframe.listbox selection set active
		.list.lframe.listbox yview active
		.list.lframe.openbox yview [.list.lframe.listbox index active]}
	"prior" {.list.lframe.listbox yview moveto [expr [ expr 2*[ lindex [ \
		.list.lframe.listbox yview] 0] ] - [ lindex [ \
		.list.lframe.listbox yview] 1] ] 
	        .list.lframe.openbox yview moveto [expr [ expr 2*[ lindex [ \
		.list.lframe.openbox yview] 0] ] - [ lindex [ \
		.list.lframe.openbox yview] 1] ] }
	"next" { .list.lframe.listbox yview moveto [ lindex [ \
		.list.lframe.listbox yview] 1] 
	        .list.lframe.openbox yview moveto [ lindex [ \
		.list.lframe.openbox yview] 1] }
    }
}


proc ListboxSearch { } \
{
	global SearchVar

#C_printf [.list.lframe.listbox curselection]
#foreach index [.list.lframe.listbox curselection] {
#    .list.lframe.listbox selection clear  $index
#}
	for {set index 0} {$index < [.list.lframe.listbox size]} {incr index} {
		if { [string match $SearchVar* [.list.lframe.listbox get $index]] == 1 } {
			foreach index2 [.list.lframe.listbox curselection] {
			    .list.lframe.listbox selection clear  $index2
			}
			# changed "from" to "set" below  --- djc for tk4
		        # .list.lframe.listbox select clear end
		        .list.lframe.listbox select set $index
			.list.lframe.listbox yview $index
			.list.lframe.openbox yview $index
			return
		}
	}
}

######################################################
# Sets up data structure for each diagnostics window

proc SetupWindow { WinName State WinPosULX WinPosULY \
	WinType xMin xMax xAuto yMin yMax yAuto zMin zMax zAuto \
	PSFile theta phi shading color grid xlog ylog zlog DumpFile xlabel \
	ylabel } \
{
	global WinData
	global SaveFile
	global default3DHeight
	global default3DWidth
	global defaultVectorHeight
	global defaultVectorWidth
	global defaultWinHeight
	global defaultWinWidth
	global TwoD
	global ThreeD
	global VectorD
	global openWindowsList

	switch $WinType \
	$TwoD { set WinWidth $defaultWinWidth
		set WinHeight $defaultWinHeight } \
	$ThreeD { set WinWidth $default3DWidth
		  set WinHeight $default3DHeight } \
	$VectorD { set WinWidth $defaultVectorWidth
		   set WinHeight $defaultVectorHeight }

	append Window . [string tolower $WinName]
	append Geometry $WinWidth x $WinHeight + $WinPosULX + $WinPosULY
	set WinData($Window,geometry) $Geometry
	set WinData($Window,type) $WinType
	set WinData($Window,title) $WinName
	set WinData($Window,open) 0
	set WinData($Window,xMin) $xMin
	set WinData($Window,xMax) $xMax
	set WinData($Window,xAuto) $xAuto
	set WinData($Window,xLinLog) $xlog		  
	set WinData($Window,yMin) $yMin
	set WinData($Window,yMax) $yMax
	set WinData($Window,yAuto) $yAuto
	set WinData($Window,yLinLog) $ylog		  
	set WinData($Window,zMin) $zMin
	set WinData($Window,zMax) $zMax
	set WinData($Window,zAuto) $zAuto
	set WinData($Window,zLinLog) $zlog		  
	set WinData($Window,origxMin) $xMin
	set WinData($Window,origxMax) $xMax
	set WinData($Window,origxAuto) $xAuto
	set WinData($Window,origxLinLog) $xlog		  
	set WinData($Window,origyMin) $yMin
	set WinData($Window,origyMax) $yMax
	set WinData($Window,origyAuto) $yAuto
	set WinData($Window,origyLinLog) $ylog		  
	set WinData($Window,origzMin) $zMin
	set WinData($Window,origzMax) $zMax
	set WinData($Window,origzAuto) $zAuto
	set WinData($Window,origzLinLog) $zlog		  
	set WinData($Window,trace) 0
	set WinData($Window,PSFile) $PSFile
	set WinData($Window,plot_title) $WinName
	set WinData($Window,printType) eps
	set WinData($Window,printMode) overwrite
	set WinData($Window,printLimit) 0
	set WinData($Window,printPeriod) 0
	set WinData($Window,printNumber) 1
	set WinData($Window,printSetStep) 0
	set WinData($Window,xCoord) 0
	set WinData($Window,yCoord) 0
	set WinData($Window,theta) $theta
	set WinData($Window,phi) $phi
	set WinData($Window,shading) $shading
	set WinData($Window,color) $color
	set WinData($Window,grid) $grid
	set WinData($Window,xlabel) $xlabel
	set WinData($Window,ylabel) $ylabel

	set SaveFile $DumpFile

	AddToList $Window

	if { $State == "open" } {
		set WinData($Window,created) 1
		CreateWindow $Window
		set WinData($Window,open) 1
		lput $Window
	} else {
		set WinData($Window,created) 0
	}
}

proc Set3dCross { WinName xstart xend ystart yend } \
{
	global WinData
	
	append Window . [string tolower $WinName]
#C_printf $xend
#C_printf $xstart
	$Window.middle.x.scale configure -from $xstart
	$Window.middle.x.scale configure -to [expr $xend - 1]
	if { [lindex [$Window.middle.x.scale configure -from ] 4] < $xstart } {
		$Window.middle.x.scale set $xstart
	} elseif {[lindex [$Window.middle.x.scale configure -to] 4] >= $xend} {
		$Window.middle.x.scale set [expr $xend - 1]
	}

	$Window.middle.y.scale configure -from $ystart
	$Window.middle.y.scale configure -to [expr $yend - 1]
	if { [lindex [$Window.middle.y.scale configure -from ] 4] < $ystart } {
		$Window.middle.y.scale set $ystart
	} elseif {[lindex [$Window.middle.y.scale configure -to] 4] >= $yend} {
		$Window.middle.y.scale set [expr $yend - 1]
	}
}	

##################################
# Procedure to popup diagnostics  

proc PopupDiag { } \
{
	global WinData
	
	#C_printf [selection own] 
        if { [selection own] == "" } {
		return
	}
	#C_printf [selection get]

	set WindowsList [ split [selection get] \n]
	foreach WinName $WindowsList {
		set WinName [string trimright [string trimleft \
			$WinName \{] \}]
		set Window ""
		append Window . [string tolower $WinName]
	    #C_printf $Window
		Open $Window
	}
}

####################################
# Procedure to close diagnostics

proc CloseDiag { } \
{
	global WinData

	if { [selection own] == "" } {
		return
	}

	set Window [selection get]
	set WinName [string trimright [string trimleft $Window \{] \}]

	append WinPath . [string tolower $WinName]
	Close $WinPath
}
	

#####################################
# Adds a diagnostic to the listbox

proc AddToList { Window } \
{
	global WinData

	set index [Search $Window]
	set Title $WinData($Window,title)

	.list.lframe.listbox insert $index $Title
	.list.lframe.openbox insert end " "

	set WinData($Window,index) $index

	set End [expr [.list.lframe.listbox size] - 1]
	
	for {set i [expr $index + 1]} {$i<=$End} {incr i} {
		set Window ""
		append Window . [string tolower [.list.lframe.listbox get $i]]
		incr WinData($Window,index)
	}
}


proc Search { Window } \
{
	global WinData

	set Begin 0
	set End [expr [.list.lframe.listbox size] - 1]
	set Title $WinData($Window,title)

	while { $Begin <= $End } {
		set Middle [expr ($End+$Begin)/2]
		switch \"[string compare $Title [.list.lframe.listbox get $Middle]]\" {
		\"-1\" { set End [expr $Middle - 1] }
		\"0\" { return $Begin }
		\"1\" { set Begin [expr $Middle + 1] }
		}
	}
	return $Begin
}

########################
# Create the Tk Window  

proc CreateWindow { Window } \
{
	global WinData
	global XGrafixIcon
	global ThreeD
	global windowsList
	
	########################################
	# Create and place the toplevel window

	set Type $WinData($Window,type)
	lappend windowsList $Window

	toplevel $Window

	wm minsize $Window 100 100
	wm title $Window $WinData($Window,title)
	wm geometry $Window $WinData($Window,geometry)
	wm iconbitmap $Window $XGrafixIcon
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $Window WM_DELETE_WINDOW [list Close $Window]
   #tkwait visibility $Window

	######################
	# Create the buttons

        frame $Window.bframe -relief raised -bd 2
        
        button $Window.bframe.close -padx 0 -pady 0 -highlightthickness 0 -text Close -command \
		[ list Close $Window ]
        button $Window.bframe.rescale -padx 0 -pady 0 -highlightthickness 0 -text Rescale -command \
		[ list PopupRescale $Window ]
        if { $Type != $ThreeD } {
                button $Window.bframe.trace -padx 0 -pady 0 -highlightthickness 0 -text Trace -command \
			[list Trace $Window]
		if { $WinData($Window,xAuto) == 1 || $WinData($Window,yAuto) == 1 } {
			$Window.bframe.trace configure -state disabled
		}
        }
        button $Window.bframe.print -padx 0 -pady 0 -highlightthickness 0 -text Print -command \
		[list PopupPrint $Window]
	button $Window.bframe.crosshair -padx 0 -pady 0 -highlightthickness 0 -text "Crosshair" -command \
		[list PopupCrosshair $Window]
        if { $Type != $ThreeD } {
		frame $Window.bottom -bd 2 -relief raised
		frame $Window.bottom.xframe -relief ridge -bd 2
		label $Window.bottom.xframe.x1 -text \
			"$WinData($Window,xlabel):" -anchor w
		label $Window.bottom.xframe.x2 -textvariable \
			WinData($Window,xCoord) -anchor w
		frame $Window.bottom.yframe -relief ridge -bd 2
		label $Window.bottom.yframe.y1 -text "Y:" -anchor w
		label $Window.bottom.yframe.y2 -textvariable \
			WinData($Window,yCoord) -anchor w
	        pack $Window.bframe.close $Window.bframe.rescale \
        	        $Window.bframe.trace $Window.bframe.print \
			$Window.bframe.crosshair \
			-side left -ipadx 3 -ipady 3 \
	                -padx 3 -pady 3
		pack $Window.bottom.xframe.x1 $Window.bottom.xframe.x2 \
			-side left -padx 3
		pack $Window.bottom.yframe.y1 $Window.bottom.yframe.y2 \
			-side left -padx 3
		pack $Window.bottom.yframe $Window.bottom.xframe -side right \
			-padx 3 -pady 3
        }
        if { $Type == $ThreeD } {
                button $Window.bframe.3dview -padx 0 -pady 0 -highlightthickness 0 -text "3D View" -command \
			[list Popup3d $Window]
	        pack $Window.bframe.close $Window.bframe.rescale \
        	        $Window.bframe.print $Window.bframe.3dview \
			$Window.bframe.crosshair -side left -ipadx 3 \
			-ipady 3 -padx 3 -pady 3
        }
        pack $Window.bframe -fill x
        
        ############################################
        # Create the label where the plot is to be

	frame $Window.middle
	if { $Type != $ThreeD } {
		label $Window.middle.label -background gray30 -cursor crosshair
		bind $Window.middle.label <1> [list UpdateCrosshair \
			$Window %x %y]
		bind $Window.middle.label <B1-Motion> [list UpdateCrosshair \
			$Window %x %y]
		bind $Window.middle.label <2> [list MiddleDown $Window %x %y ]
		bind $Window.middle.label <B2-Motion> [list StretchBox \
			 $Window %x %y ]
		bind $Window.middle.label <ButtonRelease-2> [list MiddleUp \
			$Window %x %y ]
		bind $Window.middle.label <3> [list RestoreWinParams $Window ]
	} else {
		label $Window.middle.label -background gray30
	}
        pack $Window.middle.label -fill both -expand true -side left
	pack $Window.middle -fill both -expand true

	if { $Type == $ThreeD } {
		frame $Window.middle.x -relief raised -bd 2
		label $Window.middle.x.label -text "X Axis"
		scale $Window.middle.x.scale -highlightthickness 0 -showvalue 0 -command \
			[list Update3dCrosshair $Window ]
		pack $Window.middle.x.label -fill x
		pack $Window.middle.x.scale -fill y -expand true -pady 3

		frame $Window.middle.y -relief raised -bd 2
		label $Window.middle.y.label -text "Y Axis"
		scale $Window.middle.y.scale -highlightthickness 0 -showvalue 0 -command \
			[list Update3dCrosshair $Window ]
		pack $Window.middle.y.label -fill x
		pack $Window.middle.y.scale -fill y -expand true -pady 3

		frame $Window.bottom -relief raised -bd 2		
		frame $Window.bottom.x -relief ridge -bd 2
		label $Window.bottom.x.label1 -text \
			"$WinData($Window,xlabel):" -anchor w
		label $Window.bottom.x.label2 -textvariable \
			WinData($Window,xCoord) -anchor w
		pack $Window.bottom.x.label1 -side left
		pack $Window.bottom.x.label2 -fill x -expand true -side left
		pack $Window.bottom.x -fill x -expand true -side left \
			-padx 3 -pady 3

		frame $Window.bottom.y -relief ridge -bd 2
		label $Window.bottom.y.label1 -text \
			"$WinData($Window,ylabel):" -anchor w
		label $Window.bottom.y.label2 -textvariable \
			WinData($Window,yCoord) -anchor w
		pack $Window.bottom.y.label1 -side left
		pack $Window.bottom.y.label2 -fill x -expand true -side left
		pack $Window.bottom.y -fill x -expand true -side left \
			-padx 3 -pady 3

		frame $Window.bottom.z -relief ridge -bd 2
		label $Window.bottom.z.label1 -text "Z:" -anchor w
		label $Window.bottom.z.label2 -textvariable \
			WinData($Window,zCoord) -anchor w
		pack $Window.bottom.z.label1 -side left
		pack $Window.bottom.z.label2 -fill x -expand true -side left
		pack $Window.bottom.z -fill x -expand true -side left \
			-padx 3 -pady 3

		bind $Window <Enter> { focus %W; break }
		bind $Window <Left> { \
			set pathname [string trimleft %W "."]
		        set rootname [string range $pathname 0 [expr \
				[string first "." $pathname] -1] ]
		        set rootname .$rootname
			$rootname.middle.x.scale set [expr \
			[$rootname.middle.x.scale get ] - 1 ]; break }
		bind $Window <Right> { \
			set pathname [string trimleft %W "."]
		        set rootname [string range $pathname 0 [expr \
				[string first "." $pathname] -1] ]
		        set rootname .$rootname
			$rootname.middle.x.scale set [expr \
			[$rootname.middle.x.scale get ] + 1 ]; break }
		bind $Window <Up> { \
			set pathname [string trimleft %W "."]
		        set rootname [string range $pathname 0 [expr \
				[string first "." $pathname] -1] ]
		        set rootname .$rootname
			$rootname.middle.y.scale set [expr \
			[$rootname.middle.y.scale get ] - 1 ]; break }
		bind $Window <Down> { \
			set pathname [string trimleft %W "."]
		        set rootname [string range $pathname 0 [expr \
				[string first "." $pathname] -1] ]
		        set rootname .$rootname
			$rootname.middle.y.scale set [expr \
			[$rootname.middle.y.scale get ] + 1 ]; break } 

	} else {
		bind $Window <Enter> { focus %W; break }
	}
}

proc StretchBox { Window x y } \
{
	global WinData

	#if { $WinData($Window,type) == $ThreeD } {
	 #   set coords [ C_UpdateCrosshair $WinData($Window,index) $x $y ]
	  #  set xTemp2 [lindex $coords 0]
	   # set yTemp2 [lindex $coords 1]
	    #
	 #   C_StretchBox $WinData($Window,index) $xTemp2 $yTemp2
	#}
	#else {
	 set coords [ C_UpdateCrosshair $WinData($Window,index) $x $y ]
	    set xTemp2 [lindex $coords 0]
	    set yTemp2 [lindex $coords 1]
	    
	    C_StretchBox $WinData($Window,index) $xTemp2 $yTemp2   
	#}
}

proc MiddleDown { Window x y } \
{
	global WinData
	global xTemp1
	global yTemp1

	C_SetCrosshairParameters $WinData($Window,index)
	set coords [ C_UpdateCrosshair $WinData($Window,index) $x $y ]
	if { $coords == "" } {
		set xTemp1 0
		set yTemp1 0
	} else {
		set xTemp1 [lindex $coords 0]
		set yTemp1 [lindex $coords 1]
	}

	C_DefineUL $WinData($Window,index) $xTemp1 $yTemp1
}

proc MiddleUp { Window x y } \
{
	global WinData
	global xTemp1
	global yTemp1
	global xTemp2
	global yTemp2

	set coords [ C_UpdateCrosshair $WinData($Window,index) $x $y ]
	set xTemp2 [lindex $coords 0]
	set yTemp2 [lindex $coords 1]

	if { $xTemp1 < $xTemp2 } {
		set WinData($Window,xMin) $xTemp1
		set WinData($Window,xMax) $xTemp2
	} elseif { $xTemp1 > $xTemp2 } {
		set WinData($Window,xMin) $xTemp2
		set WinData($Window,xMax) $xTemp1
	} else {
		return
	}

	if { $yTemp1 < $yTemp2 } {
		set WinData($Window,yMin) $yTemp1
		set WinData($Window,yMax) $yTemp2
	} elseif { $yTemp1 > $yTemp2 } {
		set WinData($Window,yMin) $yTemp2
		set WinData($Window,yMax) $yTemp1
	} else {
		return
	}

	set WinData($Window,xAuto) 0
	set WinData($Window,yAuto) 0
	$Window.bframe.trace configure -state normal

	C_Rescale $WinData($Window,index) $WinData($Window,xMin) \
		$WinData($Window,xMax) $WinData($Window,xAuto) \
		$WinData($Window,xLinLog) $WinData($Window,yMin) \
		$WinData($Window,yMax) $WinData($Window,yAuto) \
		$WinData($Window,yLinLog) $WinData($Window,zMin) \
		$WinData($Window,zMax) $WinData($Window,zAuto) \
		$WinData($Window,zLinLog)
}

proc RestoreWinParams { Window } {
	global WinData

	set WinData($Window,xAuto) 1
	set WinData($Window,yAuto) 1

	C_Rescale $WinData($Window,index) $WinData($Window,origxMin) \
		$WinData($Window,origxMax) $WinData($Window,origxAuto) \
		$WinData($Window,origxLinLog) $WinData($Window,origyMin) \
		$WinData($Window,origyMax) $WinData($Window,origyAuto) \
		$WinData($Window,origyLinLog) $WinData($Window,origzMin) \
		$WinData($Window,origzMax) $WinData($Window,origzAuto) \
		$WinData($Window,origzLinLog)

	if {$WinData($Window,origxAuto)==1 || $WinData($Window,origyAuto)==1} {
		$Window.bframe.trace configure -state disabled
	}
}

proc Update3dCrosshair { Window x } \
{
	global WinData
	
	#C_printf "calling C_3dCross from Update3dCrosshair"
        C_RefreshGraphics $WinData($Window,index)
        C_3dCross $WinData($Window,index) [$Window.middle.x.scale get] \
		[$Window.middle.y.scale get]
}

proc Update3dCrosshairValues { WinName x y z } \
{
	global WinData

	append Window . [string tolower $WinName]

	set WinData($Window,xCoord) $x
	set WinData($Window,yCoord) $y
	set WinData($Window,zCoord) $z
        #update
}
	

proc PopupCrosshair { Window } \
{
	global WinData
	global WasRunning
	global ThreeD

	if { $WinData($Window,type) == $ThreeD } {
	    if {[lindex [$Window.bframe.crosshair configure -relief] 4] == "raised"} {
		$Window.bframe.crosshair configure -relief sunken
		pack $Window.middle.x $Window.middle.y -fill both -expand true
		pack $Window.bottom -fill x
		C_3dCrossOn $WinData($Window,index)
		update
		Update3dCrosshair $Window 0
	    } else {
		$Window.bframe.crosshair configure -relief raised
		C_3dCrossOff $WinData($Window,index)
		pack forget $Window.middle.x $Window.middle.y $Window.bottom 
	    }
	} else {
	    if {[lindex [$Window.bframe.crosshair configure -relief] 4] == "raised"} {
		$Window.bframe.crosshair configure -relief sunken
		pack $Window.bottom -fill x
		update
	    } else {
		$Window.bframe.crosshair configure -relief raised
		pack forget $Window.bottom 
	    }
	}
	update idletasks
}
    
########################################################################
# Calls a C Procedure to refresh the graphics for each open diagnostic
		
proc DoGraphics {} \
{
	global WinData
	global windowsList
	global theCurrentStep
	global WasRunning

	incr theCurrentStep

        foreach Window $windowsList {
		if [ expr \
		{ $WinData($Window,open) && [wm state $Window]=="normal" } || \
		{ [ShouldRefresh $Window]=="1" } ] {
			#C_printf "c_refreshing Window"
	               C_RefreshGraphics $WinData($Window,index)
		}

		if { $WinData($Window,printLimit) != 0 && \
		[expr [expr $theCurrentStep - $WinData($Window,printSetStep)] \
		% $WinData($Window,printPeriod)] == 0 } {
				Print $Window
			incr WinData($Window,printLimit) -1
		}
	}
}	

proc ShouldRefresh { Window } \
{
	global WinData

	if { $WinData($Window,xAuto)==0 && $WinData($Window,yAuto)==0 \
	&& $WinData($Window,trace)==1 && $WinData($Window,printLimit) > 0 \
	&& $WinData($Window,printType)=="xpm" } {
		return 1
	} else {
		return 0
	}
}

############################
# Updates the time counter  

proc UpdateTime { newTime } \
{
	global time
	set time $newTime
}

############################################
# Calls a C procedure to turn on/off trace

proc Trace { Window } \
{
	global WinData

	if { $WinData($Window,trace)==0 } {
		$Window.bframe.trace configure -relief sunken
		set WinData($Window,trace) 1
	} else {
		$Window.bframe.trace configure -relief raised
		set WinData($Window,trace) 0
	}

	C_Trace $WinData($Window,index)
}

################################################
# Procedures to find the crosshair coordinates 

proc UpdateCrosshair { Window x y } \
{
	global WinData

	C_SetCrosshairParameters $WinData($Window,index)
	set coords [ C_UpdateCrosshair $WinData($Window,index) $x $y ]
	if { $coords != "" } {
		set WinData($Window,xCoord) [lindex $coords 0]
		set WinData($Window,yCoord) [lindex $coords 1]
	}
}

###############################
# Closes all open diagnostics  

proc CloseAllDiags { } \
{
	global openWindowsList

	foreach Window $openWindowsList {
		Close $Window
	}
}

################################################
# Sets up data structure for Special variables 

proc SetUpNewVar { index varName type } \
{
	global SpecialData
	global numberOfSpecials

	set SpecialData($index,varName) $varName
	set SpecialData($index,type) $type
	set SpecialData($index,varValue) 0
	incr numberOfSpecials
}

#######################################
# Opens or closes diagnostics listbox  

proc PopupDiagWin {} \
{
	if { [ wm state .list ] == "withdrawn" } {
		wm deiconify .list
		raise .list
		.lbframe.diags configure -text "Close Diagnostics"
	} else {
		wm withdraw .list
		.lbframe.diags configure -text "Open Diagnostics"
	}
}

#########################################
# Calls C procedure to change variables

proc SetNewSpecials { } \
{
	global SpecialData
	global numberOfSpecials

	for { set index 0 } { $index < $numberOfSpecials } { incr index } {
		C_SetNewSpecials $index $SpecialData($index,varValue)
	}
}

proc lput { Window } \
{
	global openWindowsList

	if { [lsearch $openWindowsList $Window] == -1 } {
		lappend openWindowsList $Window
	}
}
	
proc lremove { Window } \
{
	global openWindowsList

	set index [lsearch $openWindowsList $Window] 
	if { $index >= 0 } {
		set openWindowsList [lreplace $openWindowsList $index $index]
	}
}

proc Open { Window } \
{
        global WinData
         global openWindowsList
        global WasRunning

         
	if { $WinData($Window,created) == 1 } {
		if { $WasRunning == 1 } {
		#C_printf "stopping"
	        C_Stop
		update idletasks
		    update idletasks
		}
		set WinData($Window,open) 1
		lput $Window
		wm deiconify $Window
		tkwait visibility $Window
		#raise $Window
		MarkOpen $Window
	    #update idletasks
		C_Open $WinData($Window,index) 
		update 
		if { $WasRunning == 1 } {
		#update idletasks
		C_Run
		#C_printf "running"
	    }
	} else {
	    if { $WasRunning == 1 } {
		#C_printf "stopping"
	        C_Stop
		update idletasks
	}
	CreateWindow $Window
	set WinData($Window,open) 1
	lput $Window
	C_CreateWindow $WinData($Window,index) $Window.middle.label
	#C_pause "C_CreatedWindow"
	#C_printf "C_CreatedWindow"
	set WinData($Window,created) 1
	MarkOpen $Window
	update
	#C_printf "marked window open"
	C_Open $WinData($Window,index) 
	#C_printf "C_opened window"
	    if { $WasRunning == 1 } {
		#update idletasks
		C_Run
		#C_printf "running"
	    }
    }
    
}

proc MarkOpen { Window } \
{
	global WinData

         set a [.list.lframe.openbox yview]
        .list.lframe.openbox delete $WinData($Window,index)
	.list.lframe.openbox insert $WinData($Window,index) "*"
        .list.lframe.openbox yview moveto [ lindex $a 0 ]
 }

proc Close { Window } \
{
	global WinData
	global WasRunning

	if { $WinData($Window,created) == 1 } {
		set WinData($Window,open) 0
		lremove $Window
		MarkClosed $Window
		C_Close $WinData($Window,index) 
		wm withdraw $Window
	    #tkwait visibility $Window
	}
}

proc MarkClosed { Window } \
{
	global WinData

	set a [.list.lframe.openbox yview]
        .list.lframe.openbox delete $WinData($Window,index)
	.list.lframe.openbox insert $WinData($Window,index) " "
        .list.lframe.openbox yview moveto [ lindex $a 0 ]
}

proc Tk_XGStart {} \
{
	global WinData
	global openWindowsList
# changed "from" to "anchor" below  --- djc for tk4
	.list.lframe.listbox select anchor 0
	foreach Window $openWindowsList {
	        C_CreateWindow $WinData($Window,index) $Window.middle.label
		MarkOpen $Window
	}
	update
}


# tkScrollDrag ********** to update listbox and openbox*******--
# This procedure is called for each mouse motion even when the slider
# is being dragged.  It notifies the associated widget if we're not
# jump scrolling, and it just updates the scrollbar if we are jump
# scrolling.
#
# Arguments:
# w -           The scrollbar widget.
# x, y -        The current mouse position.

proc tkScrollDragCuston {w x y} {
    global tkPriv
    global tk_version

    if {$tk_version >= 8.4} {
	array set tkPriv [ array get ::tk::Priv ]
    }
    if {$tkPriv(initPos) == ""} {
        return
    }
    set delta [$w delta [expr $x - $tkPriv(pressX)] [expr $y - $tkPriv(pressY)]]
    if [$w cget -jump] {
        if {[llength $tkPriv(initValues)] == 2} {
            $w set [expr [lindex $tkPriv(initValues) 0] + $delta] \
                    [expr [lindex $tkPriv(initValues) 1] + $delta]
        } else {
            set delta [expr round($delta * [lindex $tkPriv(initValues) 0])]
            eval $w set [lreplace $tkPriv(initValues) 2 3 \
                    [expr [lindex $tkPriv(initValues) 2] + $delta] \
                    [expr [lindex $tkPriv(initValues) 3] + $delta]]
        }
    } else {
        tkScrollToPosCustom $w [expr $tkPriv(initPos) + $delta]
    }
}
proc tkScrollToPosCustom {w pos} {
    set cmd ".list.lframe.listbox yview"
    set info [$w get]
    if {[llength $info] == 2} {
        uplevel #0 $cmd moveto $pos
    } else {
        uplevel #0 $cmd [expr round([lindex $info 0]*$pos)]
    }
    set cmd ".list.lframe.openbox yview"
    set info [$w get]
    if {[llength $info] == 2} {
        uplevel #0 $cmd moveto $pos
    } else {
        uplevel #0 $cmd [expr round([lindex $info 0]*$pos)]
    }
}
