#include "xgrafixint.h"
#include <stdlib.h>

/****************************************************************/

int C_UpdateCrosshairProc(ClientData cl, Tcl_Interp *interp, int argc,
			  const char **argv)
{
  double xCoord=0,yCoord=0;
  int x,y;
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  x=atoi(argv[2]);
  y=atoi(argv[3]);

  if (x < theCrosshairx1)
    x = theCrosshairx1;
  else if (x > theCrosshairx2)
    x = theCrosshairx2;

  if (y < theCrosshairy1)
    y = theCrosshairy1;
  else if (y > theCrosshairy2)
    y = theCrosshairy2;

  /*****************************/
  /* Updating the Coordinates. */

  if (Is_X_Log(theWindow))
    xCoord=pow(10.0, x * theCrosshairmx + theCrosshairbx);
  else
    xCoord=x * theCrosshairmx + theCrosshairbx;

  if (Is_Y_Log(theWindow))
    yCoord=pow(10.0, y * theCrosshairmy + theCrosshairby);
  else
    yCoord=y * theCrosshairmy + theCrosshairby;

  sprintf(TclCommand,"%g %g\n",xCoord,yCoord);
  interp->result=TclCommand;
  return TCL_OK;
}

/****************************************************************/

int C_SetCrosshairParametersProc(ClientData cl, Tcl_Interp *interp, int argc,
				 const char **argv)
{
  LabelType       label_Ptr;
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  theCrosshairx1 = theWindow->ulxc + 1;
  theCrosshairy1 = theWindow->ulyc + 1;
  theCrosshairx2 = theWindow->lrxc - 1;
  theCrosshairy2 = theWindow->lryc - 1;

  label_Ptr = theWindow->label;

  if (Is_X_Log(theWindow)) {
    theCrosshairmx = log10(max(label_Ptr->X_Max / (label_Ptr->X_Min+DBL_MIN),
			       DBL_MIN)) / (theCrosshairx2 - theCrosshairx1 + 
					    DBL_MIN);
    theCrosshairbx = (log10(max(label_Ptr->X_Min, DBL_MIN)) * theCrosshairx2
		      - log10(max(label_Ptr->X_Max, DBL_MIN)) * 
		      theCrosshairx1) / (theCrosshairx2 - theCrosshairx1 + 
					 DBL_MIN);
  } else {
    theCrosshairmx = (label_Ptr->X_Max - label_Ptr->X_Min) / (theCrosshairx2 - theCrosshairx1 + DBL_MIN);
    theCrosshairbx = (label_Ptr->X_Min * theCrosshairx2
    - label_Ptr->X_Max * theCrosshairx1) / (theCrosshairx2 - theCrosshairx1 + DBL_MIN);
  }
  if (Is_Y_Log(theWindow)) {
    theCrosshairmy = log10(max(label_Ptr->Y_Max / (label_Ptr->Y_Min + DBL_MIN),
			       DBL_MIN)) / (theCrosshairy1 - theCrosshairy2 + 
					    DBL_MIN);
    theCrosshairby = (log10(max(label_Ptr->Y_Min, DBL_MIN)) * theCrosshairy1
		      - log10(max(label_Ptr->Y_Max, DBL_MIN)) * 
		      theCrosshairy2) / (theCrosshairy1 - theCrosshairy2 + 
					 DBL_MIN);
  } else {
    theCrosshairmy = (label_Ptr->Y_Max - label_Ptr->Y_Min) / (theCrosshairy1 - theCrosshairy2 + DBL_MIN);
    theCrosshairby = (label_Ptr->Y_Min * theCrosshairy1
    - label_Ptr->Y_Max * theCrosshairy2) / (theCrosshairy1 - theCrosshairy2 + DBL_MIN);
  }

  return TCL_OK;
}
