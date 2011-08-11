#include "xgrafixint.h"
#include <stdlib.h>
#include <signal.h>

#ifdef XPMENABLED
#include <X11/xpm.h>
#endif

static double xtemp1, ytemp1;

int C_CreateWindowProc(ClientData, Tcl_Interp *, int, char **);
int C_KillGraphicsProc();
void StartGraphics();
void Signal_KillGraphicsProc(int i);

void Signal_RestoreGraphicsProc(int i) {
  signal(SIGUSR1,SIG_IGN);
  theRunWithXFlag = TRUE;
}

void Signal_KillGraphicsProc(int i) {
  signal(SIGUSR1,SIG_IGN);
#ifdef Tcl75_Tk41
  Tcl_DoWhenIdle((Tcl_IdleProc*) C_KillGraphicsProc,(ClientData)NULL);
#endif
#ifdef Tcl74_Tk40
  Tk_DoWhenIdle((Tk_IdleProc*) C_KillGraphicsProc,(ClientData)NULL);
#endif
}

void XG_Quit()
{
  Quit();
  if (theCodeName) free(theCodeName);
  if (theInputFile) free(theInputFile);
  if (theDumpFile) free(theDumpFile);
  if (theEPSFile) free(theEPSFile);
  if (theRunWithXFlag == TRUE) {
    Tcl_DeleteInterp(interp);
    XFreeFont(theDisplay,theFontStruct);
    XCloseDisplay(theDisplay);
  }
  exit(0);
}

void StartGraphics() {
  WindowType theWindow;
  int i;
  FILE *f;
  char str[80];
  char xgdisfile[512];

  sprintf(xgdisfile,"%s/.xgdisplay",getenv("HOME"));
  f = fopen(xgdisfile, "r");
  if (f == NULL) {
    printf("File %s could not be read.\n",xgdisfile);
    printf("Continuing without X.\n");
    theRunWithXFlag = FALSE;
    return;
  }

  fgets(str, 79, f);
  strcpy(ReqDisp, str);
  i = strlen(ReqDisp) - 1;
  if (ReqDisp[i] == '\n') {
    ReqDisp[i] = '\0';
  }

  fclose(f);

  if ((theDisplay = XOpenDisplay(ReqDisp)) == NULL) {
    printf("Couldn't open display: %s\n", ReqDisp);
    printf("Continuing without X.\n");
    theRunWithXFlag = FALSE;
    return;
  }

  XCloseDisplay(theDisplay);

  /* Reinitialize the X and Tcl/Tk stuff. */
  XGInitX();
  XGInitTclTk();

  /* Reinitialize the special dialog box. */
  for (i=0; i<numberOfSpecials; i++) {
    sprintf(TclCommand, "SetUpNewVar %d \"%s\" \"%s\"\n", i, 
	    theSpecialArray[i]->Varname,
	    ( theSpecialArray[i]->Type == INTEGER ) ? "integer" :
	    (( theSpecialArray[i]->Type == FLOAT ) ? "float" :
	     (( theSpecialArray[i]->Type == DOUBLE ) ? "double" : "char" )));
    Tcl_Eval(interp, TclCommand);
  }

  /* Recreate all the diagnostic windows. */
  for (i=0; i<numberOfWindows; i++) {
    theWindow = theWindowArray[i];
    sprintf(TclCommand, "SetupWindow \"%s\" %s %d %d %c %.3g %.3g %d %.3g %.3g %d %.3g %.3g %d %s %.3g %.3g %d %d %d %d %d %d %s \"%s\" \"%s\"\n",
	    theWindow->plot_title, "closed", theWindow->ulxc, theWindow->ulyc,
	    theWindow->type, theWindow->label->X_Min, theWindow->label->X_Max,
	    Is_X_AutoRescale(theWindow), theWindow->label->Y_Min, 
	    theWindow->label->Y_Max, Is_Y_AutoRescale(theWindow), 
	    theWindow->label->Z_Min, theWindow->label->Z_Max, 
	    Is_Z_AutoRescale(theWindow), theWindow->eps_file_name,
	    theWindow->theta, theWindow->phi,Is_Shading_On(theWindow), 
	    Is_ColorCode_On(theWindow), Is_Grid_On(theWindow), 
	    Is_X_Log(theWindow), Is_Y_Log(theWindow), Is_Z_Log(theWindow),
	    theDumpFile, theWindow->label->X_Label, theWindow->label->Y_Label);
    if (Tcl_Eval(interp, TclCommand) != TCL_OK) {
      printf("Error re-creating window.\n");
    }
  }

  theRunWithXFlag = TRUE;
  theRunFlag = FALSE;

}

