#include "xgrafixint.h"

void RedrawWindow(WindowType theWindow)
{
  /* printf("enter c redraw window  \n"); */
  if (!(theWindow->pixmap_buffer))
    XG_Setup_Pixmap(theWindow);
  (*(theWindow->paint_function)) (theWindow);
  if (Is_Crosshair_On(theWindow) && !Is_Irr_Surf(theWindow->data) &&
      !Is_Scatter(theWindow->data)) {
    DrawCrosshair(theWindow,theWindow->data->xCoord,theWindow->data->yCoord);
  }
  else
    XG_Copy_Pixmap(theWindow);
}

void EventProc(ClientData cl, XEvent *eventPtr)
{
  WindowType theWindow = (WindowType) cl;
  if (theRunFlag == TRUE) {
    theRunFlag = FALSE;
    Tcl_Eval(interp,"update idletasks");
    XG_Setup_Pixmap(theWindow);
    Iconic_Before(theWindow);
    RedrawWindow(theWindow);
    theRunFlag = TRUE;
    Tcl_Eval(interp,"update idletasks");
#ifdef Tcl75_Tk41
    Tcl_DoWhenIdle(DoMain,(ClientData)NULL);
#endif
#ifdef Tcl74_Tk40
    Tk_DoWhenIdle(DoMain,(ClientData)NULL);
#endif
  } else {
    Tcl_Eval(interp,"update idletasks");
    XG_Setup_Pixmap(theWindow);
    Iconic_Before(theWindow);
    RedrawWindow(theWindow);
    Tcl_Eval(interp,"update idletasks");
  }
}

void XGSetupWindow(const char *Win_Title, const char *State, int ulx, int uly, int WinType,
		   DataType data_ptr, LabelType label_ptr, int PlotType)
{
  char type;

  if (!(theNewWindow = (WindowType) malloc(sizeof(struct window_struct)))) {
    puts("XGrafix(SetupWindow): Not enough memory to allocate a new window");
  }

  theNewWindow->ulxc = ulx;
  theNewWindow->ulyc = uly;
  theNewWindow->state = 0;
  theNewWindow->data = data_ptr;
  theNewWindow->label = label_ptr;
  theNewWindow->theta = init_theta;
  theNewWindow->phi = init_phi;
  theNewWindow->pixmap_buffer = 0;
  theNewWindow->theMesh = 0;
  theNewWindow->structures = 0;
  theNewWindow->c1 = 0;
  theNewWindow->c2 = 0;
  theNewWindow->d1 = 0;
  theNewWindow->d2 = 0;
  theNewWindow->mstart = -1;
  theNewWindow->mend = -1;
  theNewWindow->nstart = -1;
  theNewWindow->nend = -1;
  theNewWindow->xSize = -1;
  theNewWindow->ySize = -1;
  theNewWindow->openFlag = 0;

  if(numberOfWindows > sizeOfWindowArray-1) {
    ReallocateWindows();
  }

  theWindowArray[numberOfWindows++] = theNewWindow;

  strcpy(theNewWindow->eps_file_name, theEPSFile);
  if (WinType == THREED) {
    theNewWindow->type = type = '3';
    strcpy(theNewWindow->plot_title, label_ptr->Z_Label);
    Set_ThreeD(theNewWindow);
    ColorCode_On(theNewWindow);
    Grid_On(theNewWindow);
    theNewWindow->paint_function = Paint_ThreeD_Window;
    theNewWindow->print_function = PostScript_ThreeD_Window;
    theNewWindow->ascii_print_function = Ascii_ThreeD_Window;
	 theNewWindow->xgrafix_print_function = Bin_ThreeD_Window;
    if (PlotType == LOG_LIN_LIN || PlotType == LOG_LIN_LOG ||
	PlotType == LOG_LOG_LIN || PlotType == LOG_LOG_LOG)
      Set_X_Log(theNewWindow);
    if (PlotType == LIN_LOG_LIN || PlotType == LIN_LOG_LOG ||
	PlotType == LOG_LOG_LIN || PlotType == LOG_LOG_LOG)
      Set_Y_Log(theNewWindow);
    if (PlotType == LIN_LIN_LOG || PlotType == LIN_LOG_LOG ||
	PlotType == LOG_LIN_LOG || PlotType == LOG_LOG_LOG)
      Set_Z_Log(theNewWindow);
  } else if (WinType == VECD) {
    theNewWindow->type = type = 'v';
    strcpy(theNewWindow->plot_title, label_ptr->Z_Label);
    theNewWindow->paint_function = Paint_Vector_Window;
    theNewWindow->print_function = PostScript_Vector_Window;
    theNewWindow->ascii_print_function = Ascii_Vector_Window;
	 theNewWindow->xgrafix_print_function = Bin_Vector_Window;
  } else {
    theNewWindow->type = type = '2';
    strcpy(theNewWindow->plot_title, label_ptr->Y_Label);
    theNewWindow->paint_function = Paint_Window;
    theNewWindow->print_function = PostScriptOpenWindow;
    theNewWindow->ascii_print_function = Ascii_TwoD_Window;
	 theNewWindow->xgrafix_print_function = Bin_TwoD_Window;
    if (PlotType == LOG_LIN || PlotType == LOG_LOG)
      Set_X_Log(theNewWindow);
    if (PlotType == LIN_LOG || PlotType == LOG_LOG)
      Set_Y_Log(theNewWindow);
  }

  if (label_ptr->Z_Auto_Rescale)
    Z_AutoRescale(theNewWindow);
  if (label_ptr->Y_Auto_Rescale)
    Y_AutoRescale(theNewWindow);
  if (label_ptr->X_Auto_Rescale)
    X_AutoRescale(theNewWindow);

  /*********************************/
  /* Call Tcl to create the window */

  if (theRunWithXFlag == FALSE) 
    return;

  sprintf(TclCommand, "SetupWindow \"%s\" %s %d %d %c %.3g %.3g %d %.3g %.3g %d %.3g %.3g %d %s %.3g %.3g %d %d %d %d %d %d %s \"%s\" \"%s\"\n",
	  Win_Title, State, ulx, uly, type, label_ptr->X_Min,
	  label_ptr->X_Max, Is_X_AutoRescale(theNewWindow), label_ptr->Y_Min,
	  label_ptr->Y_Max, Is_Y_AutoRescale(theNewWindow), label_ptr->Z_Min,
	  label_ptr->Z_Max, Is_Z_AutoRescale(theNewWindow), 
	  theNewWindow->eps_file_name, init_theta, init_phi, 
	  Is_Shading_On(theNewWindow), Is_ColorCode_On(theNewWindow),
	  Is_Grid_On(theNewWindow), Is_X_Log(theNewWindow), 
	  Is_Y_Log(theNewWindow), Is_Z_Log(theNewWindow), theDumpFile, 
	  label_ptr->X_Label, label_ptr->Y_Label);
  if(Tcl_Eval(interp,TclCommand) != TCL_OK) {
    printf("Error creating window\n");
    if(interp->result != NULL) {
      printf("%s\n",interp->result);
    }
  }
}

