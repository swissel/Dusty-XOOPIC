#include "xgrafixint.h"

/****************************************************************/
/* Setup and malloc stuff for variable changer dealie, Payam 7.92 */

/****************************************************/

void ReallocateSpecials(void);

void ReallocateSpecials() 
{
  sizeOfSpecialArray += specialSizeIncrement;

  if(theSpecialArray==NULL)
    theSpecialArray=(SpecialType *)malloc(sizeOfSpecialArray*sizeof(SpecialType));
  else
    theSpecialArray=(SpecialType *)realloc(theSpecialArray,sizeOfSpecialArray*sizeof(SpecialType));
  if(theSpecialArray == NULL) {
    printf("Not enough memory to allocate specials\n");
    exit(1);
  }
}

void SetUpNewVar(void *spvar, const char *spname, const char *type)
{
  SpecialType     SpTemp;

  if (!theRunWithXFlag)
    return;

  SpTemp = (SpecialType) malloc(sizeof(struct special));
  SpTemp->data = spvar;
  strcpy(SpTemp->Varname, spname);
  if (!strcmp(type, "integer"))
    SpTemp->Type = INTEGER;
  else if (!strcmp(type, "float"))
    SpTemp->Type = FLOAT;
  else if (!strcmp(type, "double"))
    SpTemp->Type = DOUBLE;
  else if (!strcmp(type, "char"))
    SpTemp->Type = CHAR;
  else {
    printf("Unknown special type: %s.  Must be 'integer', 'double', 'float', or 'char'.\n", type);
    free(SpTemp);
    return;
  }

  if (numberOfSpecials == sizeOfSpecialArray)
    ReallocateSpecials();

  theSpecialArray[numberOfSpecials] = SpTemp;

  sprintf(TclCommand,"SetUpNewVar %d \"%s\" \"%s\"\n",numberOfSpecials++,
	  spname,type);
  if(Tcl_Eval(interp,TclCommand)!=TCL_OK) {
    printf("Error setting up variable\n");
    printf("%s\n",interp->result);
  }
}