void DoGraphics(ClientData cl)
{
  /* printf("c doGraph\n"); */
  sprintf(TclCommand,"UpdateTime %g ; update; DoGraphics; update idletasks\n",
	  *theTimeStep);
  Tcl_Eval(interp,TclCommand);
  if(theRunFlag==TRUE) {
#ifdef Tcl75_Tk41
    Tcl_CreateTimerHandler(0, DoMain,(ClientData)NULL);
#endif
#ifdef Tcl74_Tk40
    Tk_CreateTimerHandler(0, DoMain,(ClientData)NULL);
#endif
  }
}

void DoMain(ClientData cl)
{
  char thePDumpFile[80], thePDFRoot[80];

  strncpy(thePDFRoot,theDumpFile,findlen(theDumpFile));
  thePDFRoot[findlen(theDumpFile)] = '\0';
  sprintf(TclCommand,"update \n");
  Tcl_Eval(interp,TclCommand);
  /* printf("c domain\n"); */
  if (theNumberOfSteps==0 || theCurrentStep<=theNumberOfSteps) {
    int il;
    for(il=0;il<iterationsPerXUpdate;il++) {
      XGMainLoop();
      if (theDumpPeriod!=0 && theCurrentStep%theDumpPeriod==0) {
	sprintf(thePDumpFile,"%s%d%s",thePDFRoot,
		theCurrentStep/theDumpPeriod,theDumpExtension);
	if (theIDumpFlag) Dump(thePDumpFile);
	else Dump(theDumpFile);
      }
      theCurrentStep++;
    }
#ifdef Tcl75_Tk41
    Tcl_CreateTimerHandler(0, DoGraphics,(ClientData)NULL);
#endif
#ifdef Tcl74_Tk40
    Tk_CreateTimerHandler(0, DoGraphics,(ClientData)NULL);
#endif
  } else {
    if(theExitFlag) {
      if ((theDumpPeriod!=0) && (theIDumpFlag)) {
	sprintf(thePDumpFile,"%s%d%s",thePDFRoot,
		theCurrentStep/theDumpPeriod+1,theDumpExtension);
	Dump(thePDumpFile);
      } 
      else Dump(theDumpFile);
      XG_Quit();
    } else {
      theNumberOfSteps = 0;
      Tcl_Eval(interp,".lbframe.run invoke\n");
      Tcl_Eval(interp,".lbframe.save invoke\n");
    }
  }
}

