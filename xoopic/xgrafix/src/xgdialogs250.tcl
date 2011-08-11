proc EntryIntegerProc { window key } \
{
	switch $key {
	"0" -
	"1" -
	"2" -
	"3" -
	"4" -
	"5" -
	"6" -
	"7" -
	"8" -
	"9" -
	"0" { $window insert insert $key }
	"minus" { $window insert insert - }
	"Delete" -
	"BackSpace" { $window delete [expr [$window index insert] - 1] }
	}

	}

proc EntryFloatProc { window key } \
{
	switch $key {
	"0" -
	"1" -
	"2" -
	"3" -
	"4" -
	"5" -
	"6" -
	"7" -
	"8" -
	"9" -
	"0" -
	"e" -
	"E" { $window insert insert $key }
	"period" { $window insert insert . }
	"minus" { $window insert insert - }
	"Delete" -
	"BackSpace" { $window delete [expr [$window index insert] - 1] }
	}

}

proc EntryControlProc { window key } \
{
	switch $key {
	"a" { $window icursor 0 }
	"b" { $window icursor [expr [$window index insert] - 1] }
	"d" { $window delete [$window index insert] }
	"e" { $window icursor end }
	"f" { $window icursor [expr [$window index insert] + 1] }
	"h" { $window delete [expr [$window index insert] - 1] }
	}

}

proc CreateInputFileDialog { } \
{
	global InputFile
	global InputFileCreated
	global XGrafixIcon

	toplevel .inputfile
	
	wm title .inputfile "Input File Viewer"
	wm iconbitmap . $XGrafixIcon
	wm minsize .inputfile 80 24
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol .inputfile WM_DELETE_WINDOW [list ViewInputFile]

	label .inputfile.label -text $InputFile -relief raised
	pack .inputfile.label -fill x -ipady 3

	frame .inputfile.frame
	text .inputfile.frame.text -highlightthickness 0 -relief raised -setgrid true -wrap word \
		-bd 2 -yscrollcommand { .inputfile.frame.yscrollbar set }
	bind .inputfile.frame.text <1> { }
	bind .inputfile.frame.text <B1-Motion> { }
	bind .inputfile.frame.text <Double-1> { }
	pack .inputfile.frame.text -fill both -expand true -side left
	scrollbar .inputfile.frame.yscrollbar -highlightthickness 0 -relief raised -command \
		{ .inputfile.frame.text yview }
	pack .inputfile.frame.yscrollbar -fill y -side right
	pack .inputfile.frame -fill both -expand true

	frame .inputfile.bottom -relief raised -bd 2
	button .inputfile.bottom.close -relief raised -padx 0 -pady 0 -highlightthickness 0 -text Close -command \
		{ wm withdraw .inputfile
		  .lbframe.view configure -padx 0 -pady 0 -highlightthickness 0 -text "Open Input File" }
	pack .inputfile.bottom.close -side right -padx 3 -pady 3 -ipadx 3
	pack .inputfile.bottom -fill x

	set line [open $InputFile]
	while { [eof $line] == 0 } {
	        .inputfile.frame.text insert end [gets $line]
	        .inputfile.frame.text insert end "\n"
	}
	
	set InputFileCreated 1
}

proc ToggleButton { theButton variableName onText offText } \
{
	upvar #0 $variableName variable

	if { $variable == 0 } {
		$theButton configure -padx 0 -pady 0 -highlightthickness 0 -text $onText
		set variable 1
	} else {
		$theButton configure -padx 0 -pady 0 -highlightthickness 0 -text $offText
		set variable 0
	}
}

##########################
# The Rescale Dialog Box