int C_CreateWindowProc(ClientData cl, Tcl_Interp *interp, int argc, 
		       char **argv)
{
  XGCValues       theGCValues;
  XSetWindowAttributes theWindowAttributes;
  unsigned long   theWindowMask;
  WindowType theNewWindow = theWindowArray[atoi(argv[1])];

  /* printf("entering C_CreateWindowProc\n"); */
  
  theNewWindow->tkwin = Tk_NameToWindow(interp,argv[2],mainWindow);
  Tk_CreateEventHandler(theNewWindow->tkwin,ExposureMask,EventProc,theNewWindow);

  /****************************************************/
  /* Setting the windiw's attributes and creating it  */

  theWindowAttributes.border_pixel = theWhitePixel;
  theWindowAttributes.background_pixel = GrayShade[3];
  theWindowMask = (CWBackPixel | CWBorderPixel);

  /*
   * 7.16.93 Change theWindowMask and theWindowAttributes to use backingStore
   * speed considerations. Sure, it wastes a bit of memory, but performance
   * should be increased significantly
   */
  if (XDoesBackingStore(XScreenOfDisplay(theDisplay, theScreen)) != NotUseful) {
    theWindowAttributes.backing_store = WhenMapped;
    theWindowMask |= CWBackingStore;
  };

  Tk_ChangeWindowAttributes(theNewWindow->tkwin,theWindowMask,&theWindowAttributes);

  /***************************************/
  /* Initializing a GC for the window    */

  theNewWindow->xwingc = Tk_GetGC(theNewWindow->tkwin, (unsigned long)0, &theGCValues);
  if (theNewWindow->xwingc) {
    XSetForeground(theDisplay, theNewWindow->xwingc, theWhitePixel);
    XSetBackground(theDisplay, theNewWindow->xwingc, theBlackPixel);
  } else {
    printf("XGrafix: can't create a GC for the %s window.", theNewWindow->plot_title);
  }

  XSetFont(theDisplay, theNewWindow->xwingc, theFont);

  return TCL_OK;
}


void ReallocateWindows()
{
  sizeOfWindowArray += sizeIncrement;
  theWindowArray = (WindowType *)realloc(theWindowArray,
					 sizeof(struct window_struct)*
					 sizeOfWindowArray);
  if(theWindowArray == NULL) {
    printf("Error: Not enough memory to reallocate new window array\n");
    exit(1);
  }
}
  

LabelType 
SetupLabelStruct()
{
  LabelType       label;

  if (!(label = (LabelType) malloc(sizeof(struct label))))
    puts("Xgrafix.SetupLabelStruct: malloc error!");
  
  label->Z_Scale = 1;
  label->Z_Min = 0;
  label->Z_Max = 0;
  label->Z_Label = NULL;
  label->Z_Auto_Rescale = True;

  label->Y_Scale = 1;
  label->Y_Offset = 0;
  label->Y_Min = 0;
  label->Y_Max = 0;
  label->Y_Label = NULL;
  label->Y_Auto_Rescale = True;

  label->X_Scale = 1;
  label->X_Offset = 0;
  label->X_Min = 0;
  label->X_Max = 0;
  label->X_Label = NULL;
  label->X_Auto_Rescale = True;

  return (label);
}

DataType 
SetupDataStruct()
{
  DataType        data;

  if (!(data = (DataType) malloc(sizeof(struct data))))
    puts("Xgrafix.SetupDataStruct: malloc error!");

  data->z = NULL;
  data->zscat = NULL;
  data->y = NULL;
  data->x = NULL;
  data->mpoints = 0;
  data->npoints = 0;
  data->color = 0;
  data->state = 0;
  data->next = NULL;
  data->xCoord = 0;
  data->yCoord = 0;
  data->wArraySize = 1;
  data->xArraySize = 1;
  data->yArraySize = 1;
  data->zArraySize = 1;
  data->wArrayOffset = 0;
  data->xArrayOffset = 0;
  data->yArrayOffset = 0;
  data->zArrayOffset = 0;
  return (data);
}