void XGStart()
{
  char thePDumpFile[80], thePDFRoot[80];

  strncpy(thePDFRoot,theDumpFile,findlen(theDumpFile));
  thePDFRoot[findlen(theDumpFile)] = '\0';
  while (!theExitFlag) {
    if (theRunWithXFlag == TRUE) {
      SortWindows();
      if(numberOfSpecials!=0)
	if(Tcl_Eval(interp,"CreateSpecialDialog\n")!=TCL_OK) {
	  printf("Error creating Special Dialog\n");
	  printf("%s\n",interp->result);
	}
      sprintf(TclCommand,"UpdateTime %g ; DoGraphics; update idletasks\n",*theTimeStep);
      /* Tcl_Eval(interp,TclCommand);  put below */
      if(Tcl_Eval(interp,"Tk_XGStart\n")!=TCL_OK) {
	printf("Error calling Tk_XGStart\n");
      }
      Tcl_Eval(interp,TclCommand); 

      if (theNumberOfSteps != 0) {
	Tcl_Eval(interp,".lbframe.run invoke\n");
      }
      
      /* Change the procedure for the HUP signal. */
      signal(SIGUSR1, Signal_KillGraphicsProc);

      Tk_MainLoop();
    }
    
    if (theRunWithXFlag == FALSE) {
      signal(SIGUSR1, Signal_RestoreGraphicsProc);
      
      while ((theNumberOfSteps==0 || theCurrentStep<=theNumberOfSteps) && 
	     (theRunWithXFlag == FALSE)) {
	XGMainLoop();
	if (theDumpPeriod!=0 && theCurrentStep%theDumpPeriod==0) {
	  sprintf(thePDumpFile,"%s%d%s",thePDFRoot,
		  theCurrentStep/theDumpPeriod,theDumpExtension);
	  if (theIDumpFlag) Dump(thePDumpFile);
	  else Dump(theDumpFile);
	}
	theCurrentStep++;
      }
      if ((theDumpPeriod!=0) && (theCurrentStep%theDumpPeriod==0)) {
	sprintf(thePDumpFile,"%s%d%s",thePDFRoot,
		theCurrentStep/theDumpPeriod,theDumpExtension);
	if (theIDumpFlag) Dump(thePDumpFile);
	else Dump(theDumpFile);
      }
      if(theCurrentStep>theNumberOfSteps && (theRunWithXFlag == FALSE)) {
	XG_Quit();
      }
      if (theRunWithXFlag == TRUE) StartGraphics();
    }
  }
  XG_Quit();
}

int C_OpenProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow;
  int index = atoi(argv[1]);

  theWindow = theWindowArray[index];

  if (theWindow->openFlag) {
    *theWindow->openFlag = TRUE;
  }

  return TCL_OK;
}

int C_CloseProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow;
  int i, m;
  int index = atoi(argv[1]);

  theWindow = theWindowArray[index];

  if(theWindow->pixmap_buffer) {
    XFreePixmap(theDisplay, theWindow->pixmap_buffer);
    theWindow->pixmap_buffer = 0;
  }

  if(theWindow->theMesh) {
    m = *(theWindow->data->mpoints);
    for (i = 0; i < m; i++)
      free(theWindow->theMesh[i]);
    free(theWindow->theMesh);    
    theWindow->theMesh = 0;
  }

  if (theWindow->openFlag) {
    *theWindow->openFlag = FALSE;
  }

  return TCL_OK;
}

int C_RunProc()
{
  theRunFlag=TRUE;
#ifdef Tcl75_Tk41
    Tcl_DoWhenIdle(DoMain,(ClientData)NULL);
#endif
#ifdef Tcl74_Tk40
    Tk_DoWhenIdle(DoMain,(ClientData)NULL);
#endif
  return TCL_OK;
}

int C_StopProc()
{
  theRunFlag=FALSE;
  return TCL_OK;
}

int C_KillGraphicsProc()
{
  int i;

  theRunWithXFlag = FALSE;

  for (i=0; i<numberOfWindows; i++) {
    XFreePixmap(theDisplay, theWindowArray[i]->pixmap_buffer);
    theWindowArray[i]->pixmap_buffer = 0;
  }

  sprintf(TclCommand, "set WaitVariable 2 ; destroy .\n");
  Tcl_Eval(interp, TclCommand);
  return TCL_OK;
}

int C_QuitProc() {
  theExitFlag = TRUE;
  return TCL_OK;
}

