

/************************/
/* Including the C libs */

#include <stdio.h>
#include <unistd.h>
//#include <malloc/malloc.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "xgdatamacros.h"
/************************/

/*void Bin_TwoD_Window(WindowType theWindow, char *filename, char type) */

void BXG_Write_2D(DataType theData,LabelType theLabel,StructType *structs,char *filename, char type, double *theTimeStep)
{
  FILE *fp;
  /*DataType  theData = theWindow->data;
  LabelType theLabel= theWindow->label;*/
  int i,n;
  double xscale, yscale;
  double xoffset, yoffset;
  SCALAR theTime;
  char buf[512];
  xscale = theLabel->X_Scale;
  yscale = theLabel->Y_Scale;
  xoffset = theLabel->X_Offset;
  yoffset = theLabel->Y_Offset;
  
  strcpy(buf,filename);
  i = strlen(buf);
  sprintf(buf +i -3,"%s","num"); 
  
  if((fp = fopen(buf,"r"))==NULL) {
	 if((fp = fopen(buf,"w")) == NULL) {
		fprintf(stderr,"Error, cannot open %s",buf);
		return;
	 }
	 n=0;
	 fprintf(fp,"1");
	 fclose(fp);  /* write the first file count */
  }
  else {
	 fscanf(fp,"%d",&n);
	 n+=1;
	 fclose(fp);
	 fp=fopen(buf,"w");
	 fprintf(fp,"%d",n);
	 fclose(fp);
  }

  fp = fopen(filename,"a");  /* always append */	 
  
  if(fp == NULL) {
    printf("Bin_TwoD_Window couldn't create file\n");
    return;
  }

  theTime = *theTimeStep;
  /* tag the file */
  XGWriteStructures(structs,filename);  
  i = LINE_PLOT;
  if(Is_Scatter(theData)) i = SCATTER_PLOT;
  XGWrite(&i,sizeof(int),1,fp,"int");
  XGWriteLabel(theLabel,fp);
  XGWrite(&theTime,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  
  while(theData) {
    SCALAR theTime;
    n = *(theData->npoints);

    
    /*fprintf(fp,"\n##### time= %g \n", *theTimeStep);*/
	 XGWrite(&n,sizeof(int),1,fp,"int");
	 i = theData->color;
	 XGWrite(&i,sizeof(int),1,fp,"int");

	 for(i=0;i<n;i++)
		{
		  SCALAR x,y;
		  x = xscale * CurveX(theData,i) + xoffset;
		  y = yscale * CurveY(theData,i) + yoffset;
		  XGWrite(&x,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		  XGWrite(&y,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		}    
    theData = theData->next;
  }
  /* the EOF mark of the data set */
  n=-1;
  XGWrite(&n,sizeof(int),1,fp,"int");
  XGWrite(&n,sizeof(int),1,fp,"int");
  fclose(fp);
}

/****************************************************************/




/*void Bin_ThreeD_Window(WindowType theWindow, char *filename, char type) */
void BXG_Write_3D(DataType theData, LabelType theLabel,char *filename, char type, double *theTimeStep)
{ 
  FILE *fp;
  /*  DataType theData = theWindow->data;
  LabelType theLabel= theWindow->label;*/
  int i,j,m,n;
  char buf[512];
  double xscale, yscale, zscale;
  SCALAR x,y,z, theTime;
  xscale = theLabel->X_Scale;
  yscale = theLabel->Y_Scale;
  zscale = theLabel->Z_Scale;
  
    strcpy(buf,filename);
  i = strlen(buf);
  sprintf(buf +i -3,"%s","num"); 
  
  if((fp = fopen(buf,"r"))==NULL) {
	 if((fp = fopen(buf,"w")) == NULL) {
		fprintf(stderr,"Error, cannot open %s",buf);
		return;
	 }
	 n=0;
	 fprintf(fp,"1");
	 fclose(fp);  /* write the first file count */
  }
  else {
	 fscanf(fp,"%d",&n);
	 n+=1;
	 fclose(fp);
	 fp=fopen(buf,"w");
	 fprintf(fp,"%d",n);
	 fclose(fp);
  }
  fp = fopen(filename,"a");

  if(fp == NULL) {
    printf("Bin_ThreeD_Window couldn't create file\n");
    return;
  }

  /*fprintf(fp,"\n##### time= %g \n", *theTimeStep);*/

  theTime = *theTimeStep;
  n = *(theData->npoints);
  
  if (Is_Scatter(theData)) {

	 i = SCATTER3D;
	 XGWrite(&i,sizeof(int),1,fp,"int");
	 XGWriteLabel(theLabel,fp);
    XGWrite(&theTime,sizeof(SCALAR),1,fp,SCALAR_CHAR);
	 XGWrite(&n,sizeof(int),1,fp,"int");


    for(i=0;i<n;i++) {
		  x = xscale * ScatterX(theData,i);
		  y = yscale * ScatterY(theData,i);
		  z = zscale * ScatterZ(theData,i);
		  XGWrite(&x,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		  XGWrite(&y,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		  XGWrite(&z,sizeof(SCALAR),1,fp,SCALAR_CHAR);
    }
  } else {
    m = *(theData->mpoints);
    
    if (Is_Irr_Surf(theData)) {

		i = IRREGULAR3D;
		XGWrite(&i,sizeof(int),1,fp,"int");
		XGWriteLabel(theLabel,fp);
		XGWrite(&theTime,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		XGWrite(&n,sizeof(int),1,fp,"int");
		XGWrite(&m,sizeof(int),1,fp,"int");

      for (i=0;i<m;i++) {
		  for (j=0;j<n;j++) {
			 x = xscale * IrregularX(theData,i,j);
			 y = yscale * IrregularY(theData,i,j);
			 z = zscale * IrregularZ(theData,i,j);
			 XGWrite(&x,sizeof(SCALAR),1,fp,SCALAR_CHAR);
			 XGWrite(&y,sizeof(SCALAR),1,fp,SCALAR_CHAR);
			 XGWrite(&z,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		  }
      }
    } else {
		i = SURFACE_PLOT;
		XGWrite(&i,sizeof(int),1,fp,"int");
		XGWriteLabel(theLabel,fp);
		XGWrite(&theTime,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		XGWrite(&n,sizeof(int),1,fp,"int");
		XGWrite(&m,sizeof(int),1,fp,"int");

		for (i=0;i<m;i++) {
		  x = xscale * SurfaceX(theData,i);
		  XGWrite(&x,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		}
		for (i=0;i<n;i++) {
		  y = yscale * SurfaceY(theData,i);
		  XGWrite(&y,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		}
      for (i=0;i<m;i++) {
		  for (j=0;j<n;j++) {
			 z = zscale * SurfaceZ(theData,i,j);
			 XGWrite(&z,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		  }
      }
    }
  }

  fclose(fp);
}


/****************************************************************/

/*void Bin_Vector_Window(WindowType theWindow, char *filename, char type)*/
void  BXG_Write_Vector(DataType theData,LabelType theLabel, StructType *structs,char *filename,char type, double *theTimeStep)
{
  FILE *fp;
  /*DataType theData = theWindow->data; */

  int i,j,m,n;
  SCALAR x,y,w,z;
  SCALAR theTime;
  char buf[512];

  
  strcpy(buf,filename);
  i = strlen(buf);
  sprintf(buf +i -3,"%s","num"); 
  
  if((fp = fopen(buf,"r"))==NULL) {
	 if((fp = fopen(buf,"w")) == NULL) {
		fprintf(stderr,"Error, cannot open %s",buf);
		return;
	 }
	 n=0;
	 fprintf(fp,"1");
	 fclose(fp);  /* write the first file count */
  }
  else {
	 fscanf(fp,"%d",&n);
	 n+=1;
	 fclose(fp);
	 fp=fopen(buf,"w");
	 fprintf(fp,"%d",n);
	 fclose(fp);
  }

  fp = fopen(filename,"a");


  if(fp == NULL) {
    printf("Bin_Vector_Window couldn't create file\n");
    return;
  }

  m = *(theData->mpoints);
  n = *(theData->npoints);
  XGWriteStructures(structs,filename);
  i = VECTOR_PLOT;
  theTime = *theTimeStep;
  XGWrite(&i,sizeof(int),1,fp,"int");
  XGWriteLabel(theLabel,fp);
  XGWrite(&theTime,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&n,sizeof(int),1,fp,"int");
  XGWrite(&m,sizeof(int),1,fp,"int");

  for(i=0;i<m;i++) {
	 x = VectorX(theData,i);
	 XGWrite(&x,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  }
  for(j=0;j<n;j++) {
	 y = VectorY(theData,j);
	 XGWrite(&y,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  }
  for (i=0;i<m;i++) {
    for (j=0;j<n;j++) {
		w = VectorW(theData,i,j);
		XGWrite(&w,sizeof(SCALAR),1,fp,SCALAR_CHAR);
    }
    for (j=0;j<n;j++) {
		z = VectorZ(theData,i,j);
		XGWrite(&z,sizeof(SCALAR),1,fp,SCALAR_CHAR);
    }
  }
  fclose(fp);
}


void XGWriteLabel(LabelType theLabel, FILE *fp) {
  SCALAR scale,min,max;
  int i;
  int l;
  /* first write the three label strings */
  if(theLabel->X_Label) {
	 l = strlen(theLabel->X_Label);
	 XGWrite(&l,sizeof(int),1,fp,"int");
	 XGWrite(theLabel->X_Label,sizeof(char),l+1,fp,"char");
  }
  else {
	 l=0;
	 XGWrite(&l,sizeof(int),1,fp,"int");
  }

  if(theLabel->Y_Label) {
	 l = strlen(theLabel->Y_Label);
	 XGWrite(&l,sizeof(int),1,fp,"int");
	 XGWrite(theLabel->Y_Label,sizeof(char),l+1,fp,"char");
  }
  else {
	 l=0;
	 XGWrite(&l,sizeof(int),1,fp,"int");
  }

  if(theLabel->Z_Label) {
	 l = strlen(theLabel->Z_Label);
	 XGWrite(&l,sizeof(int),1,fp,"int");
	 XGWrite(theLabel->Z_Label,sizeof(char),l+1,fp,"char");
  }
  else {
	 l=0;
	 XGWrite(&l,sizeof(int),1,fp,"int");
  }

  scale = theLabel->X_Scale;
  min = theLabel->X_Min;
  max = theLabel->X_Max;
  l = theLabel->X_Auto_Rescale;
  XGWrite(&scale,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&min,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&max,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&l,sizeof(int),1,fp,"int");

  scale = theLabel->Y_Scale;
  min = theLabel->Y_Min;
  max = theLabel->Y_Max;
  l = theLabel->Y_Auto_Rescale;
  XGWrite(&scale,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&min,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&max,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&l,sizeof(int),1,fp,"int");

  scale = theLabel->Z_Scale;
  min = theLabel->Z_Min;
  max = theLabel->Z_Max;
  l = theLabel->Z_Auto_Rescale;
  XGWrite(&scale,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&min,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&max,sizeof(SCALAR),1,fp,SCALAR_CHAR);
  XGWrite(&l,sizeof(int),1,fp,"int");

}

void XGWriteStructures(StructType *s, const char *filename) {
  FILE *fp;
  char buf[512];
  int l;
  SCALAR temp;
  char *dest;
  /* no structures */

  if(!s) return;
  strcpy(buf,filename);
  if((dest=strstr(buf,"_")))  /* if an underscore is found in the filename*/
	 {
		sprintf(dest,".str");  
	 }
  else
	 {
		l = strlen(buf);
		sprintf(buf +l -3,"%s","str");
	 }
  /*  if there's already a struct file return:  we don't need to redo it. */
  if((fp=fopen(buf,"r"))) {
	 fclose(fp);
	 return;
  }
  if((fp=fopen(buf,"w"))==NULL) {
	 fprintf(stderr,"\nError: cannot open file %s\n",buf);
	 return;
  }

  while(s) {
	 XGWrite(&(s->numberPoints),sizeof(int),1,fp,"int");
	 l = s->fillFlag;
	 XGWrite(&l,sizeof(int),1,fp,"int");
	 XGWrite(&(s->lineColor),sizeof(int),1,fp,"int");
	 XGWrite(&(s->fillColor),sizeof(int),1,fp,"int");
	 for(l=0;l<s->numberPoints;l++) {
		temp = s->x[l];
		XGWrite(&temp,sizeof(SCALAR),1,fp,SCALAR_CHAR);
		temp = s->y[l];
		XGWrite(&temp,sizeof(SCALAR),1,fp,SCALAR_CHAR);
	 }
	 s=s->next;
  }
  l = -1;
  XGWrite(&l,sizeof(int),1,fp,"int");/* the end marker */
  fclose(fp);
}
