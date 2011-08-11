set XGrafixIcon @$XGrafixPath/xgrafix.ico

set WinData() 0
#############################
# First index is window path.  Second index is of the following fields 
#############################
# geometry
# type
# title
# index
# open
# xMin
# xMax
# xAuto
# xLinLog
# yMin
# yMax
# yAuto
# yLinLog
# zMin
# zMax
# zAuto
# zLinLog
# origxMin
# origxMax
# origxAuto
# origxLinLog
# origyMin
# origyMax
# origyAuto
# origyLinLog
# origzMin
# origzMax
# origzAuto
# origzLinLog
# trace
# PSFile
# plot_title
# printType
# printMode
# printLimit
# printPeriod
# printNumber
# theta
# phi
# shading
# color
# grid
# xCoord
# yCoord
# xlabel
# ylabel


set SpecialData() 0
############################
# First index is the index in array.  Second index is of the following fields
############################
# varName
# varValue
# type
# varValueOld

set numberOfSpecials 0

set time 0.000

set ThreeD 3
set TwoD 2
set VectorD v

set WaitVariable 0

set InputFile ""
set InputFileCreated 0

set SaveFile output.dmp
set SaveFileOld ""

set SearchVar ""

set default3DHeight 400
set default3DWidth 475
set defaultVectorHeight 400
set defaultVectorWidth 475
set defaultWinHeight 300
set defaultWinWidth 400

set defaultDialogBoxX 400
set defaultDialogBoxY 400
append defaultDialogBoxGeometry + $defaultDialogBoxX + $defaultDialogBoxY

set defaultMainMenuX 10
set defaultMainMenuY 40
set defaultMainMenuWidth 200

set openWindowsList {}
set windowsList {}

set rescaleTitle ""
set rescaleXMin 0
set rescaleXMax 0
set rescaleAutoX 0
set rescaleXType 0
set rescaleYMin 0
set rescaleYMax 0
set rescaleAutoY 0
set rescaleXType 0
set rescaleZMin 0
set rescaleZMax 0
set rescaleAutoZ 0
set rescaleZType 0
set rescaleWindowPath ""

set printType ""
set printMode ""
set printPeriod 0
set printLimit 0
set printTitle ""
set printFile ""
set printPlotTitle ""
set printWindowPath ""
set printNumber 1

set 3dTitle ""
set 3dTheta 0
set 3dPhi 0
set 3dShading 0
set 3dColor 0
set 3dGrid 0
set 3dWindowPath ""

set xTemp1 0
set xTemp2 0
set yTemp1 0
set yTemp2 0

set WasRunning 0

set Changed 0

set theCurrentStep 0

option add *background gray60
option add *activebackground gray70
option add *activeforeground gray70
option add *Button.cursor hand2
option add *Radiobutton.cursor hand2