proc CreateRescaleDialog { type } \
{
	global WinData
	global WaitVariable
	global XGrafixIcon
	global ThreeD
	global defaultDialogBoxGeometry
	global rescaleTitle
	global rescaleXMin
	global rescaleXMax
	global rescaleAutoX
	global rescaleYMin
	global rescaleYMax
	global rescaleAutoY
	global rescaleZMin
	global rescaleZMax
	global rescaleAutoZ

	if { $type == "3D" } {
		set RescalePath .3drescaledialog
		set xSize 209
		set ySize 408
	} else {
		set RescalePath .2drescaledialog
		set xSize 209
		set ySize 292
	}

	toplevel $RescalePath
	wm withdraw $RescalePath
	wm title $RescalePath "Rescale Dialog Box"
	wm iconbitmap $RescalePath $XGrafixIcon
	wm geometry $RescalePath $defaultDialogBoxGeometry
	wm minsize $RescalePath $xSize $ySize
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $RescalePath WM_DELETE_WINDOW [list RescaleButtonProc $RescalePath old]

	label $RescalePath.label -relief raised -textvariable rescaleTitle
	pack $RescalePath.label -fill x -ipady 3

	##############
	# The X Axis

	frame $RescalePath.xaxis -relief raised -bd 2

	frame $RescalePath.xaxis.xmax
	label $RescalePath.xaxis.xmax.label -text xmax -width 15 -anchor w
	entry $RescalePath.xaxis.xmax.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable rescaleXMax

	bind $RescalePath.xaxis.xmax.entry <KeyPress> { set rescaleAutoX Off; EntryFloatProc %W %K; break }
	bind $RescalePath.xaxis.xmax.entry <Control-KeyPress> {
		EntryControlProc %W %K; break }
	pack $RescalePath.xaxis.xmax.label -side left
	pack $RescalePath.xaxis.xmax.entry -side left -fill x -expand true

	frame $RescalePath.xaxis.xmin
	label $RescalePath.xaxis.xmin.label -text xmin -width 15 -anchor w
	entry $RescalePath.xaxis.xmin.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable rescaleXMin
	bind $RescalePath.xaxis.xmin.entry <KeyPress> { set rescaleAutoX Off; EntryFloatProc %W %K; break }
	bind $RescalePath.xaxis.xmin.entry <Control-KeyPress> {
		EntryControlProc %W %K break }
	pack $RescalePath.xaxis.xmin.label -side left
	pack $RescalePath.xaxis.xmin.entry -side left -fill x -expand true

	frame $RescalePath.xaxis.auto
	label $RescalePath.xaxis.auto.label -text "Autorescale X" -width 15 \
		-anchor w
	button $RescalePath.xaxis.auto.button -padx 0 -pady 0 -highlightthickness 0 -text On -width 12 \
		-command "ToggleButton $RescalePath.xaxis.auto.button \
			rescaleAutoX On Off"

	pack $RescalePath.xaxis.auto.label -side left
	pack $RescalePath.xaxis.auto.button -side left -fill both -expand true

	frame $RescalePath.xaxis.linlog
	label $RescalePath.xaxis.linlog.label -text "Scale Type" -width 15 \
		-anchor w
	button $RescalePath.xaxis.linlog.button -padx 0 -pady 0 -highlightthickness 0 -text Linear -width 12 \
		-command "ToggleButton $RescalePath.xaxis.linlog.button \
			rescaleXType Logarithmic Linear"
	pack $RescalePath.xaxis.linlog.label -side left
	pack $RescalePath.xaxis.linlog.button -side left -fill both \
		-expand true
		
	pack $RescalePath.xaxis.xmax $RescalePath.xaxis.xmin -fill x -pady 3
	pack $RescalePath.xaxis.auto $RescalePath.xaxis.linlog -fill both \
		-pady 3 -expand true
	pack $RescalePath.xaxis -fill both -expand true

	##############
	# The Y Axis

	frame $RescalePath.yaxis -relief raised -bd 2

	frame $RescalePath.yaxis.ymax
	label $RescalePath.yaxis.ymax.label -text ymax -width 15 -anchor w
	entry $RescalePath.yaxis.ymax.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable rescaleYMax
	bind $RescalePath.yaxis.ymax.entry <KeyPress> { set rescaleAutoY Off; EntryFloatProc %W %K; break }
	bind $RescalePath.yaxis.ymax.entry <Control-KeyPress> {
		EntryControlProc %W %K; break }
	pack $RescalePath.yaxis.ymax.label -side left
	pack $RescalePath.yaxis.ymax.entry -side left -fill x -expand true

	frame $RescalePath.yaxis.ymin
	label $RescalePath.yaxis.ymin.label -text ymin -width 15 -anchor w
	entry $RescalePath.yaxis.ymin.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable rescaleYMin
	bind $RescalePath.yaxis.ymin.entry <KeyPress> { set rescaleAutoY Off; EntryFloatProc %W %K; break }
	bind $RescalePath.yaxis.ymin.entry <Control-KeyPress> {
		EntryControlProc %W %K; break }
	pack $RescalePath.yaxis.ymin.label -side left
	pack $RescalePath.yaxis.ymin.entry -side left -fill x -expand true

	frame $RescalePath.yaxis.auto
	label $RescalePath.yaxis.auto.label -text "Autorescale Y" -width 15 \
		-anchor w
	button $RescalePath.yaxis.auto.button -padx 0 -pady 0 -highlightthickness 0 -text On -width 12 \
		-command "ToggleButton $RescalePath.yaxis.auto.button \
			rescaleAutoY On Off"
	pack $RescalePath.yaxis.auto.label -side left
	pack $RescalePath.yaxis.auto.button -side left -fill both -expand true

	frame $RescalePath.yaxis.linlog
	label $RescalePath.yaxis.linlog.label -text "Scale Type" -width 15 \
		-anchor w
	button $RescalePath.yaxis.linlog.button -padx 0 -pady 0 -highlightthickness 0 -text Linear -width 12 \
		-command "ToggleButton $RescalePath.yaxis.linlog.button \
			rescaleYType Logarithmic Linear"

	pack $RescalePath.yaxis.linlog.label -side left
	pack $RescalePath.yaxis.linlog.button -side left -fill both \
		-expand true
		
	pack $RescalePath.yaxis.ymax $RescalePath.yaxis.ymin -fill x -pady 3
	pack $RescalePath.yaxis.auto $RescalePath.yaxis.linlog -fill both \
		-pady 3 -expand true
	pack $RescalePath.yaxis -fill both -expand true

	##############
	# The Z Axis

	if { $type == "3D" } {
	frame $RescalePath.zaxis -relief raised -bd 2

	frame $RescalePath.zaxis.zmax
	label $RescalePath.zaxis.zmax.label -text zmax -width 15 -anchor w
	entry $RescalePath.zaxis.zmax.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable rescaleZMax
	bind $RescalePath.zaxis.zmax.entry <KeyPress> { set rescaleAutoZ Off; EntryFloatProc %W %K; break }
	bind $RescalePath.zaxis.zmax.entry <Control-KeyPress> { 
		EntryControlProc %W %K; break }
	pack $RescalePath.zaxis.zmax.label -side left
	pack $RescalePath.zaxis.zmax.entry -side left -fill x -expand true

	frame $RescalePath.zaxis.zmin
	label $RescalePath.zaxis.zmin.label -text zmin -width 15 -anchor w
	entry $RescalePath.zaxis.zmin.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable rescaleZMin
	bind $RescalePath.zaxis.zmin.entry <KeyPress> { set rescaleAutoZ Off; EntryFloatProc %W %K; break }
	bind $RescalePath.zaxis.zmin.entry <Control-KeyPress> {
		EntryControlProc %W %K; break }
	pack $RescalePath.zaxis.zmin.label -side left
	pack $RescalePath.zaxis.zmin.entry -side left -fill x -expand true

	frame $RescalePath.zaxis.auto
	label $RescalePath.zaxis.auto.label -text "Autorescale Z" -width 15 \
		-anchor w
	button $RescalePath.zaxis.auto.button -padx 0 -pady 0 -highlightthickness 0 -text On -width 12 \
		-command "ToggleButton $RescalePath.zaxis.auto.button \
			rescaleAutoZ On Off"

	pack $RescalePath.zaxis.auto.label -side left
	pack $RescalePath.zaxis.auto.button -side left -fill both -expand true

	frame $RescalePath.zaxis.linlog
	label $RescalePath.zaxis.linlog.label -text "Scale Type" -width 15 \
		-anchor w
	button $RescalePath.zaxis.linlog.button -padx 0 -pady 0 -highlightthickness 0 -text Linear -width 12 \
		-command "ToggleButton $RescalePath.zaxis.linlog.button \
			rescaleZType Logarithmic Linear"

	pack $RescalePath.zaxis.linlog.label -side left
	pack $RescalePath.zaxis.linlog.button -side left -fill both \
		-expand true
		
	pack $RescalePath.zaxis.zmax $RescalePath.zaxis.zmin -fill x -pady 3
	pack $RescalePath.zaxis.auto $RescalePath.zaxis.linlog -fill both \
		-pady 3 -expand true
	pack $RescalePath.zaxis -fill both -expand true
	}

	#####################
	# Some key bindings

	MakeBindings $RescalePath $RescalePath.xaxis.xmax.entry \
		$RescalePath.xaxis.xmin.entry "entry2entry"
	MakeBindings "" $RescalePath.xaxis.xmin.entry \
		$RescalePath.xaxis.auto.button "entry2button"
	MakeBindings $RescalePath $RescalePath.xaxis.auto.button \
		$RescalePath.xaxis.linlog.button "button2button"
	MakeBindings "" $RescalePath.xaxis.linlog.button \
		$RescalePath.yaxis.ymax.entry "button2entry"

	MakeBindings $RescalePath $RescalePath.yaxis.ymax.entry \
		$RescalePath.yaxis.ymin.entry "entry2entry"
	MakeBindings "" $RescalePath.yaxis.ymin.entry \
		$RescalePath.yaxis.auto.button "entry2button"
	MakeBindings $RescalePath $RescalePath.yaxis.auto.button \
		$RescalePath.yaxis.linlog.button "button2button"

	if { $type != "3D" } {
		MakeBindings "" $RescalePath.yaxis.linlog.button \
			$RescalePath.xaxis.xmax.entry "button2entry"
	} else {
		MakeBindings "" $RescalePath.yaxis.linlog.button \
			$RescalePath.zaxis.zmax.entry "button2entry"
		MakeBindings $RescalePath $RescalePath.zaxis.zmax.entry \
			$RescalePath.zaxis.zmin.entry "entry2entry"
		MakeBindings "" $RescalePath.zaxis.zmin.entry \
			$RescalePath.zaxis.auto.button "entry2button"
		MakeBindings $RescalePath $RescalePath.zaxis.auto.button \
			$RescalePath.zaxis.linlog.button "button2button"
		MakeBindings "" $RescalePath.zaxis.linlog.button \
			$RescalePath.xaxis.xmax.entry "button2entry"
	}

	#########################
	# Ok and Cancel buttons

	frame $RescalePath.bottom -relief raised -bd 2
	button $RescalePath.bottom.apply -padx 0 -pady 0 -highlightthickness 0 -text Apply -command { \
		set Changed 1
		Rescale new
		GetRanges}
	button $RescalePath.bottom.ok -padx 0 -pady 0 -highlightthickness 0 -text Ok -command " \
		[list RescaleButtonProc $RescalePath new ] "
	button $RescalePath.bottom.cancel -padx 0 -pady 0 -highlightthickness 0 -text Cancel -command " \
		[list RescaleButtonProc $RescalePath old ] "
	pack $RescalePath.bottom.cancel $RescalePath.bottom.ok \
		$RescalePath.bottom.apply -side right -padx 3 -pady 3 -ipadx 3
	pack $RescalePath.bottom -fill x
}

