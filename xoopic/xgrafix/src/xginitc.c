#include "xgrafixint.h"
#include <stdlib.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

void PrintUsage(char *string)
{
  printf("Usage: %s [options]\n",string);
  printf("\t-d dumpfile[.dmp]:\n\t\tSpecify dump file name. Default is output.dmp\n");
  printf("\t-display displayname:\n\t\tSpecify the display. Default is DISPLAY environment variable.\n");
  printf("\t-dp n:\n\t\tSpecify the dump period. Default is 0 (no periodic dumps)\n");
  printf("\t-exit:\n\t\tExit after done with iterations\n");
  printf("\t-h:\n\t\tThis help message\n");
  printf("\t-i inputfile[.inp]:\n\t\tSpecify input file name. Default is no input file\n");
  printf("\t-id:\n\t\tIncremental periodic dump. Default is overwrite.\n");
  printf("\t-nox:\n\t\tRun without X\n");
  printf("\t-p epsfile[.eps]:\n\t\tSpecify ps file name. Default is output.eps\n");
  printf("\t-s n:\n\t\tSpecify the number of iterations. Default is 0 (run forever)\n");
  printf("\t-u n:\n\t\tSpecify the number of iterations per X update. Default is 1\n");
}

void PrintUnknownOption(char *string, char *string2)
{
  printf("Unknown option specified: %s\n",string);
  PrintUsage(string2);
}

void PrintNoArg(char *string, char *string2)
{
  printf("No argument given to option: %s\n",string);
  PrintUsage(string2);
}