int C_StepProc()
{
  if(theRunFlag==TRUE) {
    Tcl_Eval(interp,".lbframe.run invoke\n");
  }
#ifdef Tcl75_Tk41
    Tcl_DoWhenIdle(DoMain,(ClientData)NULL);
#endif
#ifdef Tcl74_Tk40
    Tk_DoWhenIdle(DoMain,(ClientData)NULL);
#endif
  return TCL_OK;
}

int C_SaveProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  Dump(argv[1]);
  return TCL_OK;
}
int C_printfProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  printf("%s\n", argv[1]);
  return TCL_OK;
}
int C_pauseProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  int i;
  float f;
  for (i=0; i<=10000; i++) f = cos(sqrt(3.14*i));
  printf("wait done - %s\n", argv[1]);
  return TCL_OK;
}

int C_RefreshGraphicsProc(ClientData cl, Tcl_Interp *interp, int argc,
			  char **argv)
{
  WindowType Window = theWindowArray[atoi(argv[1])];

  RedrawWindow(Window);

  return TCL_OK;
}

int C_RescaleProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  LabelType label;
  WindowType Window = theWindowArray[atoi(argv[1])];

  label = Window->label;

  label->X_Min = atof(argv[2]);
  label->X_Max = atof(argv[3]);
  label->X_Auto_Rescale = atoi(argv[4]);
  if(label->X_Auto_Rescale)
    X_AutoRescale(Window);
  else
    X_AutoRescale_Off(Window);
  if(atoi(argv[5]))
    Set_X_Log(Window);
  else
    Set_X_Lin(Window);

  label->Y_Min = atof(argv[6]);
  label->Y_Max = atof(argv[7]);
  label->Y_Auto_Rescale = atoi(argv[8]);
  if(label->Y_Auto_Rescale)
    Y_AutoRescale(Window);
  else
    Y_AutoRescale_Off(Window);
  if(atoi(argv[9]))
    Set_Y_Log(Window);
  else
    Set_Y_Lin(Window);

  label->Z_Min = atof(argv[10]);
  label->Z_Max = atof(argv[11]);
  label->Z_Auto_Rescale = atoi(argv[12]);
  if(label->Z_Auto_Rescale) {
    Set_ThreeD(theNewWindow);
    ColorCode_On(theNewWindow);
    Grid_On(theNewWindow);
    Z_AutoRescale(Window);
  }
  else {
    Set_ThreeD(theNewWindow);
    ColorCode_On(theNewWindow);
    Grid_On(theNewWindow);
    Z_AutoRescale_Off(Window);
  }
  if(atoi(argv[13]))
    Set_Z_Log(Window);
  else
    Set_Z_Lin(Window);

  Iconic_Before(Window);

  RedrawWindow(Window);
  return TCL_OK;
}

int C_TraceProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  if (Is_Trace_On(theWindow)) {
    Trace_Off(theWindow);
  }
  else {
    Trace_On(theWindow);
  }

  return TCL_OK;
}