proc RescaleButtonProc { path type } \
{
	global WaitVariable
	global Changed
	global rescaleWindowPath
	global WinData
	global VectorD

	set WaitVariable 1
	wm withdraw $path
	grab release $path
        #focus .list.top.entry
        
	set Window $rescaleWindowPath
	if { $WinData($Window,type) == $VectorD } {
		pack .2drescaledialog.xaxis.linlog \
			.2drescaledialog.yaxis.linlog -pady 3
	}

	if { [string compare $type new] == 0 } {
		Rescale new
		SetRescale
	} else {
		if { $Changed == 1 } {
			Rescale old
			set Changed 0
		}
	}

	$rescaleWindowPath.bframe.rescale configure -relief raised
}

proc SetButtonString { theButton variable onText offText } \
{
	if { $variable == 0 } {
		$theButton configure -padx 0 -pady 0 -highlightthickness 0 -text $offText
	} else {
		$theButton configure -padx 0 -pady 0 -highlightthickness 0 -text $onText
	}
}

#################################################
# Backing up the data in case Cancel is pressed

proc GetRanges { } \
{
	global WinData
	global rescaleTitle
	global rescaleXMin
	global rescaleXMax
	global rescaleAutoX
	global rescaleXType
	global rescaleYMin
	global rescaleYMax
	global rescaleAutoY
	global rescaleYType
	global rescaleZMin
	global rescaleZMax
	global rescaleAutoZ
	global rescaleZType
	global rescaleWindowPath
	global ThreeD

	set Window $rescaleWindowPath
	if { $WinData($rescaleWindowPath,type) == $ThreeD } {
		set rescaleWindow .3drescaledialog
	} else {
		set rescaleWindow .2drescaledialog
	}

	set values [C_GetRanges $WinData($Window,index)]

	set rescaleXMin [lindex $values 0]
	set rescaleXMax [lindex $values 1]
	set rescaleAutoX [lindex $values 2]
	set rescaleXType [lindex $values 3]
	SetButtonString $rescaleWindow.xaxis.auto.button $rescaleAutoX On Off
	SetButtonString $rescaleWindow.xaxis.linlog.button $rescaleXType \
		Logarithmic Linear

	set rescaleYMin [lindex $values 4]
	set rescaleYMax [lindex $values 5]
	set rescaleAutoY [lindex $values 6]
	set rescaleYType [lindex $values 7]
	SetButtonString $rescaleWindow.yaxis.auto.button $rescaleAutoY On Off
	SetButtonString $rescaleWindow.yaxis.linlog.button $rescaleYType \
		Logarithmic Linear

	if { $WinData($rescaleWindowPath,type) == $ThreeD } {
		set rescaleZMin [lindex $values 8]
		set rescaleZMax [lindex $values 9]
		set rescaleAutoZ [lindex $values 10]
		set rescaleZType [lindex $values 11]
		SetButtonString $rescaleWindow.zaxis.auto.button \
			$rescaleAutoZ On Off
		SetButtonString $rescaleWindow.zaxis.linlog.button \
			$rescaleZType Logarithmic Linear
	}
}

#######################################################
# Restoring the backed up data when Cancel is pressed

proc SetRescale { } \
{
	global WinData
	global rescaleWindowPath
	global rescaleXMin
	global rescaleXMax
	global rescaleAutoX
	global rescaleXType
	global rescaleYMin
	global rescaleYMax
	global rescaleAutoY
	global rescaleYType
	global rescaleZMin
	global rescaleZMax
	global rescaleAutoZ
	global rescaleZType
	global ThreeD

	set Window $rescaleWindowPath

	set WinData($Window,xMin) $rescaleXMin
	set WinData($Window,xMax) $rescaleXMax
	set WinData($Window,xAuto) $rescaleAutoX
	set WinData($Window,xLinLog) $rescaleXType

	set WinData($Window,yMin) $rescaleYMin
	set WinData($Window,yMax) $rescaleYMax
	set WinData($Window,yAuto) $rescaleAutoY
	set WinData($Window,yLinLog) $rescaleYType

	set WinData($Window,zMin) $rescaleZMin
	set WinData($Window,zMax) $rescaleZMax
	set WinData($Window,zAuto) $rescaleAutoZ
	set WinData($Window,zLinLog) $rescaleZType

	if { $WinData($Window,type) != $ThreeD } {
		if { $rescaleAutoX == 1 || $rescaleAutoY == 1 } {
			$Window.bframe.trace configure -state disabled
		} else {
			$Window.bframe.trace configure -state normal
		}
	}
}

###########################################
# Calls a C procedure to do the rescaling