void XGInit(int argc, char **argv, double *t)
{
  int index, endianInt = 1;
  char *endianChar;
  
  /*******************/
  /* The C stuff */
  
  theTimeStep = t;
  theRunFlag = FALSE;
  theStepFlag = FALSE;
  theExitFlag = FALSE;
  init_theta = init_phi = 45;
  theRunWithXFlag = TRUE; 
  theNumberOfSteps = 0;
  iterationsPerXUpdate = 1;
  theCurrentStep = 1;
  theDumpPeriod = 0;
  theIDumpFlag = FALSE;
  WasDumpFileGiven = FALSE;
  WasInputFileGiven = FALSE;
  theExitFlag = FALSE;
  ReqDisp[0] = 0;

  /***********************************/
  /* Checking the endianness of host */

  endianChar = (char*)&endianInt;

  if (*endianChar == (char)1) {
     theEndian = LITTLE;
  } else {
     theEndian = BIG;
  }

  /*****************************************/
  /* Getting the code and input file names */

  theCodeName = theInputFile = theDumpFile = theEPSFile = NULL;

/* mindgame: to prevent segment fault */
  theCodeName = (char *)malloc((strlen(argv[0])+1)*sizeof(char));
  if (theCodeName) sprintf(theCodeName, "%s", argv[0]);

/* Try to read the input parameters, i.e. -i -d -dp, etc. options */
   for(index=1;index<argc;index++) {
    if(argv[index][0] != '-') {
      PrintUnknownOption(argv[index],argv[0]);
      exit(1);
    }
    switch(argv[index][1]) {
    case 'd':
      if(argv[index][2]!='\0' && argv[index][2]!='p' && 
	 strcmp(argv[index],"-display")) {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      switch(argv[index][2]) {
      case '\0':
	index++;
	/* mindgame: to prevent segment fault */
	theDumpFile = (char *)malloc((strlen(argv[index])+strlen(theDumpExtension)+1)*sizeof(char));
	if (theDumpFile) sprintf(theDumpFile, "%s",argv[index]);
	WasDumpFileGiven = TRUE;
    if(access(theDumpFile,F_OK)) {
        // minrk: specified dump file doesn't exist, trying in order:
        // 1. MPI load on <thefile>.n
        // 2. regular load on <thefile>.dmp
        // 3. MPI load on <thefile>.dmp.n
        // 4. give up
        char *new_name;
        new_name = (char *)malloc((strlen(argv[index])+strlen(theDumpExtension)+3)*sizeof(char));
        sprintf(new_name,"%s.0",theDumpFile);
        if (!access(new_name, F_OK)){
            // minrk: not MPI load on <thefile>.n
            sprintf(new_name,"%s%s",theDumpFile, theDumpExtension);
            if(access(new_name,F_OK)) { 
                // minrk: not regular load on <thefile>.dmp
                sprintf(new_name,"%s%s.0",theDumpFile, theDumpExtension);
                if(access(new_name,F_OK)) {
                    // minrk: not MPI load on <thefile>.dmp.n
                    fprintf(stderr, "Warning: %s does not exist\n",theDumpFile); 
                    // minrk: let dump.cpp handle this, comment out old override:
                    // WasDumpFileGiven = FALSE;
                    // free(theDumpFile); theDumpFile = NULL;
                }else {
                    strcat(theDumpFile, theDumpExtension);
                }
            } else {
                strcat(theDumpFile, theDumpExtension);
            }
        }
        free(new_name);
    }

	if (WasDumpFileGiven) printf("Dump file is '%s'\n",theDumpFile);
	break;
      case 'p':
	theDumpPeriod = atoi(argv[++index]);
	printf("Dumping every %d steps\n",theDumpPeriod);
	break;
      case 'i':
	strcpy(ReqDisp, argv[++index]);
	printf("Setting display to %s.\n",ReqDisp);
	break;
      }
      break;
    case 'e':
      if (!strcmp(argv[index],"-exit")) {
	theExitFlag = TRUE;
      } else {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      break;
    case 'h':
      PrintUsage(argv[0]);
      exit(0);
      break;
    case 'i':
      if ((argv[index][2]!='\0') && (argv[index][2]!='d')) {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if (argv[index][2]=='d') {
	theIDumpFlag = TRUE;
      } else {
	if(index+1 >= argc) {
	  PrintNoArg(argv[index],argv[0]);
	  exit(1);
	}
	index++;
  /* mindgame: to enable long inputfile name (including directory path) */
	theInputFile = (char *)malloc((strlen(argv[index])+strlen(theInputExtension)+1)*sizeof(char));
	if (theInputFile) sprintf(theInputFile, "%s", argv[index]);

	if(access(theInputFile,F_OK)) {
	  char *new_name;
	  new_name = (char *)malloc((strlen(argv[index])+strlen(theInputExtension)+1)*sizeof(char));
	  sprintf(new_name,"%s%s",theInputFile, theInputExtension);
	  if(access(new_name,F_OK)) {
	    fprintf(stderr, "Error: %s does not exist\n",theInputFile);
	    exit(1);
	  }
	  else {
	    strcat(theInputFile, theInputExtension);
	  }
	  free(new_name);
	}

	WasInputFileGiven = TRUE;
	printf("\nInput file is '%s'\n",theInputFile);
      }
      break;
    case 'n':
      if(!strcmp(argv[index],"-nox")) {
	printf("Running without X\n");
	theRunWithXFlag = FALSE;
      } else {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      break;
    case 'p':
      if(argv[index][2]!='\0') {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      index++;
      theEPSFile = (char *)malloc((strlen(argv[index])+1)*sizeof(char));
      if (theEPSFile) sprintf(theEPSFile, "%s", argv[index]);
      printf("Postscript file is '%s'\n",theEPSFile);
      break;
    case 's':
      if(argv[index][2]!='\0') {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      theNumberOfSteps = atoi(argv[++index]);
      printf("Running for %d steps\n",theNumberOfSteps);
      break;

	 case 'u':  /*  set the number of iterations per X update */
      if(argv[index][2]!='\0') {
		  PrintUnknownOption(argv[index],argv[0]);
		  exit(1);
      }
      if(index+1 >= argc) {
		  PrintNoArg(argv[index],argv[0]);
		  exit(1);
      }
      iterationsPerXUpdate = atoi(argv[++index]);
		if(iterationsPerXUpdate < 1) iterationsPerXUpdate = 1;
      printf("Iterations per X update: %d steps\n",iterationsPerXUpdate);
      break;

    default:
      PrintUnknownOption(argv[index],argv[0]);
      exit(1);
      break;
    }      
  }      
/* End of trying to read parameters */

  if (theDumpPeriod != 0) {
    if (theIDumpFlag)
      printf("Periodic dumping in incremental mode.\n");
    else printf("Periodic dumping in overwrite mode.\n");
  }

  if (!WasInputFileGiven) {
    if (!theEPSFile) theEPSFile = (char *)malloc((strlen(theDefaultEPSFile)+1)*sizeof(char));
    if (theEPSFile) sprintf(theEPSFile,"%s", theDefaultEPSFile);
    if (!theDumpFile) theDumpFile = (char *)malloc((strlen(theDefaultDumpFile)+1)*sizeof(char));
    if (theDumpFile) sprintf(theDumpFile,"%s",theDefaultDumpFile);
  } else if (!WasDumpFileGiven) {
    int len;
    len = findlen(theInputFile);
    if (!theEPSFile) theEPSFile = (char *)malloc((len+strlen(theEPSExtension)+1)*sizeof(char));
    if (theEPSFile) {
	strncpy(theEPSFile,theInputFile,len);
	theEPSFile[len] = '\0';
	strcat(theEPSFile,theEPSExtension);
    }
    if (!theDumpFile) theDumpFile = (char *)malloc((len+strlen(theDumpExtension)+1)*sizeof(char));
    if (theDumpFile) {
	strncpy(theDumpFile,theInputFile,len);
	theDumpFile[len] = '\0';
	strcat(theDumpFile,theDumpExtension);
    }
  } else {
    int len;
    len = findlen(theInputFile);
    if (!theEPSFile) theEPSFile = (char *)malloc((len+strlen(theEPSExtension)+1)*sizeof(char));
    if (theEPSFile) {
	strncpy(theEPSFile,theInputFile,len);
	theEPSFile[len] = '\0';
	strcat(theEPSFile,theEPSExtension);
    }
  }

  /*********************************/
  /* Initializing the Window array */

  sizeOfWindowArray = 10;
  theWindowArray = (WindowType *)malloc(sizeof(WindowType)*sizeOfWindowArray);
  numberOfWindows = 0;
  sizeIncrement = 10;

  /**********************************/
  /* Initializing the Special array */

  theSpecialArray = NULL;
  sizeOfSpecialArray = 0;
  specialSizeIncrement = 10;
  numberOfSpecials = 0;

  if(theRunWithXFlag==TRUE) {
    XGInitX();
    XGInitTclTk();
  }
}

/* A procedure called by XGInit to determine the rootlength of filenames */
int findlen(const char *string){
 int i, len;

 len = strlen(string);
 i = len-1;
 while (string[i] != '.' && i  >=0) {
   if(string[i]=='/') return len;
   i = i - 1;
 }
 if(i>0)  return (i);
 return len;

}

void XGInitX() {
  XColor theRGBColor, theHardwareColor;
  int i;
  char temp[60];

  /***********************/
  /* Remaining XLib code */

  if (ReqDisp[0] == 0) {
    strcpy(ReqDisp, XDisplayName(NULL));
  }

  theDisplay = XOpenDisplay(ReqDisp);
  if (theDisplay == NULL) {
    fprintf(stderr, "XGrafix: Cannot establish connection to the XServer %s\n", ReqDisp);
    exit(1);
  }

  theScreen = DefaultScreen(theDisplay);
  theDepth = DefaultDepth(theDisplay, theScreen);
  
  theBlackPixel = BlackPixel(theDisplay, theScreen);
  theWhitePixel = WhitePixel(theDisplay, theScreen);
  
  theColormap = DefaultColormap(theDisplay, theScreen);
  theRootWindow = RootWindow(theDisplay, theScreen);
  
  theDisplayWidth = DisplayWidth(theDisplay, theScreen);
  theDisplayHeight = DisplayHeight(theDisplay, theScreen);

  printf("\nXGrafix version %s\n",xgversion);
  printf("(c) Copyright 1991-2010 The Regents of the University of California\n\n");
  printf("XGrafix is running on %s version %d of the X Window System, X%d R%d\n", ServerVendor(theDisplay), VendorRelease(theDisplay), 
	 ProtocolVersion(theDisplay), ProtocolRevision(theDisplay));
  
  /*
   * 7.20.93 XDisplayName takes a (char *) and not a (Display *) ! The NULL
   * is because XDisplay name returns it's own argument if the NULL string is
   * not specified. With a NULL argument it returns the DISPLAY environment
   * variable.
   */
  printf("Display name .............. %s\n", ReqDisp);
  printf("Display width ............. %d\n", theDisplayWidth);
  printf("Display height ............ %d\n", theDisplayHeight);

  if (theDepth == 1)
    printf("Color plane depth ......... %d (monochrome)\n\n", 
	   theDepth);
  else
    printf("Color plane depth ......... %d\n\n", theDepth);

  /*****************************************/
  
  if (theDepth > 1) {
    for (i = 0; i < MAXCOLORS; i++) {
      if (XLookupColor(theDisplay, theColormap, theColorNames[i], &theRGBColor, &theHardwareColor))
	if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	  Color[i] = theHardwareColor.pixel;
	  RedColor[i] = (SCALAR)theHardwareColor.red/65535.0;
	  GreenColor[i] = (SCALAR)theHardwareColor.green/65535.0;
	  BlueColor[i] = (SCALAR)theHardwareColor.blue/65535.0;
    }
    if (XLookupColor(theDisplay, theColormap, WindowTextColor, &theRGBColor, &theHardwareColor))
      if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	theWinTextColor = theHardwareColor.pixel;
    
    if (XLookupColor(theDisplay, theColormap, BorderColor, &theRGBColor, &theHardwareColor))
      if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	theBorderColor = theHardwareColor.pixel;
    
    if (XLookupColor(theDisplay, theColormap, TickMarkColor, &theRGBColor, &theHardwareColor))
      if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	theTickMarkColor = theHardwareColor.pixel;
    
    if (XLookupColor(theDisplay, theColormap, BackgroundColor, &theRGBColor, &theHardwareColor))
      if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	theBKGDColor = theHardwareColor.pixel;
    
    for (i = 0; i < MAXGRAYSHADES; i++) {
      if (XLookupColor(theDisplay, theColormap, theGrayShades[i], &theRGBColor, &theHardwareColor))
	if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	  GrayShade[i] = theHardwareColor.pixel;
    }
    
    /************* the ThreeD Colors ***************/
    for (i = 0; i <= 60; i++) {
      if (0 <= i && i < 10) {
	sprintf(temp, "#%s%s%s", theTDColorNames[10], theTDColorNames[i], theTDColorNames[0]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[10], theTDPSColorNames[i], theTDPSColorNames[0]);
      } else if (10 <= i && i < 20) {
	sprintf(temp, "#%s%s%s", theTDColorNames[20 - i], theTDColorNames[10], theTDColorNames[0]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[20 - i], theTDPSColorNames[10], theTDPSColorNames[0]);
      } else if (20 <= i && i < 30) {
	sprintf(temp, "#%s%s%s", theTDColorNames[0], theTDColorNames[10], theTDColorNames[i - 20]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[0], theTDPSColorNames[10], theTDPSColorNames[i - 20]);
      } else if (30 <= i && i < 40) {
	sprintf(temp, "#%s%s%s", theTDColorNames[0], theTDColorNames[40 - i], theTDColorNames[10]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[0], theTDPSColorNames[40 - i], theTDPSColorNames[10]);
      } else if (40 <= i && i < 50) {
	sprintf(temp, "#%s%s%s", theTDColorNames[i - 40], theTDColorNames[0], theTDColorNames[10]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[i - 40], theTDPSColorNames[0], theTDPSColorNames[10]);
      } else if (50 <= i && i < 60) {
	sprintf(temp, "#%s%s%s", theTDColorNames[10], theTDColorNames[i - 50], theTDColorNames[10]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[10], theTDPSColorNames[i - 50], theTDPSColorNames[10]);
      } else if (i == 60) {
	sprintf(temp, "#%s%s%s", theTDColorNames[10], theTDColorNames[10], theTDColorNames[10]);
	sprintf(ThreeDPSColor[i], "%s%s%s", theTDPSColorNames[10], theTDPSColorNames[10], theTDPSColorNames[10]);
      }
      /* sprintf(temp, "#%x%x%x", 64+3*i, 64+3*i, 64+3*i); */
      
      if (XParseColor(theDisplay, theColormap, temp, &theHardwareColor))
	if (XAllocColor(theDisplay, theColormap, &theHardwareColor))
	  ThreeDColor[i] = theHardwareColor.pixel;
    }
  } else {
    for (i = 0; i < MAXCOLORS; i++)
      Color[i] = theWhitePixel;
    for (i = 0; i < MAXTHREEDCOLORS; i++)
      ThreeDColor[i] = theWhitePixel;
    theWinTextColor = theWhitePixel;
    theDBTextColor = theWhitePixel;
    theBorderColor = theWhitePixel;
    theTickMarkColor = theWhitePixel;
    theBKGDColor = theBlackPixel;

    for (i = 0; i < MAXGRAYSHADES; i++)
      GrayShade[i] = theWhitePixel;
    GrayShade[1] = theBlackPixel;
    GrayShade[3] = theBlackPixel;
    GrayShade[5] = theBlackPixel;
    GrayShade[6] = theBlackPixel;
    GrayShade[10] = theBlackPixel;
  }

  /************************************/
  /* Initializing the XGrafix font    */

  if ((theFontStruct = XLoadQueryFont(theDisplay, theFontName)) == NULL) {
    printf("can't open the specified font\n");
    exit(-1);
  }
  theFont = theFontStruct->fid;
  theFontAscent = theFontStruct->ascent;
  theFontDescent = theFontStruct->descent;
  theFontHeight = theFontAscent + theFontDescent;
}

void XGInitTclTk() {
  char *str, str2[200];
  /************************************/
  /* Initializing the Tcl/Tk stuff    */

#ifdef Tcl75_Tk41
  interp = Tcl_CreateInterp();
  Tcl_Init(interp);
  sprintf(str2, "-display %s", ReqDisp);
  Tcl_SetVar(interp, "argv", str2, 0);
  Tk_Init(interp);
  mainWindow = Tk_MainWindow(interp);
#endif
#ifdef Tcl74_Tk40
  interp = Tcl_CreateInterp();
  mainWindow = Tk_CreateMainWindow(interp, ReqDisp, "", "");
#endif

  /***************************/
  /* Create the Tcl Commands */

  InitTclCommands();

  /******************************/
  /* Sourcing the main Tcl file */

  /*if XGTCL is set in ENV then use it else use compile time default*/
  if(getenv("XGTCL")) {
    str = (char *)malloc((strlen(getenv("XGTCL"))+20)*sizeof(char));
    sprintf(str, "%s", getenv("XGTCL"));
    sprintf(TclCommand,"set XGrafixPath %s; source %s/xgsetup250.tcl; InitMainMenu %d \"%s\"\n",str, str, WasInputFileGiven,theInputFile);
    free(str);
  }
  else {
    sprintf(TclCommand,"set XGrafixPath " XGTCL "; source " XGTCL "/xgsetup250.tcl; InitMainMenu %d \"%s\"\n",WasInputFileGiven,theInputFile);
  }
  if(Tcl_Eval(interp,TclCommand)!=TCL_OK) {
    printf("Error in initialization\n");
    if(*interp->result != 0) {
      printf("%s\n",interp->result);
    }
    if (theCodeName) free(theCodeName);
    if (theInputFile) free(theInputFile);
    if (theDumpFile) free(theDumpFile);
    if (theEPSFile) free(theEPSFile);
    Tcl_DeleteInterp(interp);
    XFreeFont(theDisplay,theFontStruct);
    XCloseDisplay(theDisplay);
    exit(1);
  }
  
#ifndef XPMENABLED
  Tcl_Eval(interp,".printdialog.top.type.menu disable 1\n");
  Tcl_Eval(interp,".printdialog.top.type.menu disable 2\n");
#endif

}

#ifdef __STDC__
void XGStructure(int numPoints,...) {
#else
void XGStructure(va_alist) va_dcl {
  int numPoints;
#endif
  int i, lineColor, fillColor;
  va_list ap;
  STRUCT_FILL fillFlag;
  StructType *structure = (StructType *)malloc(sizeof(StructType));

#ifdef __STDC__
  va_start(ap,numPoints);
#else
  va_start(ap);
  numPoints = va_arg(ap,int);
#endif
  fillFlag = va_arg(ap,int);
  lineColor = va_arg(ap,int);
  fillColor = va_arg(ap,int);

  structure->x = (double *)malloc(numPoints * sizeof(double));
  structure->y = (double *)malloc(numPoints * sizeof(double));

  structure->numberPoints = numPoints;
  structure->fillFlag = fillFlag;
  structure->lineColor = lineColor;
  structure->fillColor = fillColor;

  for (i=0;i<numPoints;i++) {
    structure->x[i] = va_arg(ap,double);
    structure->y[i] = va_arg(ap,double);
  }
  va_end(ap);

  structure->next = theNewWindow->structures;
  theNewWindow->structures = structure;
}

void XGStructureArray(int numPoints,STRUCT_FILL fillFlag,int lineColor,
                                                         int fillColor, SCALAR *
points ) 
{
  int i;
  StructType *structure = (StructType *)malloc(sizeof(StructType));

  structure->x = (double *)malloc(numPoints * sizeof(double));
  structure->y = (double *)malloc(numPoints * sizeof(double));

  structure->numberPoints = numPoints;
  structure->fillFlag = fillFlag;
  structure->lineColor = lineColor;
  structure->fillColor = fillColor;

  for (i=0;i<numPoints;i++) {
    structure->x[i] = points[2*i];
    structure->y[i] = points[2*i+1];
  }

  structure->next = theNewWindow->structures;
  theNewWindow->structures = structure;
}


void XGSetVec(const char *PlotType, const char *X_Label, const char *Y_Label,
	      const char *Z_Label,
	      const char *State, int ulx, int uly,
	      SCALAR X_Scale, SCALAR Y_Scale,
	      int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
	      SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max)
{
  int             plottype;
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  label_ptr = SetupLabelStruct();
  
  label_ptr->Z_Label = strdup(Z_Label);
  label_ptr->Z_Auto_Rescale = 0;

  label_ptr->Y_Label = strdup(Y_Label);
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;
  
  label_ptr->X_Label = strdup(X_Label);
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;

  if (!strcmp(PlotType, "vecvec"))
    plottype = VEC_VEC;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, 
	   Y_Label);
    exit(-1);
  }

  XGSetupWindow(Z_Label, State, ulx, uly, VECD, data_ptr, label_ptr, plottype);
}

void XGSetVecFlag(const char *PlotType, const char *X_Label,
		  const char *Y_Label, const char *Z_Label,
		  const char *State, int ulx, int uly, SCALAR X_Scale,
		  SCALAR Y_Scale,
		  int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
		  SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, int *openFlag)
{
  XGSetVec(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale,
	   Y_Scale, X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min,Y_Max);

  theWindowArray[numberOfWindows-1]->openFlag = openFlag;
}  

void XGSet3D(const char *PlotType, const char *X_Label, const char *Y_Label,
	     const char *Z_Label,
	     SCALAR Theta, SCALAR Phi, const char *State, int ulx, int uly,
	     SCALAR X_Scale, SCALAR Y_Scale, SCALAR Z_Scale, 
	     int X_Auto_Rescale, int Y_Auto_Rescale, int Z_Auto_Rescale,
	     SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, 
	     SCALAR Z_Min, SCALAR Z_Max)
{
  int             plottype;
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  init_theta = Theta;
  init_phi = Phi;

  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = strdup(Z_Label);
  label_ptr->Z_Min = Z_Min;
  label_ptr->Z_Max = Z_Max;
  label_ptr->Z_Scale = Z_Scale;
  label_ptr->Z_Auto_Rescale = Z_Auto_Rescale;

  label_ptr->Y_Label = strdup(Y_Label);
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;
  
  label_ptr->X_Label = strdup(X_Label);
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;
  
  if (!strcmp(PlotType, "linlinlin"))      plottype = LIN_LIN_LIN;
  else if (!strcmp(PlotType, "linlinlog")) plottype = LIN_LIN_LOG;
  else if (!strcmp(PlotType, "linloglin")) plottype = LIN_LOG_LIN;
  else if (!strcmp(PlotType, "linloglog")) plottype = LIN_LOG_LOG;
  else if (!strcmp(PlotType, "loglinlin")) plottype = LOG_LIN_LIN;
  else if (!strcmp(PlotType, "loglinlog")) plottype = LOG_LIN_LOG;
  else if (!strcmp(PlotType, "logloglin")) plottype = LOG_LOG_LIN;
  else if (!strcmp(PlotType, "logloglog")) plottype = LOG_LOG_LOG;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Z_Label);
    exit(-1);
  }

  XGSetupWindow(Z_Label, State, ulx, uly, THREED, data_ptr, label_ptr, 
		plottype);
}

void XGSet3DFlag(const char *PlotType, const char *X_Label,
		 const char *Y_Label, const char *Z_Label,
		 SCALAR Theta, SCALAR Phi, const char *State, int ulx, int uly,
		 SCALAR X_Scale, SCALAR Y_Scale, SCALAR Z_Scale, 
		 int X_Auto_Rescale, int Y_Auto_Rescale, int Z_Auto_Rescale,
		 SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, 
		 SCALAR Z_Min, SCALAR Z_Max, int *openFlag)
{
  XGSet3D(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	  X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	  Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max);
  
  theWindowArray[numberOfWindows-1]->openFlag = openFlag;
}

void XGSet2D(const char *PlotType, const char *X_Label, const char *Y_Label,
	     const char *State,
	     int ulx, int uly, SCALAR X_Scale, SCALAR Y_Scale, 
	     int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
	     SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max)
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  label_ptr = SetupLabelStruct();
  label_ptr->Y_Label = strdup(Y_Label);
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;

  label_ptr->X_Label = strdup(X_Label);
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;
  
  if (!strcmp(PlotType, "linlin"))      plottype = LIN_LIN;
  else if (!strcmp(PlotType, "linlog")) plottype = LIN_LOG;
  else if (!strcmp(PlotType, "loglog")) plottype = LOG_LOG;
  else if (!strcmp(PlotType, "loglin")) plottype = LOG_LIN;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
  XGSetupWindow(Y_Label, State, ulx, uly, TWOD, data_ptr, label_ptr, plottype);
}

void XGSet2D_OS(const char *PlotType, const char *X_Label, const char *Y_Label,
	     const char *State,
	     int ulx, int uly, SCALAR X_Offset, SCALAR Y_Offset,
             SCALAR X_Scale, SCALAR Y_Scale, 
	     int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
	     SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max)
{
  int             plottype;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  label_ptr = SetupLabelStruct();
  label_ptr->Y_Label = strdup(Y_Label);
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Offset = Y_Offset;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;

  label_ptr->X_Label = strdup(X_Label);
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Offset = X_Offset;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;
  
  if (!strcmp(PlotType, "linlin"))      plottype = LIN_LIN;
  else if (!strcmp(PlotType, "linlog")) plottype = LIN_LOG;
  else if (!strcmp(PlotType, "loglog")) plottype = LOG_LOG;
  else if (!strcmp(PlotType, "loglin")) plottype = LOG_LIN;
  else {
    fprintf(stderr, "Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
  XGSetupWindow(Y_Label, State, ulx, uly, TWOD, data_ptr, label_ptr, plottype);
}

void XGSet2DFlag(const char *PlotType, const char *X_Label,
		 const char *Y_Label, const char *State,
		 int ulx, int uly, SCALAR X_Scale, SCALAR Y_Scale, 
		 int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
		 SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, int *openFlag)
{
  XGSet2D(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	  X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max);

  theWindowArray[numberOfWindows-1]->openFlag = openFlag;
}

void XGSet2DFlag_OS(const char *PlotType, const char *X_Label,
	         const char *Y_Label, const char *State,
		 int ulx, int uly, SCALAR X_Offset, SCALAR Y_Offset,
		 SCALAR X_Scale, SCALAR Y_Scale, 
		 int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
		 SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, int *openFlag)
{
  XGSet2D_OS(PlotType, X_Label, Y_Label, State, ulx, uly, X_Offset, Y_Offset,
	  X_Scale, Y_Scale,
	  X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max);

  theWindowArray[numberOfWindows-1]->openFlag = openFlag;
}

void XGSet2DC(const char *PlotType, const char *X_Label, const char *Y_Label,
	      const char *Z_Label,
	      const char *State, int ulx, int uly,
	      SCALAR X_Scale, SCALAR Y_Scale, SCALAR Z_Scale,
	      int X_Auto_Rescale, int Y_Auto_Rescale, int Z_Auto_Rescale,
	      SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min, 
	      SCALAR Y_Max, SCALAR Z_Min, SCALAR Z_Max)
{
  XGSet3D(PlotType, X_Label, Y_Label, Z_Label, 0.0, 0.0, State, ulx, uly,
	  X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	  Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max);
}

void XGSet2DCFlag(const char *PlotType, const char *X_Label,
		  const char *Y_Label, char *Z_Label,
		  const char *State, int ulx, int uly,
	          SCALAR X_Scale, SCALAR Y_Scale, SCALAR Z_Scale,
		  int X_Auto_Rescale, int Y_Auto_Rescale, int Z_Auto_Rescale,
		  SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min,
		  SCALAR Y_Max, SCALAR Z_Min, SCALAR Z_Max, int *openFlag)
{
  XGSet3DFlag(PlotType, X_Label, Y_Label, Z_Label, 0.0, 0.0, State, ulx, uly,
	      X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	      Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max, 
	      openFlag);
}

void XGCurve(SCALAR *x_array, SCALAR *y_array, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;

  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->color = color;
  data_ptr->next = data_prevptr;

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGCurveVector(SCALAR *x_array, SCALAR *y_array, int *npoints, int color,
		    int xSize, int xOffset, int ySize, int yOffset)
{
  DataType data_ptr;

  XGCurve(x_array, y_array, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
}

void XGScat2D(SCALAR *x_array, SCALAR *y_array, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->color = color;
  data_ptr->next = data_prevptr;

  Set_Scatter(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGScat3D(SCALAR *x_array, SCALAR *y_array, SCALAR *z_array,
	      int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->zscat = z_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->mpoints = npoints;
  data_ptr->color = Color[color];
  data_ptr->next = data_prevptr;

  Set_Scatter(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGVector(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array, 
	      SCALAR **w_array, int *mpoints, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->w = z_array; /* VV: notice these are switched */
  data_ptr->z = w_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->mpoints = mpoints;
  data_ptr->color = color;
  data_ptr->next = data_prevptr;

  Set_Vector(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGVectorVector(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array, 
		    SCALAR **w_array, int *mpoints, int *npoints, int color,
		    int xSize, int xOffset, int ySize, int yOffset, int zSize,
		    int zOffset, int wSize, int wOffset)
{
  DataType        data_ptr;
  
  XGVector(x_array, y_array, z_array, w_array, mpoints, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
  data_ptr->zArraySize = wSize; /*DKW: these are switched as well */
  data_ptr->wArraySize = zSize;
  data_ptr->zArrayOffset = wOffset;
  data_ptr->wArrayOffset = zOffset;
}

void XGSurf(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array,
	    int *mpoints, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = z_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->mpoints = mpoints;
  data_ptr->npoints = npoints;
  data_ptr->color = Color[color];
  data_ptr->next = data_prevptr;

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGSurfVector(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array,
		  int *mpoints, int *npoints, int color, int xSize, 
		  int xOffset, int ySize, int yOffset, int zSize, int zOffset)
{
  DataType        data_ptr;
  
  XGSurf(x_array, y_array, z_array, mpoints, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->zArraySize = zSize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
  data_ptr->zArrayOffset = zOffset;
}

void XGCont(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array,
	    int *mpoints, int *npoints, int color)
{ 
  XGSurf(x_array, y_array, z_array, mpoints, npoints, color);
}

void XGIRSurf(SCALAR **x_array, SCALAR **y_array, SCALAR **z_array,
	      int *mpoints, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = z_array;
  data_ptr->y_irr = y_array;
  data_ptr->x_irr = x_array;
  data_ptr->mpoints = mpoints;
  data_ptr->npoints = npoints;
  data_ptr->color = Color[color];
  data_ptr->next = data_prevptr;

  Set_Irr_Surf(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGIRSurfVector(SCALAR **x_array, SCALAR **y_array, SCALAR **z_array,
		    int *mpoints, int *npoints, int color, int xSize, 
		    int xOffset, int ySize, int yOffset, int zSize, 
		    int zOffset)
{
  DataType        data_ptr;
  
  XGIRSurf(x_array, y_array, z_array, mpoints, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->zArraySize = zSize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
  data_ptr->zArrayOffset = zOffset;
}