int C_PrintProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  char fileType, printMode;
  WindowType theWindow = theWindowArray[atoi(argv[1])];
  char **xpmData;

  fileType = argv[4][0];
  printMode = argv[5][0];

  switch(fileType) {
  case 'x':

#ifdef XPMENABLED
    if(argv[6][0]!='n' && (!Is_Trace_On(theWindow) || 
			   Is_X_AutoRescale(theWindow) || 
			   Is_Y_AutoRescale(theWindow))) {
      Iconic_Before(theWindow);
      C_RefreshGraphicsProc(NULL,NULL,1,argv);
    }
    XpmWriteFileFromPixmap(theDisplay,argv[2],theWindow->pixmap_buffer,
			   0,NULL);
#endif

    break;

  case 'p':
#ifdef XPMENABLED
    if(argv[4][1]=='n') /* png */ {
      if(argv[6][0]!='n' && (!Is_Trace_On(theWindow) || 
                             Is_X_AutoRescale(theWindow) || 
                             Is_Y_AutoRescale(theWindow))) {
        Iconic_Before(theWindow);
        C_RefreshGraphicsProc(NULL,NULL,1,argv);
      }
      XpmWriteFileFromPixmap(theDisplay,argv[2],theWindow->pixmap_buffer,0,0);
      {
        char name[512];
        char name2[512];
        char buffer[512];
        int i;
        sprintf(name,"%s",argv[2]);
        i = strlen(name);
        if(i<5) return 0; /* invalid name */
        sprintf(name+i-4,"%s",".xpm");
        sprintf(name2,"%s",name);
        sprintf(name2+i-4,"%s",".png");
      
        sprintf(buffer,"mv %s %s;convert %s %s; rm %s",name2,name,name,name2,name);
        system(buffer);
        break;
      }
    }

#else
    break;
#endif /* XPMENABLED */

  case 'e':
    if(access(argv[2],F_OK) || printMode=='o' || printMode=='c')
      PSHeader(argv[2]);
    (*(theWindow->print_function)) (theWindow, argv[2], argv[3]);
    break;
  case 'a':
    (*(theWindow->ascii_print_function)) (theWindow, argv[2], printMode);
    break;
  case 'b':
    (*(theWindow->xgrafix_print_function)) (theWindow, argv[2], printMode);
    break;
  case 'g':

#ifdef XPMENABLED
    if(argv[6][0]!='n' && (!Is_Trace_On(theWindow) || 
			   Is_X_AutoRescale(theWindow) || 
			   Is_Y_AutoRescale(theWindow))) {
      Iconic_Before(theWindow);
      C_RefreshGraphicsProc(NULL,NULL,1,argv);
    }
    XpmCreateDataFromPixmap(theDisplay, &xpmData, theWindow->pixmap_buffer,
			    0,NULL);
    XpmToGif(xpmData,argv[2]);
    XpmFree(xpmData);
#endif

    break;
  }
  return TCL_OK;
}
  
int C_3DViewProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType Window = theWindowArray[atoi(argv[1])];
  Window->theta = atof(argv[2]);
  Window->phi = atof(argv[3]);
  if(atoi(argv[4]))
    Shading_On(Window);
  else
    Shading_Off(Window);

  if(atoi(argv[5]))
    ColorCode_On(Window);
  else
    ColorCode_Off(Window);

  if(atoi(argv[6]))
    Grid_On(Window);
  else
    Grid_Off(Window);

  RedrawWindow(Window);
  return TCL_OK;
}

int C_GetSpecialValuesProc(ClientData cl, Tcl_Interp *interp, int argc,
			   char **argv)
{
  int i;
  strcpy(TclCommand,"");
  for(i=0;i<numberOfSpecials;i++) {
    if (theSpecialArray[i]->Type == DOUBLE) {
      sprintf(TclCommand,"%s %g",TclCommand,*(double*)(theSpecialArray[i]->data));
    } else if (theSpecialArray[i]->Type == INTEGER) {
      sprintf(TclCommand,"%s %d",TclCommand,*(int*)(theSpecialArray[i]->data));
    } else if (theSpecialArray[i]->Type == FLOAT) {
      sprintf(TclCommand,"%s %f",TclCommand,*(float*)(theSpecialArray[i]->data));
    } else {
      sprintf(TclCommand,"%s %c",TclCommand,*(char*)(theSpecialArray[i]->data));
    }      
  }
  sprintf(TclCommand,"%s\n",TclCommand);
  interp->result = TclCommand;
  return TCL_OK;
}

int C_SetNewSpecialProc(ClientData cl, Tcl_Interp *interp, int argc,
			char **argv)
{
  int index = atoi(argv[1]);

  if (theSpecialArray[index]->Type == DOUBLE) {
    *(double*)(theSpecialArray[index]->data) = atof(argv[2]);
  } else if (theSpecialArray[index]->Type == INTEGER) {
    *(int*)(theSpecialArray[index]->data) = atoi(argv[2]);
  } else if (theSpecialArray[index]->Type == FLOAT) {
    *(float*)(theSpecialArray[index]->data) = atof(argv[2]);
  } else {
    *(char*)(theSpecialArray[index]->data) = argv[2][0];
  }

  Tcl_Eval(interp,"DoGraphics\n");
  return TCL_OK;
}