proc Rescale { type } \
{
	global WinData
	global rescaleXMin
	global rescaleXMax
	global rescaleAutoX
	global rescaleXType
	global rescaleYMin
	global rescaleYMax
	global rescaleAutoY
	global rescaleYType
	global rescaleZMin
	global rescaleZMax
	global rescaleAutoZ
	global rescaleZType
	global rescaleWindowPath

	set Window $rescaleWindowPath
	
	if { $type == "new" } {
		C_Rescale $WinData($Window,index) $rescaleXMin $rescaleXMax \
			$rescaleAutoX $rescaleXType $rescaleYMin $rescaleYMax \
			$rescaleAutoY $rescaleYType $rescaleZMin $rescaleZMax \
			$rescaleAutoZ $rescaleZType
	} else {
		C_Rescale $WinData($Window,index) $WinData($Window,xMin) \
			$WinData($Window,xMax) $WinData($Window,xAuto) \
			$WinData($Window,xLinLog) $WinData($Window,yMin) \
			$WinData($Window,yMax) $WinData($Window,yAuto) \
			$WinData($Window,yLinLog) $WinData($Window,zMin) \
			$WinData($Window,zMax) $WinData($Window,zAuto) \
			$WinData($Window,zLinLog)
	}
}

#####################################################################
# Pops up the Dialog Box and grabs the focus, stopping if necessary

proc PopupRescale { Window } \
{
	global WaitVariable
	global WinData
	global ThreeD
	global rescaleTitle
	global rescaleWindowPath
	global WasRunning
	global VectorD

	set rescaleTitle $WinData($Window,title)
	set rescaleWindowPath $Window

	GetRanges 

	$rescaleWindowPath.bframe.rescale configure -relief sunken

	if { $WinData($Window,type) != $ThreeD } {
		set RescalePath .2drescaledialog
	} else {
		set RescalePath .3drescaledialog
	}

	if { $WasRunning == 1 } {
		C_Stop
	}

	if { $WinData($Window,type) == $VectorD } {
		pack forget $RescalePath.xaxis.linlog $RescalePath.yaxis.linlog
		update idletasks
	}

	wm deiconify $RescalePath
	focus $RescalePath.xaxis.xmax.entry
	grab set $RescalePath

	if { $WasRunning == 1 } {
		set WaitVariable 0
		tkwait variable WaitVariable
		if { $WaitVariable == 1 } {
			C_Run
		}
	}
}

########################
# The Print Dialog Box

proc CreatePrintDialog { } \
{
	global WinData
	global WaitVariable
	global XGrafixIcon
	global defaultDialogBoxGeometry
	global printType
	global printMode
	global printTitle
	global printFile
	global printPlotTitle
	global printPeriod
	global printLimit

	set PSPath .printdialog

	toplevel $PSPath
	wm withdraw $PSPath
	wm title $PSPath "Print Dialog Box"
	wm iconbitmap $PSPath $XGrafixIcon
	wm geometry $PSPath $defaultDialogBoxGeometry
	wm minsize $PSPath 193 204
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $PSPath WM_DELETE_WINDOW [list PSButtonProc $PSPath cancel]

	label $PSPath.label -relief raised -textvariable printTitle
	pack $PSPath.label -fill x -ipady 3

	frame $PSPath.middle -relief raised -bd 2

	############
	# The menus

	frame $PSPath.top
	menubutton $PSPath.top.type -text "File Type" -menu \
		$PSPath.top.type.menu -relief raised
	menu $PSPath.top.type.menu
	$PSPath.top.type.menu add radiobutton -selectcolor red  -label "EPS" \
		-value eps -variable printType -command { ChangeFileTo PS }
	$PSPath.top.type.menu add radiobutton -selectcolor red -label "XPM" \
		-value xpm -variable printType -command { ChangeFileTo XPM }
	$PSPath.top.type.menu add radiobutton -selectcolor red -label "PNG" \
		-value png -variable printType -command { ChangeFileTo PNG }
	$PSPath.top.type.menu add radiobutton -selectcolor red -label "GIF" \
		-value gif -variable printType -command { ChangeFileTo GIF }
	$PSPath.top.type.menu add radiobutton -selectcolor red -label "ASCII" \
		-value ascii -variable printType -command { ChangeFileTo ASCII } 
	$PSPath.top.type.menu add radiobutton -selectcolor red -label "XGBIN" \
		-value bxg -variable printType -command { ChangeFileTo XGBIN }

	menubutton $PSPath.top.mode -text "Print Mode" -menu \
		$PSPath.top.mode.menu -relief raised
	menu $PSPath.top.mode.menu
	$PSPath.top.mode.menu add radiobutton -selectcolor red -label "Overwrite" \
		-value overwrite -variable printMode
	$PSPath.top.mode.menu add radiobutton -selectcolor red -label "Append" \
		-value append -variable printMode
	$PSPath.top.mode.menu add radiobutton -selectcolor red -label "Increment" \
		-value count -variable printMode

	label $PSPath.top.end -relief raised
	pack $PSPath.top.type $PSPath.top.mode -side left -ipadx 3 -ipady 3
	pack $PSPath.top.end -fill both -expand true

	###############
	# The entries

	frame $PSPath.middle.file
	label $PSPath.middle.file.label -width 10 -text "File name" \
		-anchor w
	entry $PSPath.middle.file.entry -highlightthickness 0 -textvariable printFile \
		-relief sunken -width 15
	bind $PSPath.middle.file.entry <Control-KeyPress> { EntryControlProc \
		%W %K; break }
	pack $PSPath.middle.file.label -side left
	pack $PSPath.middle.file.entry -side left -fill x -expand true

	frame $PSPath.middle.plot
	label $PSPath.middle.plot.label -width 10 -text "Plot title" \
		-anchor w
	entry $PSPath.middle.plot.entry -highlightthickness 0 -textvariable printPlotTitle \
		-relief sunken -width 15
	bind $PSPath.middle.plot.entry <Control-KeyPress> { EntryControlProc \
		%W %K; break }
	pack $PSPath.middle.plot.label -side left
	pack $PSPath.middle.plot.entry -side left -fill x -expand true

	frame $PSPath.middle.period
	label $PSPath.middle.period.label -width 10 -text "Print period" \
		-anchor w
	entry $PSPath.middle.period.entry -highlightthickness 0 -textvariable printPeriod \
		-relief sunken -width 15
	bind $PSPath.middle.period.entry <Control-KeyPress> {EntryIntegerProc \
		%W %K; break }
	pack $PSPath.middle.period.label -side left
	pack $PSPath.middle.period.entry -side left -fill x -expand true

	frame $PSPath.middle.limit
	label $PSPath.middle.limit.label -width 10 -text "Iterations" \
		-anchor w
	entry $PSPath.middle.limit.entry -highlightthickness 0 -textvariable printLimit \
		-relief sunken -width 15
	bind $PSPath.middle.limit.entry <Control-KeyPress> { EntryIntegerProc \
		%W %K; break }
	pack $PSPath.middle.limit.label -side left
	pack $PSPath.middle.limit.entry -side left -fill x -expand true

	pack $PSPath.middle.file $PSPath.middle.plot $PSPath.middle.period \
		$PSPath.middle.limit -pady 3 -fill both -expand true

	#############################
	# The Ok and Cancel buttons

	frame $PSPath.bottom -relief raised -bd 2
	button $PSPath.bottom.apply
	button $PSPath.bottom.ok -padx 0 -pady 0 -highlightthickness 0 -text Ok -command \
		[list PSButtonProc $PSPath ok ]
	button $PSPath.bottom.cancel -padx 0 -pady 0 -highlightthickness 0 -text Cancel -command \
		[list PSButtonProc $PSPath cancel]
	pack $PSPath.bottom.cancel $PSPath.bottom.ok -side right -padx 3 \
		-pady 3 -ipadx 3

	pack $PSPath.top -fill x
	pack $PSPath.middle -fill both -expand true
	pack $PSPath.bottom -fill x

	################################
	# Key bindings for the entries

	MakeBindings $PSPath $PSPath.middle.file.entry \
		$PSPath.middle.plot.entry "entry2entry"
	MakeBindings "" $PSPath.middle.plot.entry $PSPath.middle.period.entry \
		"entry2entry"
	MakeBindings $PSPath $PSPath.middle.period.entry \
		$PSPath.middle.limit.entry "entry2entry"
	MakeBindings "" $PSPath.middle.limit.entry $PSPath.middle.file.entry \
		"entry2entry"
}

