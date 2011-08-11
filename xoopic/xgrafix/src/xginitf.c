#include "xgrafixint.h"
#include <stdlib.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifdef UNICOS
extern void XGMAINLOOP(void);
#else
#ifdef IBM
extern void xgmainloop(void);
#else
#ifdef HP
extern void xgmainloop(void);
#else
extern void xgmainloop_(void);
#endif
#endif
#endif

/***********************************************************************/
/* FORTRAN callable routines */

/***********************************************************************/

void
#ifdef UNICOS
INITXG(number_of_names,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n16,thetime)
#else
#ifdef IBM
initxg(number_of_names,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n16,thetime)
#else
#ifdef HP
initxg(number_of_names,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n16,thetime)
#else
initxg_(number_of_names,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n16,thetime)
#endif
#endif
#endif
     int            *number_of_names;
     double         *thetime;
     char           *n1,*n2,*n3,*n4,*n5,*n6,*n7,*n8,*n9,*n10,*n11,*n12,*n13,*n14,*n15,*n16;
     
{
  char     *names[16];
  int      i;
  xindex_3d_plots = 0;
  yindex_3d_plots = 0;
  zindex_3d_plots = 0;
  /* Write the NAMEn strings into the NAMES array. */
  names[0] = n1;
  names[1] = n2;
  names[2] = n3;
  names[3] = n4;
  names[4] = n5;
  names[5] = n6;
  names[6] = n7;
  names[7] = n8;
  names[8] = n9;
  names[9] = n10;
  names[10] = n11;
  names[11] = n12;
  names[12] = n13;
  names[13] = n14;
  names[14] = n15;
  names[15] = n16;
 
  /* Initialize the XGrafix package. */
  XGInit(*number_of_names, names, thetime);
}

/*****************************************************************/

void
#ifdef UNICOS
STARTXG()
#else
#ifdef IBM
startxg()
#else
#ifdef HP
startxg()
#else
startxg_()
#endif
#endif
#endif
{
 XGStart();
}

/***********************************************************************/