int C_GetRangesProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow = theWindowArray[atoi(argv[1])];
  LabelType theLabel = theWindow->label;

  sprintf(TclCommand,"%g %g %d %d %g %g %d %d %g %g %d %d\n",theLabel->X_Min,
	  theLabel->X_Max, theLabel->X_Auto_Rescale, Is_X_Log(theWindow), 
	  theLabel->Y_Min, theLabel->Y_Max, theLabel->Y_Auto_Rescale, 
	  Is_Y_Log(theWindow), theLabel->Z_Min, theLabel->Z_Max, 
	  theLabel->Z_Auto_Rescale, Is_Z_Log(theWindow));
  interp->result = TclCommand;
  return TCL_OK;
}

int C_3dCrossProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow = theWindowArray[atoi(argv[1])];
  DataType theData = theWindow->data;

  if (!(Is_Irr_Surf(theData)) && !(Is_Scatter(theData))) {
    DrawCrosshair(theWindow, atoi(argv[2]), atoi(argv[3]));
  }
  return TCL_OK;
}

int C_3dCrossOnProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  Crosshair_On(theWindow);
  return TCL_OK;
}

int C_3dCrossOffProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  Crosshair_Off(theWindow);
  return TCL_OK;
}

int C_ShouldRefreshProc(ClientData cl, Tcl_Interp *interp, int argc,
			char **argv)
{
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  if (Is_Trace_On(theWindow) && !Is_X_AutoRescale(theWindow) &&
      !Is_Y_AutoRescale(theWindow))
    TclCommand[0] = '1';
  else
    TclCommand[0] = '0';

  TclCommand[1] = '\0';
  interp->result = TclCommand;
  return TCL_OK;
}

int C_DefineULProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  double x1, y1;
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  x1 = atof(argv[2]);
  y1 = atof(argv[3]);

  if (Is_X_Log(theWindow)) {
    xtemp1 = theWindow->c1 * log10(max(x1, DBL_MIN)) + theWindow->d1;
  } else {
    xtemp1 = theWindow->c1 * x1 + theWindow->d1;
  }

  if (Is_Y_Log(theWindow)) {
    ytemp1 = theWindow->c2 * log10(max(y1, DBL_MIN)) + theWindow->d2;
  } else {
    ytemp1 = theWindow->c2 * y1 + theWindow->d2;
  }

  return TCL_OK;
}

int C_StretchBoxProc(ClientData cl, Tcl_Interp *interp, int argc, char **argv)
{
  double x2,y2,xtemp2,ytemp2;
  Pixmap pm;
  XRectangle theClientArea;
  Tk_Window tkwin;
  WindowType theWindow = theWindowArray[atoi(argv[1])];

  tkwin = theWindow->tkwin;

  theClientArea.x = 0;
  theClientArea.y = 0;
  theClientArea.width = Tk_Width(tkwin);
  theClientArea.height = Tk_Height(tkwin);
  XSetClipRectangles(theDisplay, theWindow->xwingc, 0, 0, &theClientArea, 
		     1, Unsorted);

  pm = XCreatePixmap(theDisplay, Tk_WindowId(tkwin), Tk_Width(tkwin),
		     Tk_Height(tkwin), Tk_Depth(tkwin));
  XCopyArea(theDisplay, theWindow->pixmap_buffer, pm, theWindow->xwingc, 0,
	    0, Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);

  x2 = atof(argv[2]);
  y2 = atof(argv[3]);

  if (Is_X_Log(theWindow)) {
    xtemp2 = theWindow->c1 * log10(max(x2, DBL_MIN)) + theWindow->d1;
  } else {
    xtemp2 = theWindow->c1 * x2 + theWindow->d1;
  }
    
  if (Is_Y_Log(theWindow)) {
    ytemp2 = theWindow->c2 * log10(max(y2, DBL_MIN)) + theWindow->d2;
  } else {
    ytemp2 = theWindow->c2 * y2 + theWindow->d2;
  }

  XSetForeground(theDisplay, theWindow->xwingc, theWhitePixel);
  XDrawRectangle(theDisplay, pm, theWindow->xwingc, min(xtemp1,xtemp2), 
		 min(ytemp1,ytemp2), fabs(xtemp2 - xtemp1), 
		 fabs(ytemp2 - ytemp1));
  XCopyArea(theDisplay, pm, Tk_WindowId(tkwin), theWindow->xwingc, 0, 0,
	    Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);
  XFlush(theDisplay);
  XFreePixmap(theDisplay, pm);
  
  return TCL_OK;
}