proc ChangeFileTo { type } \
{
	global printFile
	global printMode

	switch $type {
	"PS" { set extension ".eps"
		.printdialog.top.mode.menu entryconfigure 2 -state normal}
	"XPM" { set extension ".xpm"
		.printdialog.top.mode.menu entryconfigure 2 -state disabled
		if { $printMode == "append" } {
			set printMode count
		} }
	"PNG" { set extension ".png"
		.printdialog.top.mode.menu entryconfigure 2 -state disabled
		if { $printMode == "append" } {
			set printMode count
		} }
	"GIF" { set extension ".gif"
		.printdialog.top.mode.menu entryconfigure 2 -state disabled
		if { $printMode == "append" } {
			set printMode count
		} }
	"ASCII" { set extension ".txt"
		.printdialog.top.mode.menu entryconfigure 2 -state normal }

	"XGBIN" { set extension ".bxg"
		.printdialog.top.mode.menu entryconfigure 2 -state normal }
	    }

	if { [set index [string last .eps $printFile]] == -1 } {
		if { [set index [string last .xpm $printFile]] == -1 } {
			if { [set index [string last .txt $printFile]] == -1} {
			if { [set index [string last .png $printFile]] == -1} {
			if { [set index [string last .gif $printFile]] == -1} {
			if { [set index [string last .bxg $printFile]] == -1} {
				append printFile $extension
				return
			}
		    }
		    }
		    }
		    }
	    }
	
	
	set printFile [string range $printFile 0 [expr $index - 1]]
	append printFile $extension
}

proc PSButtonProc { path type } \
{
	global WaitVariable
	global WinData
	global printWindowPath
	global theCurrentStep

	set WaitVariable 1
	wm withdraw $path
	grab release $path
	#focus .list.top.entry

	if { [string compare $type ok] == 0 } {
		set WinData($printWindowPath,printSetStep) $theCurrentStep
		SetPrint
		if { $WinData($printWindowPath,printPeriod) == 0 } {
			for {} {$WinData($printWindowPath,printLimit) > 0} \
			       {incr WinData($printWindowPath,printLimit) -1} {
				Print $printWindowPath
			}
		}
	}

	$printWindowPath.bframe.print configure -relief raised
}

#######################################################
# Pops up the print dialog box, stopping if necessary

proc PopupPrint { Window } \
{
	global WaitVariable
	global WinData
	global printTitle
	global printFile
	global printPlotTitle
	global printType
	global printMode
	global printPeriod
	global printLimit
	global printWindowPath
	global WasRunning

	set printTitle $WinData($Window,title)
	set printFile $WinData($Window,PSFile)
	set printPlotTitle $WinData($Window,plot_title)
	set printType $WinData($Window,printType)
	set printMode $WinData($Window,printMode)
	set printPeriod $WinData($Window,printPeriod)

	if { $WinData($Window,printLimit) < 1 } {
		set printLimit 1
	} else {
		set printLimit $WinData($Window,printLimit)
	}

	set printWindowPath $Window

	$printWindowPath.bframe.print configure -relief sunken
	
	if { $WasRunning == 1 } {
		C_Stop
	}

	set PSPath .printdialog
	wm deiconify $PSPath
	focus $PSPath.middle.file.entry
	grab set $PSPath

	if { $WasRunning == 1 } {
		set WaitVariable 0
		tkwait variable WaitVariable
		if { $WaitVariable == 1 } {
			C_Run
		}
	}
}

proc SetPrint {} \
{
	global WinData
	global printWindowPath
	global printFile
	global printPlotTitle
	global printType
	global printMode
	global printPeriod
	global printLimit

	set WinData($printWindowPath,PSFile) $printFile
	set WinData($printWindowPath,plot_title) $printPlotTitle
	set WinData($printWindowPath,printType) $printType
	set WinData($printWindowPath,printMode) $printMode
	set WinData($printWindowPath,printPeriod) $printPeriod
	set WinData($printWindowPath,printLimit) $printLimit
}

##########################################
# Calls a C procedure to do the printing

proc Print { windowPath } \
{
	global WinData

	if { $WinData($windowPath,printMode) == "count" } {
		while { 1 } {
			set index [ string last . $WinData($windowPath,PSFile)]
			set fileName [ string range \
				$WinData($windowPath,PSFile) 0 [expr $index-1]]
			append fileName "_" $WinData($windowPath,printNumber) \
				[ string range $WinData($windowPath,PSFile) \
					 $index [string length \
						$WinData($windowPath,PSFile)] ]
			incr WinData($windowPath,printNumber)
			if { [catch { open $fileName RDONLY }] == 1 } {
				break
			}
		}
	} else {
		set fileName $WinData($windowPath,PSFile)
	}

	C_Print $WinData($windowPath,index) $fileName \
		$WinData($windowPath,plot_title) \
		$WinData($windowPath,printType) \
		$WinData($windowPath,printMode) \
		[wm state $windowPath]
}

#####################
# The 3D Dialog Box