void
#ifdef UNICOS
SETVECXG(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#else
#ifdef IBM
setvecxg(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#else
#ifdef HP
setvecxg(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#else
setvecxg_(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	  X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#endif
#endif
#endif
     char           *PlotType;/* "vecvec"                                      */
     char           *X_Label;	/* x label for the frame                         */
     char           *Y_Label;	/* y label for the frame                         */
     char           *Z_Label;	/* z label for the frame                         */
     char           *State;	/* "open" or "iconic"                            */
     int            *ulx,*uly;/* requested position of frame's upper left coner*/
     SCALAR          *X_Scale;	/* scaling factor for the x array                */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                */
     int            *X_Auto_Rescale;/* if True X_Min and X_Max are neglected   */
     int            *Y_Auto_Rescale;/* if True Y_Min and Y_Max are neglected   */
     SCALAR          *X_Min, *X_Max;	/* x bounds for the plot if x-autorescale if False */
     SCALAR          *Y_Min, *Y_Max;	/* y bounds for the plot if y-autorescale if False */
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Auto_Rescale = 0;

  label_ptr->Y_Label = X_Label;
  label_ptr->Y_Min = *X_Min;
  label_ptr->Y_Max = *X_Max;
  label_ptr->Y_Scale = *X_Scale;
  label_ptr->Y_Auto_Rescale = *X_Auto_Rescale;
  
  label_ptr->X_Label = Y_Label;
  label_ptr->X_Min = *Y_Min;
  label_ptr->X_Max = *Y_Max;
  label_ptr->X_Scale = *Y_Scale;
  label_ptr->X_Auto_Rescale = *Y_Auto_Rescale;

  if (!strcmp(PlotType, "vecvec"))
    plottype = VEC_VEC;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
    
  XGSetupWindow(Z_Label,State,*ulx,*uly,VECD, data_ptr, label_ptr, plottype);
}

/***********************************************************************/


void
#ifdef UNICOS
SETVECXGFLAG(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, openFlag)
#else
#ifdef IBM
setvecxgflag(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, openFlag)
#else
#ifdef HP
setvecxgflag(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, openFlag)
#else
setvecxgflag_(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale, Y_Scale,
	  X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, openFlag)
#endif
#endif
#endif
     char           *PlotType;/* "vecvec"                                      */
     char           *X_Label;	/* x label for the frame                         */
     char           *Y_Label;	/* y label for the frame                         */
     char           *Z_Label;	/* z label for the frame                         */
     char           *State;	/* "open" or "iconic"                            */
     int            *ulx,*uly;/* requested position of frame's upper left coner*/
     SCALAR          *X_Scale;	/* scaling factor for the x array                */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                */
     int            *X_Auto_Rescale;/* if True X_Min and X_Max are neglected   */
     int            *Y_Auto_Rescale;/* if True Y_Min and Y_Max are neglected   */
     SCALAR         *X_Min, *X_Max;	/* x bounds for the plot if x-autorescale if False */
     SCALAR         *Y_Min, *Y_Max;	/* y bounds for the plot if y-autorescale if False */
     int            *openFlag;
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Auto_Rescale = 0;

  label_ptr->Y_Label = X_Label;
  label_ptr->Y_Min = *X_Min;
  label_ptr->Y_Max = *X_Max;
  label_ptr->Y_Scale = *X_Scale;
  label_ptr->Y_Auto_Rescale = *X_Auto_Rescale;
  
  label_ptr->X_Label = Y_Label;
  label_ptr->X_Min = *Y_Min;
  label_ptr->X_Max = *Y_Max;
  label_ptr->X_Scale = *Y_Scale;
  label_ptr->X_Auto_Rescale = *Y_Auto_Rescale;

  if (!strcmp(PlotType, "vecvec"))
    plottype = VEC_VEC;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
    
  XGSetupWindow(Z_Label,State,*ulx,*uly,VECD, data_ptr, label_ptr, plottype);

  theWindowArray[numberOfWindows -1]->openFlag = openFlag;
}

/***********************************************************************/

void
#ifdef UNICOS
SET2DXG(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#else
#ifdef IBM
set2dxg(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#else
#ifdef HP
set2dxg(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#else
set2dxg_(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max)
#endif
#endif
#endif
     char           *PlotType;/* "linlin", "linlog", "loglog", or "loglin"        */
     char           *X_Label;	/* x label for the frame                            */
     char           *Y_Label;	/* y label for the frame                            */
     SCALAR          *X_Scale;	/* scaling factor for the x array                   */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                   */
     char           *State;	/* "open" or "iconic"                               */
     int            *ulx, *uly;/* requested position of frame's upper left coner  */
     int            *X_Auto_Rescale;/* if True X_Min and X_Max are * neglected    */
     int            *Y_Auto_Rescale;/* if True Y_Min and Y_Max are * neglected    */
     SCALAR          *X_Min, *X_Max;/* x bounds for the plot if x-autorescale if False*/
     SCALAR          *Y_Min, *Y_Max;/* y bounds for the plot if y-autorescale if False*/
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;

  label_ptr = SetupLabelStruct();
  label_ptr->Y_Label = Y_Label;
  label_ptr->Y_Min = *Y_Min;
  label_ptr->Y_Max = *Y_Max;
  label_ptr->Y_Scale = *Y_Scale;
  label_ptr->Y_Auto_Rescale = *Y_Auto_Rescale;

  label_ptr->X_Label = X_Label;
  label_ptr->X_Min = *X_Min;
  label_ptr->X_Max = *X_Max;
  label_ptr->X_Scale = *X_Scale;
  label_ptr->X_Auto_Rescale = *X_Auto_Rescale;

  if (!strcmp(PlotType, "linlin"))      plottype = LIN_LIN;
  else if (!strcmp(PlotType, "linlog")) plottype = LIN_LOG;
  else if (!strcmp(PlotType, "loglog")) plottype = LOG_LOG;
  else if (!strcmp(PlotType, "loglin")) plottype = LOG_LIN;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
  XGSetupWindow(Y_Label,State,*ulx,*uly,TWOD,data_ptr, label_ptr,plottype);
}
/***********************************************************************/

void
#ifdef UNICOS
SET2DXGFLAG(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, openFlag)
#else
#ifdef IBM
set2dxgflag(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, openFlag)
#else
#ifdef HP
set2dxgflag(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max,openFlag)
#else
set2dxgflag_(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	 X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max,openFlag)
#endif
#endif
#endif
     char           *PlotType;/* "linlin", "linlog", "loglog", or "loglin"        */
     char           *X_Label;	/* x label for the frame                            */
     char           *Y_Label;	/* y label for the frame                            */
     SCALAR          *X_Scale;	/* scaling factor for the x array                   */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                   */
     char           *State;	/* "open" or "iconic"                               */
     int            *ulx, *uly;/* requested position of frame's upper left coner  */
     int            *X_Auto_Rescale;/* if True X_Min and X_Max are * neglected    */
     int            *Y_Auto_Rescale;/* if True Y_Min and Y_Max are * neglected    */
     SCALAR          *X_Min, *X_Max;/* x bounds for the plot if x-autorescale if False*/
     SCALAR          *Y_Min, *Y_Max;/* y bounds for the plot if y-autorescale if False*/
     int             *openFlag;
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;

  label_ptr = SetupLabelStruct();
  label_ptr->Y_Label = Y_Label;
  label_ptr->Y_Min = *Y_Min;
  label_ptr->Y_Max = *Y_Max;
  label_ptr->Y_Scale = *Y_Scale;
  label_ptr->Y_Auto_Rescale = *Y_Auto_Rescale;

  label_ptr->X_Label = X_Label;
  label_ptr->X_Min = *X_Min;
  label_ptr->X_Max = *X_Max;
  label_ptr->X_Scale = *X_Scale;
  label_ptr->X_Auto_Rescale = *X_Auto_Rescale;

  if (!strcmp(PlotType, "linlin"))      plottype = LIN_LIN;
  else if (!strcmp(PlotType, "linlog")) plottype = LIN_LOG;
  else if (!strcmp(PlotType, "loglog")) plottype = LOG_LOG;
  else if (!strcmp(PlotType, "loglin")) plottype = LOG_LIN;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
  XGSetupWindow(Y_Label,State,*ulx,*uly,TWOD,data_ptr, label_ptr,plottype);
  theWindowArray[numberOfWindows -1]->openFlag = openFlag;
}

/****************************************************************************/
void
#ifdef UNICOS
SET2DCXG(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#else
#ifdef IBM
set2dcxg(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#else
#ifdef HP
set2dcxg(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#else
set2dcxg_(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	  X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	  Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#endif
#endif
#endif
     char           *PlotType;	/* "linlinlin", "linlinlog", "linloglog", etc.   */
     char           *X_Label;	/* x label for the frame                            */
     char           *Y_Label;	/* y label for the frame                            */
     char           *Z_Label;	/* z label for the frame                            */
     char           *State;	/* "open" or "iconic"                               */
     int            *ulx, *uly;	/* requested position of frame's upper left coner*/
     SCALAR          *X_Scale;	/* scaling factor for the x array                   */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                   */
     SCALAR          *Z_Scale;	/* scaling factor for the z array                   */
     int            *X_Auto_Rescale;	/* if True X_Min and X_Max are neglected   */
     int            *Y_Auto_Rescale;	/* if True Y_Min and Y_Max are neglected   */
     int            *Z_Auto_Rescale;	/* if True Z_Min and Z_Max are neglected   */
     SCALAR          *X_Min, *X_Max;/* x bounds for the plot if x-autorescale if False  */
     SCALAR          *Y_Min, *Y_Max;/* y bounds for the plot if y-autorescale if False  */
     SCALAR          *Z_Min, *Z_Max;/* z bounds for the plot if z-autorescale if False  */
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;

  init_theta = 0.0;
  init_phi = 0.0;

  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Min = *Z_Min;
  label_ptr->Z_Max = *Z_Max;
  label_ptr->Z_Scale = *Z_Scale;
  label_ptr->Z_Auto_Rescale = *Z_Auto_Rescale;

  label_ptr->Y_Label = X_Label;
  label_ptr->Y_Min = *X_Min;
  label_ptr->Y_Max = *X_Max;
  label_ptr->Y_Scale = *X_Scale;
  label_ptr->Y_Auto_Rescale = *X_Auto_Rescale;

  label_ptr->X_Label = Y_Label;
  label_ptr->X_Min = *Y_Min;
  label_ptr->X_Max = *Y_Max;
  label_ptr->X_Scale = *Y_Scale;
  label_ptr->X_Auto_Rescale = *Y_Auto_Rescale;

  if (!strcmp(PlotType, "linlinlin"))      plottype = LIN_LIN_LIN;
  else if (!strcmp(PlotType, "linlinlog")) plottype = LIN_LIN_LOG;
  else if (!strcmp(PlotType, "linloglin")) plottype = LOG_LIN_LIN;
  else if (!strcmp(PlotType, "linloglog")) plottype = LOG_LIN_LOG;
  else if (!strcmp(PlotType, "loglinlin")) plottype = LIN_LOG_LIN;
  else if (!strcmp(PlotType, "loglinlog")) plottype = LIN_LOG_LOG;
  else if (!strcmp(PlotType, "logloglin")) plottype = LOG_LOG_LIN;
  else if (!strcmp(PlotType, "logloglog")) plottype = LOG_LOG_LOG;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Z_Label);
    exit(-1);
  }

  XGSetupWindow(Z_Label,State,*ulx,*uly,THREED,data_ptr,label_ptr, plottype);
}

/***********************************************************************/

void
#ifdef UNICOS
SET2DCXGFLAG(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max,openFlag)
#else
#ifdef IBM
set2dcxgflag(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max,openFlag)
#else
#ifdef HP
set2dcxgflag(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max,openFlag)
#else
set2dcxgflag_(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly,
	  X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	  Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max,openFlag)
#endif
#endif
#endif
     char           *PlotType;	/* "linlinlin", "linlinlog", "linloglog", etc.   */
     char           *X_Label;	/* x label for the frame                            */
     char           *Y_Label;	/* y label for the frame                            */
     char           *Z_Label;	/* z label for the frame                            */
     char           *State;	/* "open" or "iconic"                               */
     int            *ulx, *uly;	/* requested position of frame's upper left coner*/
     SCALAR          *X_Scale;	/* scaling factor for the x array                   */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                   */
     SCALAR          *Z_Scale;	/* scaling factor for the z array                   */
     int            *X_Auto_Rescale;	/* if True X_Min and X_Max are neglected   */
     int            *Y_Auto_Rescale;	/* if True Y_Min and Y_Max are neglected   */
     int            *Z_Auto_Rescale;	/* if True Z_Min and Z_Max are neglected   */
     SCALAR          *X_Min, *X_Max;/* x bounds for the plot if x-autorescale if False  */
     SCALAR          *Y_Min, *Y_Max;/* y bounds for the plot if y-autorescale if False  */
     SCALAR          *Z_Min, *Z_Max;/* z bounds for the plot if z-autorescale if False  */
     int             *openFlag;
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;

  init_theta = 0.0;
  init_phi = 0.0;

  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Min = *Z_Min;
  label_ptr->Z_Max = *Z_Max;
  label_ptr->Z_Scale = *Z_Scale;
  label_ptr->Z_Auto_Rescale = *Z_Auto_Rescale;

  label_ptr->Y_Label = X_Label;
  label_ptr->Y_Min = *X_Min;
  label_ptr->Y_Max = *X_Max;
  label_ptr->Y_Scale = *X_Scale;
  label_ptr->Y_Auto_Rescale = *X_Auto_Rescale;

  label_ptr->X_Label = Y_Label;
  label_ptr->X_Min = *Y_Min;
  label_ptr->X_Max = *Y_Max;
  label_ptr->X_Scale = *Y_Scale;
  label_ptr->X_Auto_Rescale = *Y_Auto_Rescale;

  if (!strcmp(PlotType, "linlinlin"))      plottype = LIN_LIN_LIN;
  else if (!strcmp(PlotType, "linlinlog")) plottype = LIN_LIN_LOG;
  else if (!strcmp(PlotType, "linloglin")) plottype = LOG_LIN_LIN;
  else if (!strcmp(PlotType, "linloglog")) plottype = LOG_LIN_LOG;
  else if (!strcmp(PlotType, "loglinlin")) plottype = LIN_LOG_LIN;
  else if (!strcmp(PlotType, "loglinlog")) plottype = LIN_LOG_LOG;
  else if (!strcmp(PlotType, "logloglin")) plottype = LOG_LOG_LIN;
  else if (!strcmp(PlotType, "logloglog")) plottype = LOG_LOG_LOG;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Z_Label);
    exit(-1);
  }

  XGSetupWindow(Z_Label,State,*ulx,*uly,THREED,data_ptr,label_ptr, plottype);
  theWindowArray[numberOfWindows -1]->openFlag = openFlag;
}
/***********************************************************************/

void
#ifdef UNICOS
SET3DXG(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#else
#ifdef IBM
set3dxg(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#else
#ifdef HP
set3dxg(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#else
set3dxg_(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max)
#endif
#endif
#endif
     char           *PlotType;	/* "linlinlin", "linlinlog", "linloglog", etc.   */
     char           *X_Label;	/* x label for the frame                            */
     char           *Y_Label;	/* y label for the frame                            */
     char           *Z_Label;	/* z label for the frame                            */
     SCALAR          *Theta, *Phi;	/* initial viewing angles                     */
     char           *State;	/* "open" or "iconic"                               */
     int            *ulx, *uly;	/* requested position of frame's upper left coner*/
     SCALAR          *X_Scale;	/* scaling factor for the x array                   */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                   */
     SCALAR          *Z_Scale;	/* scaling factor for the z array                   */
     int            *X_Auto_Rescale;	/* if True X_Min and X_Max are neglected   */
     int            *Y_Auto_Rescale;	/* if True Y_Min and Y_Max are neglected   */
     int            *Z_Auto_Rescale;	/* if True Z_Min and Z_Max are neglected   */
     SCALAR          *X_Min, *X_Max; /* x bounds for the plot if x-autorescale if False */
     SCALAR          *Y_Min, *Y_Max; /* y bounds for the plot if y-autorescale if False */
     SCALAR          *Z_Min, *Z_Max; /* z bounds for the plot if z-autorescale if False */
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  init_theta = *Theta;
  init_phi = *Phi;

  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Min = *Z_Min;
  label_ptr->Z_Max = *Z_Max;
  label_ptr->Z_Scale = *Z_Scale;
  label_ptr->Z_Auto_Rescale = *Z_Auto_Rescale;

  label_ptr->Y_Label = X_Label;
  label_ptr->Y_Min = *X_Min;
  label_ptr->Y_Max = *X_Max;
  label_ptr->Y_Scale = *X_Scale;
  label_ptr->Y_Auto_Rescale = *X_Auto_Rescale;

  label_ptr->X_Label = Y_Label;
  label_ptr->X_Min = *Y_Min;
  label_ptr->X_Max = *Y_Max;
  label_ptr->X_Scale = *Y_Scale;
  label_ptr->X_Auto_Rescale = *Y_Auto_Rescale;

  if (!strcmp(PlotType, "linlinlin"))      plottype = LIN_LIN_LIN;
  else if (!strcmp(PlotType, "linlinlog")) plottype = LIN_LIN_LOG;
  else if (!strcmp(PlotType, "linloglin")) plottype = LOG_LIN_LIN;
  else if (!strcmp(PlotType, "linloglog")) plottype = LOG_LIN_LOG;
  else if (!strcmp(PlotType, "loglinlin")) plottype = LIN_LOG_LIN;
  else if (!strcmp(PlotType, "loglinlog")) plottype = LIN_LOG_LOG;
  else if (!strcmp(PlotType, "logloglin")) plottype = LOG_LOG_LIN;
  else if (!strcmp(PlotType, "logloglog")) plottype = LOG_LOG_LOG;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Z_Label);
    exit(-1);
  }
  
  XGSetupWindow(Z_Label,State,*ulx,*uly,THREED,data_ptr,label_ptr,plottype);
}
/***********************************************************************/

void
#ifdef UNICOS
SET3DXGFLAG(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max, openFlag)
#else
#ifdef IBM
set3dxgflag(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max, openFlag)
#else
#ifdef HP
set3dxgflag(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max, openFlag)
#else
set3dxgflag_(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	 X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	 Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max, openFlag)
#endif
#endif
#endif
     char           *PlotType;	/* "linlinlin", "linlinlog", "linloglog", etc.   */
     char           *X_Label;	/* x label for the frame                            */
     char           *Y_Label;	/* y label for the frame                            */
     char           *Z_Label;	/* z label for the frame                            */
     SCALAR          *Theta, *Phi;	/* initial viewing angles                     */
     char           *State;	/* "open" or "iconic"                               */
     int            *ulx, *uly;	/* requested position of frame's upper left coner*/
     SCALAR          *X_Scale;	/* scaling factor for the x array                   */
     SCALAR          *Y_Scale;	/* scaling factor for the y array                   */
     SCALAR          *Z_Scale;	/* scaling factor for the z array                   */
     int            *X_Auto_Rescale;	/* if True X_Min and X_Max are neglected   */
     int            *Y_Auto_Rescale;	/* if True Y_Min and Y_Max are neglected   */
     int            *Z_Auto_Rescale;	/* if True Z_Min and Z_Max are neglected   */
     SCALAR          *X_Min, *X_Max; /* x bounds for the plot if x-autorescale if False */
     SCALAR          *Y_Min, *Y_Max; /* y bounds for the plot if y-autorescale if False */
     SCALAR          *Z_Min, *Z_Max; /* z bounds for the plot if z-autorescale if False */
     int             *openFlag;
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  init_theta = *Theta;
  init_phi = *Phi;

  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Min = *Z_Min;
  label_ptr->Z_Max = *Z_Max;
  label_ptr->Z_Scale = *Z_Scale;
  label_ptr->Z_Auto_Rescale = *Z_Auto_Rescale;

  label_ptr->Y_Label = X_Label;
  label_ptr->Y_Min = *X_Min;
  label_ptr->Y_Max = *X_Max;
  label_ptr->Y_Scale = *X_Scale;
  label_ptr->Y_Auto_Rescale = *X_Auto_Rescale;

  label_ptr->X_Label = Y_Label;
  label_ptr->X_Min = *Y_Min;
  label_ptr->X_Max = *Y_Max;
  label_ptr->X_Scale = *Y_Scale;
  label_ptr->X_Auto_Rescale = *Y_Auto_Rescale;

  if (!strcmp(PlotType, "linlinlin"))      plottype = LIN_LIN_LIN;
  else if (!strcmp(PlotType, "linlinlog")) plottype = LIN_LIN_LOG;
  else if (!strcmp(PlotType, "linloglin")) plottype = LOG_LIN_LIN;
  else if (!strcmp(PlotType, "linloglog")) plottype = LOG_LIN_LOG;
  else if (!strcmp(PlotType, "loglinlin")) plottype = LIN_LOG_LIN;
  else if (!strcmp(PlotType, "loglinlog")) plottype = LIN_LOG_LOG;
  else if (!strcmp(PlotType, "logloglin")) plottype = LOG_LOG_LIN;
  else if (!strcmp(PlotType, "logloglog")) plottype = LOG_LOG_LOG;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Z_Label);
    exit(-1);
  }
  
  XGSetupWindow(Z_Label,State,*ulx,*uly,THREED,data_ptr,label_ptr,plottype);
  theWindowArray[numberOfWindows -1]->openFlag = openFlag;
}