void InitTclCommands()
{
  Tcl_CreateCommand(interp, "C_Run", (Tcl_CmdProc *)C_RunProc,
		    (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Quit", (Tcl_CmdProc *)C_QuitProc,
		    (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_KillGraphics",
		    (Tcl_CmdProc *) C_KillGraphicsProc, 
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Close",
		    (Tcl_CmdProc *)C_CloseProc, (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Open", (Tcl_CmdProc *)C_OpenProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Stop", (Tcl_CmdProc *)C_StopProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Step", (Tcl_CmdProc *)C_StepProc,
		    (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Save", (Tcl_CmdProc *)C_SaveProc,
		    (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_printf", (Tcl_CmdProc *)C_printfProc,
		    (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_pause", (Tcl_CmdProc *)C_pauseProc,
		    (ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_RefreshGraphics",
		    (Tcl_CmdProc *)C_RefreshGraphicsProc, 
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Rescale", (Tcl_CmdProc *)C_RescaleProc, 
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Trace", (Tcl_CmdProc *)C_TraceProc, 
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_Print", (Tcl_CmdProc *)C_PrintProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_UpdateCrosshair",
		    (Tcl_CmdProc *)C_UpdateCrosshairProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_SetCrosshairParameters", 
		    (Tcl_CmdProc *)C_SetCrosshairParametersProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_3DView", (Tcl_CmdProc *)C_3DViewProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_GetSpecialValues",
		    (Tcl_CmdProc *)C_GetSpecialValuesProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_SetNewSpecials",
		    (Tcl_CmdProc *)C_SetNewSpecialProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_GetRanges", (Tcl_CmdProc *)C_GetRangesProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_CreateWindow", (Tcl_CmdProc *)C_CreateWindowProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_3dCross", (Tcl_CmdProc *)C_3dCrossProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_3dCrossOn", (Tcl_CmdProc *)C_3dCrossOnProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_3dCrossOff", (Tcl_CmdProc *)C_3dCrossOffProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_ShouldRefresh",
		    (Tcl_CmdProc *)C_ShouldRefreshProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_DefineUL", (Tcl_CmdProc *)C_DefineULProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "C_StretchBox", (Tcl_CmdProc *)C_StretchBoxProc,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
}

void SortWindows()
{
  int i;
  int j;
  WindowType *newWindowArray = (WindowType *)
    malloc(sizeof(struct window_struct)*sizeOfWindowArray);

  for (i=0;i<numberOfWindows;i++) {
    j = Search(theWindowArray[i]->plot_title);
    newWindowArray[j] = theWindowArray[i];
  }

  free(theWindowArray);
  theWindowArray=newWindowArray;
}

int Search(const char *WinTitle)
{
  sprintf(TclCommand,"set temp \"\"; set \"WinData([append temp . [string tolower \"%s\"]],index)\"\n",WinTitle);
  Tcl_Eval(interp,TclCommand);
  return atoi(interp->result);
}