proc Create3dDialog { } \
{
	global WinData
	global WaitVariable
	global XGrafixIcon
	global defaultDialogBoxGeometry
	global 3dTitle
	global 3dTheta
	global 3dPhi
	global 3dShading
	global 3dColor
	global 3dGrid

	set 3dPath .3ddialog

	toplevel $3dPath
	wm withdraw $3dPath
	wm title $3dPath "3DView Dialog Box"
	wm iconbitmap $3dPath $XGrafixIcon
	wm geometry $3dPath $defaultDialogBoxGeometry
	wm minsize $3dPath 174 204
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $3dPath WM_DELETE_WINDOW [list 3dButtonProc $3dPath old]

	label $3dPath.label -relief raised -textvariable 3dTitle
	pack $3dPath.label -fill x -ipady 3

	###############
	# The entries 

	frame $3dPath.middle -relief raised -bd 2

	frame $3dPath.middle.theta
	label $3dPath.middle.theta.label -text theta -width 10 -anchor w
	entry $3dPath.middle.theta.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable 3dTheta
	bind $3dPath.middle.theta.entry <KeyPress> { EntryFloatProc %W %K; break }
	bind $3dPath.middle.theta.entry <Control-KeyPress> { EntryControlProc \
		%W %K; break }
	pack $3dPath.middle.theta.label -side left
	pack $3dPath.middle.theta.entry -side left -fill x -expand true

	frame $3dPath.middle.phi
	label $3dPath.middle.phi.label -text phi -width 10 -anchor w
	entry $3dPath.middle.phi.entry -highlightthickness 0 -relief sunken -width 12 \
		-textvariable 3dPhi
	bind $3dPath.middle.phi.entry <KeyPress> { EntryFloatProc %W %K; break }
	bind $3dPath.middle.phi.entry <Control-KeyPress> { EntryControlProc \
		%W %K; break } 
	pack $3dPath.middle.phi.label -side left
	pack $3dPath.middle.phi.entry -side left -fill x -expand true

	#############
	# The buttons  

	frame $3dPath.middle.shading
	label $3dPath.middle.shading.label -text Shading -width 10 \
		-anchor w
	button $3dPath.middle.shading.button -padx 0 -pady 0 -highlightthickness 0 -text On -width 12 \
		-command "ToggleButton $3dPath.middle.shading.button \
			3dShading On Off"
	pack $3dPath.middle.shading.label -side left
	pack $3dPath.middle.shading.button -side left -fill both -expand true

	frame $3dPath.middle.color
	label $3dPath.middle.color.label -text "Color Code" -width 10 \
		-anchor w
	button $3dPath.middle.color.button -padx 0 -pady 0 -highlightthickness 0 -text On -width 12 \
		-command "ToggleButton $3dPath.middle.color.button \
			3dColor On Off"
	pack $3dPath.middle.color.label -side left
	pack $3dPath.middle.color.button -side left -fill both -expand true

	frame $3dPath.middle.grid
	label $3dPath.middle.grid.label -text Grid -width 10 \
		-anchor w
	button $3dPath.middle.grid.button -padx 0 -pady 0 -highlightthickness 0 -text On -width 12 \
		-command "ToggleButton $3dPath.middle.grid.button \
			3dGrid On Off"
	pack $3dPath.middle.grid.label -side left
	pack $3dPath.middle.grid.button -side left -fill both -expand true

	pack $3dPath.middle.theta $3dPath.middle.phi -pady 3 -fill x
	pack $3dPath.middle.shading $3dPath.middle.color $3dPath.middle.grid \
		-pady 3 -fill both -expand true

	#############################
	# The Ok and Cancel buttons

	frame $3dPath.bottom -relief raised -bd 2

	button $3dPath.bottom.apply -padx 0 -pady 0 -highlightthickness 0 -text Apply -command { \
		3dView new
		set Changed 1 }
	button $3dPath.bottom.ok -padx 0 -pady 0 -highlightthickness 0 -text Ok -command " \
		[list 3dButtonProc $3dPath new] "
	button $3dPath.bottom.cancel -padx 0 -pady 0 -highlightthickness 0 -text Cancel -command " \
		[list 3dButtonProc $3dPath old] "
	pack $3dPath.bottom.cancel $3dPath.bottom.ok \
		$3dPath.bottom.apply -side right -padx 3 -pady 3 -ipadx 3

	pack $3dPath.middle -fill both -expand true
	pack $3dPath.bottom -fill x

	################################################
	# Key bindings for the entries and the buttons

	MakeBindings $3dPath $3dPath.middle.theta.entry \
		$3dPath.middle.phi.entry "entry2entry"
	MakeBindings "" $3dPath.middle.phi.entry \
		$3dPath.middle.shading.button "entry2button"
	MakeBindings $3dPath $3dPath.middle.shading.button \
		$3dPath.middle.color.button "button2button"
	MakeBindings $3dPath $3dPath.middle.color.button \
		$3dPath.middle.grid.button "button2button"
	MakeBindings "" $3dPath.middle.grid.button \
		$3dPath.middle.theta.entry "button2entry"
}

proc 3dButtonProc { path type } \
{
	global WaitVariable
	global Changed
	global 3dWindowPath

	set WaitVariable 1
	wm withdraw $path
	grab release $path
	#focus .list.top.entry

	if { [string compare $type new] == 0 } {
		Set3D
		3dView new
	} else {
		if { $Changed == 1 } {
			3dView old
			set Changed 0
		}
	}

	$3dWindowPath.bframe.3dview configure -relief raised
}

#################################################
# Pops up the 3D Dialog Box, stopping if needed 

proc Popup3d { Window } \
{
	global WinData
	global WaitVariable
	global 3dTitle
	global 3dTheta
	global 3dPhi
	global 3dShading
	global 3dColor
	global 3dGrid
	global 3dWindowPath
	global WasRunning

	set 3dTitle $WinData($Window,title)
	set 3dTheta $WinData($Window,theta)
	set 3dPhi $WinData($Window,phi)
	set 3dShading $WinData($Window,shading)
	set 3dColor $WinData($Window,color)
	set 3dGrid $WinData($Window,grid)
	set 3dWindowPath $Window

	SetButtonString .3ddialog.middle.shading.button $3dShading On Off
	SetButtonString .3ddialog.middle.color.button $3dColor On Off
	SetButtonString .3ddialog.middle.grid.button $3dGrid On Off

	$3dWindowPath.bframe.3dview configure -relief sunken

	if { $WasRunning == 1 } {
		C_Stop
	}

	set 3dPath .3ddialog
	wm deiconify $3dPath
	focus $3dPath.middle.theta.entry
	grab set $3dPath
	
	if { $WasRunning == 1 } {
		set WaitVariable 0
		tkwait variable WaitVariable
		if { $WaitVariable == 1 } {
			C_Run
		}
	}
}

####################
# Restores 3D Data

proc Set3D { } \
{
	global WinData
	global 3dTheta
	global 3dPhi
	global 3dShading
	global 3dColor
	global 3dGrid
	global 3dWindowPath

	set WinData($3dWindowPath,theta) $3dTheta
	set WinData($3dWindowPath,phi) $3dPhi
	set WinData($3dWindowPath,shading) $3dShading
	set WinData($3dWindowPath,color) $3dColor
	set WinData($3dWindowPath,grid) $3dGrid
}

#######################################
# Use a C procedure to do the 3D stuff

proc 3dView { type } \
{
	global WinData
	global 3dTheta
	global 3dPhi
	global 3dShading
	global 3dColor
	global 3dGrid
	global 3dWindowPath

	if { $type == "new" } {
		C_3DView $WinData($3dWindowPath,index) $3dTheta $3dPhi \
			$3dShading $3dColor $3dGrid
	} else {
		C_3DView $WinData($3dWindowPath,index) \
			$WinData($3dWindowPath,theta) \
	                $WinData($3dWindowPath,phi) \
			$WinData($3dWindowPath,shading) \
        	        $WinData($3dWindowPath,color) \
			$WinData($3dWindowPath,grid)
	}
}