/***********************************************************************/

void
#ifdef UNICOS
CURVEXG(x_array, y_array, npoints, color)
#else
#ifdef IBM
curvexg(x_array, y_array, npoints, color)
#else
#ifdef HP
curvexg(x_array, y_array, npoints, color)
#else
curvexg_(x_array, y_array, npoints, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                       */
     SCALAR          *y_array;	/* y_array to be plotted                       */
     int            *npoints;	/* number of points in the x (and y) direction */
     int            *color;	/* plot's color chosen from 0-9                */
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;
  
  theWindowArray[numberOfWindows -1]->data = data_ptr;
}

void
#ifdef UNICOS
SCAT2DXG(x_array, y_array, npoints, color)
#else
#ifdef IBM
scat2dxg(x_array, y_array, npoints, color)
#else
#ifdef HP
scat2dxg(x_array, y_array, npoints, color)
#else
scat2dxg_(x_array, y_array, npoints, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                       */
     SCALAR          *y_array;	/* y_array to be plotted                       */
     int            *npoints;	/* number of points in the x (and y) direction */
     int            *color;	/* plot's color chosen from 0-9                */
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;
  Set_Scatter(data_ptr);
  
  theWindowArray[numberOfWindows -1]->data = data_ptr;
}

/***********************************************************************/

