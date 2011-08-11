
/************************/
/* Including the C libs */

#include <stdio.h>
#include <unistd.h>
//#include <malloc/malloc.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "xgdatamacros.h"
#include "xgminint.h"

void PrintUsage(char *string)
{
  printf("Usage: %s [options]\n",string);
  printf("\t-d dumpfile[.dmp]:\n\t\tSpecify dump file name. Default is output.dmp\n");
  printf("\t-display displayname:\n\t\tSpecify the display. Default is DISPLAY environment variable.\n");
  printf("\t-dp n:\n\t\tSpecify the dump period. Default is 0 (no periodic dumps)\n");
  printf("\t-exit:\n\t\tExit after done with iterations\n");
  printf("\t-h:\n\t\tThis help message\n");
  printf("\t-i inputfile[.inp]:\n\t\tSpecify input file name. Default is no input file\n");
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


void XG_Quit()
{
  Quit();
  if (theCodeName) free(theCodeName);
  if (theInputFile) free(theInputFile);
  if (theDumpFile) free(theDumpFile);
  if (theEPSFile) free(theEPSFile);
  exit(0);
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
  theNumberOfSteps = 0;
  iterationsPerXUpdate = 1;
  theCurrentStep = 1;
  theDumpPeriod = 0;
  theIDumpFlag = FALSE;
  WasDumpFileGiven = FALSE;
  WasInputFileGiven = FALSE;
  theExitFlag = FALSE;

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
	  char *new_name;
	  new_name = (char *)malloc((strlen(argv[index])+strlen(theDumpExtension)+1)*sizeof(char));
	  sprintf(new_name,"%s%s",theDumpFile, theDumpExtension);
	  if(access(new_name,F_OK)) {
	    fprintf(stderr, "Warning: %s does not exist\n",theDumpFile); 
	/* mindgame: dumpfile is not given */
	    WasDumpFileGiven = FALSE;
	    free(theDumpFile); theDumpFile = NULL;
	    /*exit(1); */
	  }
	  else {
            strcat(theDumpFile, theDumpExtension);
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
	printf("MiniXGRAFIX doesn't display.");
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
    if (theEPSFile) sprintf(theEPSFile,"%s",theDefaultEPSFile);
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


void XGStart()
{
  char thePDumpFile[80], thePDFRoot[80];

  strncpy(thePDFRoot,theDumpFile,findlen(theDumpFile));
  thePDFRoot[findlen(theDumpFile)] = '\0';
  while (!theExitFlag) {
      
	 while ((theNumberOfSteps==0 || theCurrentStep<=theNumberOfSteps) ) {
		XGMainLoop();
		if (theDumpPeriod!=0 && theCurrentStep%theDumpPeriod==0) {
		  sprintf(thePDumpFile,"%s%d%s",thePDFRoot,
			  theCurrentStep/theDumpPeriod,theDumpExtension);
		  if (theIDumpFlag) Dump(thePDumpFile);
		  else Dump(theDumpFile);
		}
		theCurrentStep++;
	 }
	 if (theDumpPeriod!=0 && theCurrentStep%theDumpPeriod==0) {
	   sprintf(thePDumpFile,"%s%d%s",thePDFRoot,
		   theCurrentStep/theDumpPeriod,theDumpExtension);
	   if (theIDumpFlag) Dump(thePDumpFile);
	   else Dump(theDumpFile);
	 }
	 if(theCurrentStep>theNumberOfSteps) {
		XG_Quit();
	 }	
  }
  XG_Quit();
}