#######################
# The Save Dialog Box 

proc CreateSaveDialog {} \
{
	global SaveFile
	global WaitVariable
	global XGrafixIcon
	global defaultDialogBoxGeometry

	set SavePath .save
	
	toplevel $SavePath
	wm withdraw $SavePath
	wm title $SavePath "Save Dialog Box"
	wm iconbitmap $SavePath $XGrafixIcon
	wm geometry $SavePath $defaultDialogBoxGeometry
	wm minsize $SavePath 263 92
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $SavePath WM_DELETE_WINDOW "[list SaveButton $SavePath]\n [list RestoreSaveData]"
	
	label $SavePath.label -relief raised -text "Dumping the present \
		state into a file"
	pack $SavePath.label -fill x -ipady 3

	#######################
	# The Save Dialog Box  

	frame $SavePath.middle -relief raised -bd 2
	label $SavePath.middle.label -width 15 -text "File name" -anchor w
	entry $SavePath.middle.entry -width 20 -highlightthickness 0 -relief sunken \
		-textvariable SaveFile
	bind $SavePath.middle.entry <Control-KeyPress> { EntryControlProc \
		%W %K; break }
	pack $SavePath.middle.label -side left -pady 3
	pack $SavePath.middle.entry -side left -pady 3 -fill x -expand true
	pack $SavePath.middle -fill both -expand true

	#############################
	# The Ok and Cancel buttons 

	frame $SavePath.bottom -relief raised -bd 2
	button $SavePath.bottom.apply
	button $SavePath.bottom.ok -padx 0 -pady 0 -highlightthickness 0 -text Ok -command " \
		 [list SaveButton $SavePath ]
		  [list Save] "
	button $SavePath.bottom.cancel -padx 0 -pady 0 -highlightthickness 0 -text Cancel -command " \
		 [list SaveButton $SavePath ]
		  [list RestoreSaveData] "
	pack $SavePath.bottom.cancel $SavePath.bottom.ok -side right \
		-padx 3 -pady 3 -ipadx 3
	pack $SavePath.bottom -fill x

	##############################
	# Key bindings for the entry  

	MakeBindings $SavePath $SavePath.middle.entry \
		$SavePath.middle.entry "entry2entry"
}

proc SaveButton { SavePath } \
{
	global WaitVariable

	.lbframe.save configure -relief raised

	set WaitVariable 1
	wm withdraw $SavePath
	grab release $SavePath
	#focus .list.top.entry
}

#########################################
# Calls a C procedure to do the dumping

proc Save {} \
{
	global SaveFile
	C_Save $SaveFile
}

###################################################
# Pops up the Save Dialog Box, stopping if needed 

proc PopupSave {} \
{
	global WaitVariable
	global WasRunning

	if { $WasRunning == 1 } {
		C_Stop
	}

	.lbframe.save configure -relief sunken

	BackupSaveData
	wm deiconify .save
	focus .save.middle.entry
	grab set .save

	if { $WasRunning == 1 } {
		set WaitVariable 0
		tkwait variable WaitVariable
		if { $WaitVariable == 1 } {
			C_Run
		}
	}		
}

############################
# Backing up the Save data

proc BackupSaveData {} \
{
	global SaveFileOld
	global SaveFile

	set SaveFileOld $SaveFile
}

###########################
# Restoring the save data 

proc RestoreSaveData {} \
{
	global SaveFileOld
	global SaveFile

	set SaveFile $SaveFileOld
}

#######################
# The Quit Dialog Box

proc CreateQuitDialog {} \
{
	global WaitVariable
	global XGrafixIcon
	global defaultDialogBoxGeometry

	set QuitPath .quit

	toplevel $QuitPath
	wm withdraw $QuitPath
	wm title $QuitPath "Quit Dialog Box"
	wm iconbitmap $QuitPath $XGrafixIcon
	wm geometry $QuitPath $defaultDialogBoxGeometry
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $QuitPath WM_DELETE_WINDOW "[list set WaitVariable 1]\n [list wm withdraw $QuitPath]\n [list grab release $QuitPath]\n [list .lbframe.quit configure -relief raised ]"

	frame $QuitPath.top -relief raised -bd 2	
	label $QuitPath.top.label1 -text "Do you really want to exit?"
	pack $QuitPath.top.label1 -ipady 3
	pack $QuitPath.top -fill x

	##########################
	# The Yes and No buttons  

	frame $QuitPath.bottom -relief raised -bd 2
	button $QuitPath.bottom.apply
	button $QuitPath.bottom.killX -padx 0 -pady 0 -highlightthickness 0 -text "Kill Graphics" -command \
		{ C_KillGraphics }
	button $QuitPath.bottom.yes -padx 0 -pady 0 -highlightthickness 0 -text Yes -command {C_Quit ; set WaitVariable 2 ; 
		destroy .}
	button $QuitPath.bottom.no -padx 0 -pady 0 -highlightthickness 0 -text No -command \
		" [list set WaitVariable 1]
		  [list wm withdraw $QuitPath]
		  #[list focus .list.top.entry ]
		  [list grab release $QuitPath] 
		  [list .lbframe.quit configure -relief raised ] "
	pack $QuitPath.bottom.no $QuitPath.bottom.yes $QuitPath.bottom.killX \
		-side right -padx 3 -pady 3 -ipadx 3
	pack $QuitPath.bottom -fill x

	bind $QuitPath.top.label1 <Return> [list $QuitPath.bottom.yes invoke]
	bind $QuitPath.top.label1 <Escape> [list $QuitPath.bottom.no invoke]
}

###################################################
# Pops up the Quit Dialog Box, stopping if needed

proc PopupQuit {} \
{
	global WaitVariable
	global WasRunning

	if { $WasRunning == 1 } {
		C_Stop
	}

	.lbframe.quit configure -relief sunken
	wm deiconify .quit
	focus .quit.top.label1
	grab set .quit

	if { $WasRunning == 1 } {
		set WaitVariable 0
		tkwait variable WaitVariable
	        if { $WaitVariable == 1 } {
			C_Run
		}
	}
}

##########################
# The Special Dialog Box  