void
#ifdef UNICOS
SCAT3DXG(x_array, y_array, z_array, npoints, color)
#else
#ifdef IBM
scat3dxg(x_array, y_array, z_array, npoints, color)
#else
#ifdef HP
scat3dxg(x_array, y_array, z_array, npoints, color)
#else
scat3dxg_(x_array, y_array, z_array, npoints, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                        */
     SCALAR          *y_array;	/* y_array to be plotted                        */
     SCALAR          *z_array;	/* z_array to be plotted                        */
     int            *npoints;	/* number of points in the x, y and z direction */
     int            *color;	/* plot's color chosen from 0-9                 */
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->zscat = z_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->mpoints = npoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;
  Set_Scatter(data_ptr);
  
  theWindowArray[numberOfWindows -1]->data = data_ptr;
}

/***********************************************************************/

void
#ifdef UNICOS
VECTORXG(x_array, y_array, z_array, w_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef IBM
vectorxg(x_array, y_array, z_array, w_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef HP
vectorxg(x_array, y_array, z_array, w_array, mpoints, npoints, ndim1, ndim2, color)
#else
vectorxg_(x_array, y_array, z_array, w_array, mpoints, npoints, ndim1, ndim2, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                 */
     SCALAR          *y_array;	/* y_array to be plotted                 */
     SCALAR          *z_array;	/* x component of Vector                 */
     SCALAR          *w_array;	/* y component of Vector                 */
     int            *mpoints;	/* number of points in the x direction   */
     int            *npoints;	/* number of points in the y direction   */
     int            *ndim1;   /* dimensions of the 1st index of z_array */
     int            *ndim2;   /* dimensions of the 2nd index of z_array */
     int            *color;	/* plot's color chosen from 0-9          */
{
  int             j;
  
  DataType        data_prevptr, data_ptr;
  
  /* Allocate the 2-D FORTRAN to C conversion arrays within the 3-D array ZPLOT3D. */
  if (++zindex_3d_plots > MAX_3D_PLOTS ||
      ++windex_3d_plots > MAX_3D_PLOTS) {
    puts("Number of initialized 3-D plots exceeds established allocation.");
    exit(-1);
  }
  zplot3d[zindex_3d_plots - 1] = (SCALAR **) malloc((*ndim2)*sizeof(SCALAR *));
  wplot3d[windex_3d_plots - 1] = (SCALAR **) malloc((*ndim2)*sizeof(SCALAR *));
  for (j = 0; j < (*ndim2); j++) {
    zplot3d[zindex_3d_plots - 1][j] = &z_array[j*(*ndim1)];
    wplot3d[windex_3d_plots - 1][j] = &w_array[j*(*ndim1)];
  }
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = zplot3d[zindex_3d_plots - 1];
  data_ptr->w = wplot3d[windex_3d_plots - 1];
  data_ptr->y = x_array;
  data_ptr->x = y_array;
  data_ptr->mpoints = npoints;
  data_ptr->npoints = mpoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;
  Set_Vector(data_ptr);

  theWindowArray[numberOfWindows -1]->data = data_ptr;
}

/***********************************************************************/

void
#ifdef UNICOS
SURFXG(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef IBM
surfxg(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef HP
surfxg(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
surfxg_(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                  */
     SCALAR          *y_array;	/* y_array to be plotted                  */
     SCALAR          *z_array;	/* z_array to be plotted                  */
     int            *mpoints;	/* number of points in the x direction    */
     int            *npoints;	/* number of points in the y direction    */
     int            *ndim1;   /* dimensions of the 1st index of z_array */
     int            *ndim2;   /* dimensions of the 2nd index of z_array */
     int            *color;	/* plot's color chosen from 0-9           */
{
  int             j;
  DataType        data_prevptr, data_ptr;
  
  /* Allocate the 2-D FORTRAN to C conversion arrays within the 3-D array ZPLOT3D. */
  if (++zindex_3d_plots > MAX_3D_PLOTS) {
    puts("Number of initialized 3-D plots exceeds established allocation.");
    exit(-1);
  }
  zplot3d[zindex_3d_plots - 1] = (SCALAR **) malloc((*ndim2)*sizeof(SCALAR *));
  for (j = 0; j < (*ndim2); j++)
    zplot3d[zindex_3d_plots - 1][j] = &z_array[j*(*ndim1)];
  
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = zplot3d[zindex_3d_plots - 1];
  data_ptr->y = x_array;
  data_ptr->x = y_array;
  data_ptr->mpoints = npoints;
  data_ptr->npoints = mpoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;
  theWindowArray[numberOfWindows -1]->data = data_ptr;
}
/***********************************************************************/

void
#ifdef UNICOS
CONTXG(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef IBM
contxg(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef HP
contxg(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
contxg_(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                  */
     SCALAR          *y_array;	/* y_array to be plotted                  */
     SCALAR          *z_array;	/* z_array to be plotted                  */
     int            *mpoints;	/* number of points in the x direction    */
     int            *npoints;	/* number of points in the y direction    */
     int            *ndim1;   /* dimensions of the 1st index of z_array */
     int            *ndim2;   /* dimensions of the 2nd index of z_array */
     int            *color;	/* plot's color chosen from 0-9           */
{
  int             j;
  
  DataType        data_prevptr, data_ptr;
  
  /* Allocate the 2-D FORTRAN to C conversion arrays within the 3-D array ZPLOT3D. */
  if (++zindex_3d_plots > MAX_3D_PLOTS) {
    puts("Number of initialized 3-D plots exceeds established allocation.");
    exit(-1);
  }
  zplot3d[zindex_3d_plots - 1] = (SCALAR **) malloc((*ndim2) * sizeof(SCALAR *));
  for (j = 0; j < (*ndim2); j++)
    zplot3d[zindex_3d_plots - 1][j] = &z_array[j*(*ndim1)];
  
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = zplot3d[zindex_3d_plots - 1];
  data_ptr->y = x_array;
  data_ptr->x = y_array;
  data_ptr->mpoints = npoints;
  data_ptr->npoints = mpoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;

  
  theWindowArray[numberOfWindows -1]->data = data_ptr;
}

/***********************************************************************/

void
#ifdef UNICOS
IRSURFXG(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef IBM
irsurfxg(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
#ifdef HP
irsurfxg(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#else
irsurfxg_(x_array, y_array, z_array, mpoints, npoints, ndim1, ndim2, color)
#endif
#endif
#endif
     SCALAR          *x_array;	/* x_array to be plotted                  */
     SCALAR          *y_array;	/* y_array to be plotted                  */
     SCALAR          *z_array;	/* z_array to be plotted                  */
     int            *mpoints;	/* number of points in the x direction    */
     int            *npoints;	/* number of points in the y direction    */
     int            *ndim1;   /* dimensions of the 1st index of z_array */
     int            *ndim2;   /* dimensions of the 2nd index of z_array */
     int            *color;	/* plot's color chosen from 0-9           */
{
  int             j;
  
  DataType        data_prevptr, data_ptr;
  
  /* Allocate the 2-D FORTRAN to C conversion arrays within the 3-D array ZPLOT3D. */
  if (++xindex_3d_plots > MAX_3D_PLOTS ||
      ++yindex_3d_plots > MAX_3D_PLOTS ||
      ++zindex_3d_plots > MAX_3D_PLOTS) {
    puts("Number of initialized 3-D plots exceeds established allocation.");
    exit(-1);
  }
  xplot3d[xindex_3d_plots - 1] = (SCALAR **) malloc((*ndim2)*sizeof(SCALAR *));
  yplot3d[yindex_3d_plots - 1] = (SCALAR **) malloc((*ndim2)*sizeof(SCALAR *));
  zplot3d[zindex_3d_plots - 1] = (SCALAR **) malloc((*ndim2)*sizeof(SCALAR *));
  for (j = 0; j < (*ndim2); j++) {
    xplot3d[xindex_3d_plots - 1][j] = &x_array[j*(*ndim1)];
    yplot3d[yindex_3d_plots - 1][j] = &y_array[j*(*ndim1)];
    zplot3d[zindex_3d_plots - 1][j] = &z_array[j*(*ndim1)];
  }
  data_prevptr = theWindowArray[numberOfWindows -1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = zplot3d[zindex_3d_plots - 1];
  data_ptr->y_irr = xplot3d[xindex_3d_plots - 1];
  data_ptr->x_irr = yplot3d[yindex_3d_plots - 1];
  data_ptr->mpoints = npoints;
  data_ptr->npoints = mpoints;
  data_ptr->color = *color;
  data_ptr->next = data_prevptr;
  Set_Irr_Surf(data_ptr);

 theWindowArray[numberOfWindows -1]->data = data_ptr;
}
/***********************************************************************/

void
#ifdef UNICOS
STRUCTUREXGARRAY(numpoints,fillstatus,linecolor,fillcolor,struct_array)
#else
#ifdef IBM
structurexgarray(numpoints,fillstatus,linecolor,fillcolor,struct_array)
#else
#ifdef HP
structurexgarray(numpoints,fillstatus,linecolor,fillcolor,struct_array)
#else
structurexgarray_(numpoints,fillstatus,linecolor,fillcolor,struct_array)
#endif
#endif
#endif
  int      *numpoints;
  int      *fillstatus;
  int      *linecolor;
  int      *fillcolor;
  double   *struct_array;
{
  int i;
  STRUCT_FILL fillFlag;
  StructType *structure = (StructType *)malloc(sizeof(StructType));

  structure->x = (double *)malloc(*numpoints * sizeof(double));
  structure->y = (double *)malloc(*numpoints * sizeof(double));
  structure->numberPoints = *numpoints;
  structure->fillFlag = *fillstatus;
  structure->lineColor = *linecolor;
  structure->fillColor = *fillcolor;

  for (i=0;i< *numpoints;i++) {
    structure->x[i] = struct_array[2*i];
    structure->y[i] = struct_array[2*i + 1];
  }

  structure->next = theNewWindow->structures;
  theNewWindow->structures = structure;
}

/*************************************************************************/
void
#ifdef UNICOS
NEWINTSETUP(newint,name)
#else
#ifdef IBM
newintsetup(newint,name)
#else
#ifdef HP
newintsetup(newint,name)
#else
newintsetup_(newint,name)
#endif
#endif
#endif
  int      *newint;
  char     *name;
{
 SetUpNewVar((void*) newint,name,"integer");
 }

/**********************************************************************/  
void
#ifdef UNICOS
NEWREALSETUP(newreal,name)
#else
#ifdef IBM
newrealsetup(newreal,name)
#else
#ifdef HP
newrealsetup(newreal,name)
#else
newrealsetup_(newreal,name)
#endif
#endif
#endif
  float    *newreal;
  char     *name;
{
 SetUpNewVar((void*) newreal,name,"float");
 }

/***********************************************************************/
void
#ifdef UNICOS
NEWDOUBLESETUP(newdouble,name)
#else
#ifdef IBM
newdoublesetup(newdouble,name)
#else
#ifdef HP
newdoublesetup(newdouble,name)
#else
newdoublesetup_(newdouble,name)
#endif
#endif
#endif
  double   *newdouble;
  char     *name;
{
 SetUpNewVar((void*) newdouble,name,"double");
 }
/*************************************************************************/
void
#ifdef UNICOS
NEWCHARSETUP(newchar,name)
#else
#ifdef IBM
newcharsetup(newchar,name)
#else
#ifdef HP
newcharsetup(newchar,name)
#else
newcharsetup_(newchar,name)
#endif
#endif
#endif
  char     *newchar;
  char     *name;
{
 SetUpNewVar((void*) newchar,name,"char");
 }

/************************************************************************/
void XGMainLoop(){
#ifdef UNICOS
    XGMAINLOOP();
#else
#ifdef IBM
    xgmainloop();
#else
#ifdef HP
    xgmainloop();
#else
    xgmainloop_();
#endif
#endif
#endif
 }