proc CreateSpecialDialog {} \
{
	global SpecialData
	global numberOfSpecials
	global XGrafixIcon
	global WaitVariable
	global defaultDialogBoxGeometry

	set SpecialPath .special

	toplevel $SpecialPath
	wm withdraw $SpecialPath
	wm iconbitmap $SpecialPath $XGrafixIcon
	wm title $SpecialPath "Special Dialog Box"
	wm geometry $SpecialPath $defaultDialogBoxGeometry
# mindgame: for the frequently happening case that user closes window
#          from windowmanager
	wm protocol $SpecialPath WM_DELETE_WINDOW  "[ list RestoreSpecialData ]\n [ list SpecialButton $SpecialPath ]"

	frame $SpecialPath.top -relief raised -bd 2
	label $SpecialPath.top.label -text "Adjusting internal variables"
	pack $SpecialPath.top.label -ipady 3 -fill x
	pack $SpecialPath.top -fill x

	###############
	# The entries 

	frame $SpecialPath.vars -relief raised -bd 2
	for { set index 0 } { $index < $numberOfSpecials } { incr index } {
		frame $SpecialPath.vars.$index
		label $SpecialPath.vars.$index.label -width 15 -text \
			"($SpecialData($index,type)) $SpecialData($index,varName)" -anchor w
		entry $SpecialPath.vars.$index.entry -width 12 -highlightthickness 0 -relief sunken \
			-textvariable SpecialData($index,varValue)
		if { $SpecialData($index,type) == "float" || $SpecialData($index,type) == "double"} {
			bind $SpecialPath.vars.$index.entry <KeyPress> {
				EntryFloatProc %W %K; break }
		} elseif { $SpecialData($index,type) == "integer"} {
			bind $SpecialPath.vars.$index.entry <KeyPress> {
				EntryIntegerProc %W %K; break }
		}
		bind $SpecialPath.vars.$index.entry <Control-KeyPress> { \
			EntryControlProc %W %K; break }
		pack $SpecialPath.vars.$index.label $SpecialPath.vars.$index.entry \
			-side left -pady 3
		pack $SpecialPath.vars.$index
		    }
	pack $SpecialPath.vars

	################################
	# Key bindings for the entries 

	for { set index 0 } { $index < $numberOfSpecials } { incr index 2 } {
		if { [expr $index + 1] < $numberOfSpecials } {
			MakeBindings $SpecialPath \
				$SpecialPath.vars.$index.entry \
				$SpecialPath.vars.[expr $index + 1].entry \
				"entry2entry"
		}
		if { [expr $index + 2] < $numberOfSpecials } {
			MakeBindings "" \
				$SpecialPath.vars.[expr $index + 1].entry \
				$SpecialPath.vars.[expr $index + 2].entry \
				"entry2entry"
		}
	}
	MakeBindings $SpecialPath $SpecialPath.vars.[expr $numberOfSpecials - \
		1].entry $SpecialPath.vars.0.entry "entry2entry"
		
	#############################
	# The Ok and Cancel buttons  

	frame $SpecialPath.bottom -relief raised -bd 2

	button $SpecialPath.bottom.apply -padx 0 -pady 0 -highlightthickness 0 -text Apply -command \
		[ list SetNewSpecials ]
	button $SpecialPath.bottom.cancel -command \
		" [ list RestoreSpecialData ]
		  [ list SpecialButton $SpecialPath ] " -padx 0 -pady 0 -highlightthickness 0 -text Cancel
	button $SpecialPath.bottom.ok -command \
		" [ list SpecialButton $SpecialPath ] " -padx 0 -pady 0 -highlightthickness 0 -text Ok
	pack $SpecialPath.bottom.cancel $SpecialPath.bottom.ok \
		$SpecialPath.bottom.apply -side right -padx 3 -pady 3 -ipadx 3

	pack $SpecialPath.bottom -fill x

	.lbframe.special configure -command { PopupSpecial }
	pack .lbframe.special -fill both -expand true -ipady 3
}

proc SpecialButton { SpecialPath } \
{
	global WaitVariable

	set WaitVariable 1
	wm withdraw $SpecialPath
	#focus .list.top.entry
	grab release $SpecialPath
	SetNewSpecials
	.lbframe.special configure -relief raised
}

proc BackupSpecialData {} \
{
	global SpecialData

	set index 0
	foreach value [C_GetSpecialValues] {
		set SpecialData($index,varValueOld) $value
		set SpecialData($index,varValue) $value
		incr index
	}
}

proc RestoreSpecialData {} \
{
	global SpecialData
	global numberOfSpecials

	for {set index 0} {$index < $numberOfSpecials} {incr index} {
		set SpecialData($index,varValue) $SpecialData($index,varValueOld)
	}
}
		

######################################################
# Pops up the Special Dialog Box, stopping if needed 

proc PopupSpecial {} \
{
	global WaitVariable
	global WasRunning

	###########################################
	# Gets the current value of each variable 
	BackupSpecialData

	.lbframe.special configure -relief sunken
	if { $WasRunning == 1 } {
		C_Stop
	}

	wm deiconify .special
	focus .special.vars.0.entry
	grab set .special

	if { $WasRunning == 1 } {
		set WaitVariable 0
		tkwait variable WaitVariable
		if { $WaitVariable == 1 } {
			C_Run
		}
	}		
}

######################################################################
# Procedure to make bindings for entries and buttons in dialog boxes

proc MakeBindings { Parent Widget1 Widget2 State } \
{
	if { $Parent != "" } {
	bind $Widget1 <Escape> [list $Parent.bottom.cancel invoke ]
	bind $Widget1 <Return> [list $Parent.bottom.ok invoke ]
	bind $Widget1 <Tab> [list $Parent.bottom.apply invoke]
	bind $Widget2 <Escape> [list $Parent.bottom.cancel invoke ]
	bind $Widget2 <Return> [list $Parent.bottom.ok invoke ]
	bind $Widget2 <Tab> [list $Parent.bottom.apply invoke]
	}

	switch $State {
	"entry2entry" {
		bind $Widget1 <Down> "
			[list focus $Widget2 ] "
		bind $Widget1 <Left> { %W icursor [expr [%W index insert] - 1]
					; break }
		bind $Widget1 <Right> { %W icursor [expr [%W index insert] + 1]
					; break }
		bind $Widget2 <Up> "
			[list focus $Widget1 ] "
		bind $Widget2 <Left> { %W icursor [expr [%W index insert] - 1]
					; break }
		bind $Widget2 <Right> { %W icursor [expr [%W index insert] + 1]
					; break }
	}
	"entry2button" {
		bind $Widget1 <Down> "
			[list focus $Widget2]
			[list $Widget2 flash]"
		bind $Widget2 <Up> "
			[list focus $Widget1]"
	}
	"button2entry" {
		bind $Widget1 <Down> "
			[list focus $Widget2]"
		bind $Widget2 <Up> "
			[list focus $Widget1]
			[list $Widget1 flash]"
	}
	"button2button" {
		bind $Widget1 <Escape> {+ break}
		bind $Widget1 <Return> {+ ; break}
		bind $Widget1 <space> {%W invoke; break}
		bind $Widget1 <Down> "
			[list focus $Widget2]
			[list $Widget2 flash]"
		bind $Widget2 <Escape> {+ ; break}
		bind $Widget2 <Return> {+ ; break}
		bind $Widget2 <space> {%W invoke; break}
		bind $Widget2 <Up> "
			[list focus $Widget1]
			[list $Widget1 flash]"
	}
	}
}

