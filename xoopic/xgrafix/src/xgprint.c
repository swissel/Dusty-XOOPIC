#include "xgrafixint.h"


#define  BACK         0
#define  FRONT        1

#define  ABOVE        0
#define  UPPER        1
#define  LOWER        2

#define  FIRST_QUAD   0
#define  SECOND_QUAD  1
#define  THIRD_QUAD   2
#define  FOURTH_QUAD  3
#define Vlength(x,y) (sqrt((x)*(x)+(y)*(y)))

/****************************************************************/

#define	NPOINTMAX   500

void PaintWall(MeshType *theBox, SCALAR c1, SCALAR d1, SCALAR c2, SCALAR d2,
	       int wall, int side, FILE *fp)
{
  XPoint          thePoint[4];

  if (wall == 0) {
    thePoint[0].x = c1 * theBox[0].x + d1;
    thePoint[0].y = c2 * theBox[0].y + d2;
    thePoint[1].x = c1 * theBox[1].x + d1;
    thePoint[1].y = c2 * theBox[1].y + d2;
    thePoint[2].x = c1 * theBox[2].x + d1;
    thePoint[2].y = c2 * theBox[2].y + d2;
    thePoint[3].x = c1 * theBox[3].x + d1;
    thePoint[3].y = c2 * theBox[3].y + d2;
  } else if (wall == 1) {
    thePoint[0].x = c1 * theBox[0].x + d1;
    thePoint[0].y = c2 * theBox[0].y + d2;
    thePoint[1].x = c1 * theBox[1].x + d1;
    thePoint[1].y = c2 * theBox[1].y + d2;
    thePoint[2].x = c1 * theBox[6].x + d1;
    thePoint[2].y = c2 * theBox[6].y + d2;
    thePoint[3].x = c1 * theBox[5].x + d1;
    thePoint[3].y = c2 * theBox[5].y + d2;
  } else if (wall == 2) {
    thePoint[0].x = c1 * theBox[1].x + d1;
    thePoint[0].y = c2 * theBox[1].y + d2;
    thePoint[1].x = c1 * theBox[2].x + d1;
    thePoint[1].y = c2 * theBox[2].y + d2;
    thePoint[2].x = c1 * theBox[7].x + d1;
    thePoint[2].y = c2 * theBox[7].y + d2;
    thePoint[3].x = c1 * theBox[6].x + d1;
    thePoint[3].y = c2 * theBox[6].y + d2;
  } else if (wall == 3) {
    thePoint[0].x = c1 * theBox[2].x + d1;
    thePoint[0].y = c2 * theBox[2].y + d2;
    thePoint[1].x = c1 * theBox[3].x + d1;
    thePoint[1].y = c2 * theBox[3].y + d2;
    thePoint[2].x = c1 * theBox[4].x + d1;
    thePoint[2].y = c2 * theBox[4].y + d2;
    thePoint[3].x = c1 * theBox[7].x + d1;
    thePoint[3].y = c2 * theBox[7].y + d2;
  } else if (wall == 4) {
    thePoint[0].x = c1 * theBox[3].x + d1;
    thePoint[0].y = c2 * theBox[3].y + d2;
    thePoint[1].x = c1 * theBox[0].x + d1;
    thePoint[1].y = c2 * theBox[0].y + d2;
    thePoint[2].x = c1 * theBox[5].x + d1;
    thePoint[2].y = c2 * theBox[5].y + d2;
    thePoint[3].x = c1 * theBox[4].x + d1;
    thePoint[3].y = c2 * theBox[4].y + d2;
  } else if (wall == 5) {
    thePoint[0].x = c1 * theBox[4].x + d1;
    thePoint[0].y = c2 * theBox[4].y + d2;
    thePoint[1].x = c1 * theBox[5].x + d1;
    thePoint[1].y = c2 * theBox[5].y + d2;
    thePoint[2].x = c1 * theBox[6].x + d1;
    thePoint[2].y = c2 * theBox[6].y + d2;
    thePoint[3].x = c1 * theBox[7].x + d1;
    thePoint[3].y = c2 * theBox[7].y + d2;
  }
  if (side == BACK)
    fprintf(fp, "%d %d %d %d %d %d %d %d bbwfpoly\n",
	    thePoint[0].x, thePoint[0].y, thePoint[1].x, thePoint[1].y,
	    thePoint[2].x, thePoint[2].y, thePoint[3].x, thePoint[3].y);
  else
    fprintf(fp, "%d %d %d %d %d %d %d %d poly\n",
	    thePoint[0].x, thePoint[0].y, thePoint[1].x, thePoint[1].y,
	    thePoint[2].x, thePoint[2].y, thePoint[3].x, thePoint[3].y);
}

/****************************************************************/

void PostScriptOpenWindow(WindowType theWindow, const char *eps_file_name,
			  const char *plot_title)
{
  DataType        data_Ptr, ptr;
  LabelType       label_Ptr;
  int             i, j, n;
  FILE           *fp;
  StructType     *structures;
  XPoint         *StructPoints;
  int             x1, x2, y1, y2;
  double          bx, by, mx, my;

  if (!(fp = fopen(eps_file_name, "a"))) {
    printf("PostScriptOpenWindow(): File was not opened properly!");
    return;
  }

  x1 = 132;
  x2 = 684;
  y1 = 108;
  y2 = 504;

  data_Ptr = theWindow->data;
  label_Ptr = theWindow->label;
  
  /************************/
  /* Translate and rotate */

  fprintf(fp, "%%Translate and rotate\n");
  fprintf(fp, "612 0 translate\n");
  fprintf(fp, "90 rotate\n");
  
  /************************************************/
  /* Limits of the bounding box (PS and physical) */
  
  fprintf(fp, "%%Limits of the bounding box (PS and physical)\n"); 
  fprintf(fp, "/xps1 %d def\n",x1); 
  fprintf(fp, "/yps1 %d def\n",y1); 
  fprintf(fp, "/xps2 %d def\n",x2);
  fprintf(fp, "/yps2 %d def\n",y2);
  fprintf(fp, "/xmin %.6G def\n", label_Ptr->X_Min); 
  fprintf(fp, "/xmax %.6G def\n", label_Ptr->X_Max);
  fprintf(fp, "/xscale %.6G def\n", label_Ptr->X_Scale);
  fprintf(fp, "/ymin %.6G def\n", label_Ptr->Y_Min); 
  fprintf(fp, "/ymax %.6G def\n", label_Ptr->Y_Max);
  fprintf(fp, "/yscale %.6G def\n\n", label_Ptr->Y_Scale);
  
  /*****************/
  /* Draw boundary */
  
  fprintf(fp, "%%Draw boundary, and Draw X and Y limits\n"); 
  fprintf(fp, "1 setlinewidth\n");
  fprintf(fp, "0 setgray\n");
  fprintf(fp, "%d %d %d %d rectangle\n\n",x1,y1,x2,y2);

  /***********************/
  /* Draw X and Y limits */
  
  fprintf(fp, "/Times-Bold findfont 12 scalefont setfont\n");
  fprintf(fp, "xps1 yps1 16 sub moveto\n");
  fprintf(fp, "xmin str cvs prtstrfromlftup\n");
  fprintf(fp, "xps2 yps1 16 sub moveto\n");
  fprintf(fp, "xmax str cvs prtstrfromritup\n");
  fprintf(fp, "xps1 8 sub yps2 9 sub moveto\n");
  fprintf(fp, "ymax str cvs prtstrfromritup\n");
  fprintf(fp, "xps1 8 sub yps1 moveto\n");
  fprintf(fp, "ymin str cvs prtstrfromritlow\n\n");

  /************************/
  /* Printing the labels */
  
  fprintf(fp, "%%Printing the labels\n");
  fprintf(fp, "90 rotate\n");
  fprintf(fp, "/Times-BoldItalic findfont 12 scalefont setfont\n");
  fprintf(fp, "yps1 yps2 add 2 div 8 xps1 sub moveto\n");
  fprintf(fp, "(%s) prtstrfrommidlow\n", label_Ptr->Y_Label);
  fprintf(fp, "-90 rotate\n");

  fprintf(fp, "xps1 xps2 add 2 div yps1 16 sub moveto\n");
  fprintf(fp, "(%s) prtstrfrommidup\n", label_Ptr->X_Label);

  if (strcmp(plot_title, label_Ptr->Y_Label)){ /* print title only if different */
    fprintf(fp, "xps1 xps2 add 2 div yps2 8 add moveto\n");
    fprintf(fp, "/Times-Bold findfont 14 scalefont setfont\n");
    fprintf(fp, "(%s) prtstrfrommidlow\n\n", plot_title);
  }
  
  /****************************/
  /* Rescale the window first */

  RescaleTwoDWindow(theWindow);

  /************************************************************/
  /* If the plot limits are the same, close the file and quit */
  
  if (label_Ptr->Y_Max == label_Ptr->Y_Min || label_Ptr->X_Max == label_Ptr->X_Min) {
    fprintf(fp, "grestore\n");
    fprintf(fp, "showpage\n\n");

    if (fclose(fp))
      printf("PostScriptOpenWindow(): File was not closed properly!");
    return;
  }
  
  /**********************************/
  /* Setting up the clipping region */
  
  fprintf(fp, "%%Setting up the clipping region\n");
  fprintf(fp, "/boundary {newpath xps1 yps1 moveto xps2 yps1 lineto\n");
  fprintf(fp, " xps2 yps2 lineto xps1 yps2 lineto closepath} def\n");
  fprintf(fp, "gsave boundary clip\n\n");
  
  /***************************/
  /* Drawing the tick marks  */
  
  fprintf(fp, "%%Drawing the tick marks\n");
  fprintf(fp, ".8 setlinewidth\n");
  
  if (Is_X_Log(theWindow)) {
    fprintf(fp, "/dtklog {xmax xmin div log cvi} def\n");
    fprintf(fp, "1 1 dtklog {doxlogtickmarks} for\n");
  } else 
    fprintf(fp, "doxlintickmarks\n");
  
  if (Is_Y_Log(theWindow)) {
    fprintf(fp, "/dtklog {ymax ymin div log cvi} def\n");
    fprintf(fp, "1 1 dtklog {doylogtickmarks} for\n\n");
  } else
    fprintf(fp, "doylintickmarks\n\n");
  
  /************************************/
  /* Determining scaling coefficients */
  
  fprintf(fp, "%%Determining scaling coefficients\n");
  if (Is_Y_Log(theWindow)) {
    fprintf(fp, "/my {yps2 yps1 sub ymax ymin div log div} def\n"); 
    fprintf(fp, "/by {yps1 ymax log mul yps2 ymin log mul sub ymax ymin div log div 0.5 add} def\n");
  } else {
    fprintf(fp, "/my {yps2 yps1 sub ymax ymin sub div yscale mul} def\n"); 
    fprintf(fp, "/by {yps1 ymax mul yps2 ymin mul sub ymax ymin sub div 0.5 add} def\n");
  }
  if (Is_X_Log(theWindow)) {
    fprintf(fp, "/mx {xps2 xps1 sub xmax xmin div log div} def\n"); 
    fprintf(fp, "/bx {xps1 xmax log mul xps2 xmin log mul sub xmax xmin div log div 0.5 add} def\n\n");
  } else {
    fprintf(fp, "/mx {xps2 xps1 sub xmax xmin sub div xscale mul} def\n"); 
    fprintf(fp, "/bx {xps1 xmax mul xps2 xmin mul sub xmax xmin sub div 0.5 add} def\n\n");
  }
  
  if (Is_X_Log(theWindow) && Is_Y_Log(theWindow)) {
    fprintf(fp, "/mv {exch xscale mul log mx mul bx add exch yscale mul log my mul by add moveto} def\n"); 
    fprintf(fp, "/ln {exch xscale mul log mx mul bx add exch yscale mul log my mul by add lineto} def\n\n"); 
  }
  else if(Is_X_Log(theWindow) && !Is_Y_Log(theWindow)) {
    fprintf(fp, "/mv {exch xscale mul log mx mul bx add exch my mul by add moveto} def\n"); 
    fprintf(fp, "/ln {exch xscale mul log mx mul bx add exch my mul by add lineto} def\n\n");
  }
  else if(!Is_X_Log(theWindow) && Is_Y_Log(theWindow)) {
    fprintf(fp, "/mv {exch mx mul bx add exch yscale mul log my mul by add moveto} def\n"); 
    fprintf(fp, "/ln {exch mx mul bx add exch yscale mul log my mul by add lineto} def\n\n"); 
  }
  else {
    fprintf(fp, "/mv {exch mx mul bx add exch my mul by add moveto} def\n"); 
    fprintf(fp, "/ln {exch mx mul bx add exch my mul by add lineto} def\n\n"); 
  }

  /***************************/
  /* Printing the structures */
  
  mx = (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN);
  bx = (label_Ptr->X_Max * x1 - label_Ptr->X_Min * x2)
    / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) + 0.5;
  my = (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN);
  by = (label_Ptr->Y_Max * y1 - label_Ptr->Y_Min * y2)
    / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) + 0.5;

  structures = theWindow->structures;
  while(structures) {
    n = structures->numberPoints;
    
    if (!(StructPoints = (XPoint *)malloc(n*sizeof(XPoint))))
      putchar(7);

    if (Is_X_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].x = mx * log10(max(label_Ptr->X_Scale *
					   structures->x[i],
					   DBL_MIN)) + bx;
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].x = mx * structures->x[i] + bx;
    }	 

    if (Is_Y_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].y = my * log10(max(label_Ptr->Y_Scale * 
					  structures->y[i],
					  DBL_MIN)) + by;
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].y = my * structures->y[i] + by;
    }

    fprintf(fp, "%%Printing the structures\n");
    fprintf(fp, "/Times-Bold findfont 4 scalefont setfont\n");
    fprintf(fp, "newpath\n");
    fprintf(fp, "%d %d moveto\n", StructPoints[0].x, StructPoints[0].y);

    for (i = 1; i < n; i++) {
      fprintf(fp, "%d %d lineto\n", StructPoints[i].x, StructPoints[i].y);
    }

    fprintf(fp, "1 setlinewidth\n");
    fprintf(fp, "0 setgray\n");
    if(structures->fillFlag == FILLED) {
      fprintf(fp, "fill\n");
    }

    fprintf(fp, "1 setlinewidth\nstroke\n\n");

    free(StructPoints);
    structures = structures->next;
  }

  /*****************************/
  /* Printing the actual plots */
  
  fprintf(fp, "%%Printing the actual plots\n");
  fprintf(fp, "1.5 setlinewidth\n");
  fprintf(fp, "0 setgray\n");
  
  if (label_Ptr->X_Scale != 0)
    mx = label_Ptr->X_Offset/label_Ptr->X_Scale;
  else mx = 0;

  if (label_Ptr->Y_Scale != 0)
    my = label_Ptr->Y_Offset/label_Ptr->Y_Scale;
  else my = 0;
  
  for (ptr = data_Ptr, j = 0; ptr != NULL; ptr = ptr->next, j++) {
    n = *(ptr->npoints);
    
    if (Is_Scatter(ptr)) {
      fprintf(fp, "Dash0\n");
      fprintf(fp, "/Times-Bold findfont 6 scalefont setfont\n");
      for (i = 1; i < n; i++)
        fprintf(fp, "%.6G %.6G mv sp%d\n", CurveX(ptr,i) + mx, CurveY(ptr,i) + my, j);
      fprintf(fp, "\n\n");
    }
    else {
      fprintf(fp, "Dash%d\n", j);
      fprintf(fp, "newpath %.6G %.6G mv\n", CurveX(ptr,0) + mx, CurveY(ptr,0) + my);
      for (i = 1; i < n; i++) {
       bx = CurveX(ptr,i)+mx;
       by = CurveY(ptr,i)+my;
	/* COMMENT try to avoid taking log(x <= 0) */
       if (bx > 0 || !Is_Y_Log(theWindow)) {
        fprintf(fp, "%.6G %.6G ln\n", bx, by);}
       else {
 	fprintf(fp, "%.6G %.6G\n", bx, by);}
       if (!(i % NPOINTMAX)) {
          fprintf(fp, "stroke\n\n");
          fprintf(fp, "newpath %.6G %.6G mv\n", bx, by);
        }
      }
      fprintf(fp, "stroke\n\n");
    }
  }

  fprintf(fp, "grestore\n");
  fprintf(fp, "showpage\n\n");
  if (fclose(fp))
    printf("PostScriptOpenWindow(): File was not closed properly!");
}

/****************************************************************/

void PostScript_Vector_Window(WindowType theWindow, const char *eps_file_name, 
			      const char *plot_title)
{
  DataType        data_Ptr;
  LabelType       label_Ptr;
  int             i, j, m, n, nstart = 0, mstart = 0, nend, mend;
  int             x1, x2, y1, y2;
  double          dtick, bx, by, mx, my, maxvlen = 0.0, w1, w2, X0, Y0;
  MeshType      **Point, **Vector;
  FILE           *fp;
  StructType     *structures;
  XPoint         *StructPoints;
  XColor          theRGBColor, theHardwareColor;

  if (!(fp = fopen(eps_file_name, "a"))) {
    printf("PostScriptOpenWindow(): File was not opened properly!");
    return;
  }

  /*****************************/

  data_Ptr = theWindow->data;
  label_Ptr = theWindow->label;

  x1 = 132; /* modified to accomodate wider labels (JohnV) */
  x2 = 684;
  y1 = 108;
  y2 = 504;

  fprintf(fp, "\n612 0 translate\n");
  fprintf(fp, "90 rotate\n");
  fprintf(fp, "%%Limits of the bounding box (PS and physical)\n"); 
  fprintf(fp, "/xps1 %d def\n",x1); 
  fprintf(fp, "/yps1 %d def\n",y1); 
  fprintf(fp, "/xps2 %d def\n",x2);
  fprintf(fp, "/yps2 %d def\n",y2);

  /************************/
  /* Printing the labels */
  fprintf(fp, "90 rotate\n");
  fprintf(fp, "/Times-BoldItalic findfont 12 scalefont setfont\n");
  fprintf(fp, "yps1 yps2 add 2 div -100 moveto\n");
  fprintf(fp, "(%s) prtstrfrommidlow\n", label_Ptr->Y_Label);
  fprintf(fp, "-90 rotate\n");

  fprintf(fp, "xps1 xps2 add 2 div yps1 16 sub moveto\n");
  fprintf(fp, "(%s) prtstrfrommidup\n", label_Ptr->X_Label);

  fprintf(fp, "/Times-Bold findfont 12 scalefont setfont\n");
  fprintf(fp, "xps1 yps1 12 sub moveto\n");
  fprintf(fp, "(%.3G) prtstrfromlftup\n", label_Ptr->X_Min);
  fprintf(fp, "xps2 yps1 12 sub moveto\n");
  fprintf(fp, "(%.3G) prtstrfromritup\n", label_Ptr->X_Max);

  fprintf(fp, "xps1 xps2 add 2 div yps2 8 add moveto\n");
  fprintf(fp, "/Times-Bold findfont 14 scalefont setfont\n");
  fprintf(fp, "(%s) prtstrfrommidlow\n", plot_title);
  fprintf(fp, "/Times-Bold findfont 12 scalefont setfont\n");
  fprintf(fp, "xps1 8 sub yps2 8 sub moveto\n");
  fprintf(fp, "(%.3G) prtstrfromritup\n", label_Ptr->Y_Max);
  fprintf(fp, "xps1 8 sub yps1 moveto\n");
  fprintf(fp, "(%.3G) prtstrfromritlow\n", label_Ptr->Y_Min);

  if (label_Ptr->Y_Max == label_Ptr->Y_Min || label_Ptr->X_Max == label_Ptr->X_Min)
    return;

  /*****************************/
  /* Printing the actual plots */

  m = *(data_Ptr->mpoints);
  n = *(data_Ptr->npoints);

  RescaleVectorDWindow(theWindow,&mstart,&mend,&nstart,&nend);

  /****************************/
  /* find longest vector in the system */

  maxvlen = Vlength(VectorZ(data_Ptr,mstart,nstart), 
		    VectorW(data_Ptr,mstart,nstart));
  for (i = mstart; i <= mend; i++)
    for (j = nstart; j <= nend; j++)
      maxvlen = max(maxvlen, Vlength(VectorZ(data_Ptr,i,j), 
				     VectorW(data_Ptr,i,j)));

  mx = (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) * label_Ptr->X_Scale;
  bx = (label_Ptr->X_Max * x1 - label_Ptr->X_Min * x2)
    / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) + 0.5;
  my = (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) * label_Ptr->Y_Scale;
  by = (label_Ptr->Y_Max * y1 - label_Ptr->Y_Min * y2)
    / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) + 0.5;

  if (maxvlen != 0.0) {
    w1 = (x2 - x1) / (mend - mstart + 1) / maxvlen;
    w2 = (y2 - y1) / (nend - nstart + 1) / maxvlen;
  } else {
    w1 = 0.0;
    w2 = 0.0;
  }

  /* malloc space for the arrays */
  if (!(Point = (MeshType **) malloc(m * sizeof(MeshType *))))
    printf("PostScript_Vector_Window: malloc failed");
  for (i = 0; i < m; i++)
    if (!(Point[i] = (MeshType *) malloc(n * sizeof(MeshType))))
      printf("PostScript_Vector_Window: malloc failed");

  if (!(Vector = (MeshType **) malloc(m * sizeof(MeshType *))))
    printf("PostScript_Vector_Window: malloc failed");
  for (i = 0; i < m; i++)
    if (!(Vector[i] = (MeshType *) malloc(n * sizeof(MeshType))))
      printf("PostScript_Vector_Window: malloc failed");

  /* calculate what the vectors are */
  for (i = mstart; i <= mend; i++)
    for (j = nstart; j <= nend; j++) {
      Point[i][j].x = X0 = mx * VectorX(data_Ptr,i) + bx;
      Point[i][j].y = Y0 = my * VectorY(data_Ptr,j) + by;
      Vector[i][j].x = X0 + VectorW(data_Ptr,i,j) * w1;
      Vector[i][j].y = Y0 + VectorZ(data_Ptr,i,j) * w2;
    }

  /* Set up a clipping region */
  fprintf(fp, ".25 setlinewidth\n\n");
/*  fprintf(fp, "stroke grestore\n\n"); */
  fprintf(fp,"/boundary {newpath %d %d moveto %d %d lineto\n", x1, y1, x2, y1);
  fprintf(fp, " %d %d lineto %d %d lineto closepath} def\n", x2, y2, x1, y2);
  fprintf(fp, "gsave boundary clip\n");

  /* calculate what the structures are and draw them */
  structures = theWindow->structures;
  while(structures) {
    n = structures->numberPoints;

    if (!(StructPoints = (XPoint *)malloc(n*sizeof(XPoint))))
      putchar(7);

    if (Is_X_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].x = mx * log10(max(label_Ptr->X_Scale *
					   structures->x[i],
					   DBL_MIN)) + bx;
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].x = mx * structures->x[i] + bx;
    }	 

    if (Is_Y_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].y = my * log10(max(label_Ptr->Y_Scale * 
					  structures->y[i],
					  DBL_MIN)) + by;
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].y = my * structures->y[i] + by;
    }

    fprintf(fp, "%%Printing the structures\n");
    fprintf(fp, "/Times-Bold findfont 4 scalefont setfont\n");
    fprintf(fp, "newpath\n");
    fprintf(fp, "%d %d moveto\n", StructPoints[0].x, StructPoints[0].y);

    for (i = 1; i < n; i++) {
      fprintf(fp, "%d %d lineto\n", StructPoints[i].x, StructPoints[i].y);
    }

    fprintf(fp, "1 setlinewidth\n");
    if(structures->fillFlag == FILLED) {
      fprintf(fp, "%f %f %f setrgbcolor\n", 
	      RedColor[structures->fillColor],
	      GreenColor[structures->fillColor],
	      BlueColor[structures->fillColor]);
      fprintf(fp, "fill\n");
    }

    XLookupColor(theDisplay, theColormap, theColorNames[structures->lineColor],
		 &theRGBColor, &theHardwareColor);
    fprintf(fp, "%f %f %f setrgbcolor\n", RedColor[structures->lineColor],
	    GreenColor[structures->lineColor],
	    BlueColor[structures->lineColor]);
    fprintf(fp, "1 setlinewidth\nstroke\n\n");

    free(StructPoints);
    structures = structures->next;
  }

  fprintf(fp, ".25 setlinewidth\n\n");

  /* and actually plot some vectors now */
  fprintf(fp, "/Times-Bold findfont 4 scalefont setfont\n");
  fprintf(fp, "0 setgray\n");
  fprintf(fp, "newpath\n");
  for (i = mstart; i <= mend; i++)
    for (j = nstart; j <= nend; j++) {
      fprintf(fp, "%1.0f %1.0f %1.0f %1.0f arw\n", Point[i][j].x,Point[i][j].y,
	      Vector[i][j].x, Vector[i][j].y);
    }
  fprintf(fp, "stroke\n\n");

  /*********************/
  /* Draw the boundary */

  fprintf(fp, "1 setlinewidth\n");
  fprintf(fp, "%d %d %d %d rectangle\n", x1, y1, x2, y2);

  /***************************/
  /* Drawing the tick marks  */

  fprintf(fp, "gsave 0 setgray .8 setlinewidth\n");
  if ((label_Ptr->Y_Min < 0.0 && 0.0 < label_Ptr->Y_Max) ||
      (label_Ptr->Y_Max < 0.0 && 0.0 < label_Ptr->Y_Min)) {
    dtick = (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
    fprintf(fp, "%d %g moveto\n", x1, y2 - dtick * (label_Ptr->Y_Max));
    fprintf(fp, "%d %g lineto\n", x1 + 4, y2 - dtick * (label_Ptr->Y_Max));

    fprintf(fp, "%d %g moveto\n", x2 - 4, y2 - dtick * (label_Ptr->Y_Max));
    fprintf(fp, "%d %g lineto\n", x2, y2 - dtick * (label_Ptr->Y_Max));
  }
  if ((label_Ptr->X_Min < 0.0 && 0.0 < label_Ptr->X_Max) ||
      (label_Ptr->X_Max < 0.0 && 0.0 < label_Ptr->X_Min)) {
    dtick = (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min);
    fprintf(fp, "%g %d moveto\n", x2 - dtick * (label_Ptr->X_Max), y2);
    fprintf(fp, "%g %d lineto\n", x2 - dtick * (label_Ptr->X_Max), y2 - 4);

    fprintf(fp, "%g %d moveto\n", x2 - dtick * (label_Ptr->X_Max), y1);
    fprintf(fp, "%g %d lineto\n", x2 - dtick * (label_Ptr->X_Max), y1 + 4);
  }

  /* and never, never forget to free up your memory */
  for (i = 0; i < m; i++) {
    free(Point[i]);
    free(Vector[i]);
  }
  free(Point);
  free(Vector);

  fprintf(fp, "grestore\n");
  fprintf(fp, "showpage\n\n");

  if (fclose(fp))
    printf("PostScriptOpenWindow(): File was not closed properly!");
}

/****************************************************************/

void PSHeader(const char *filename)
{
  FILE *fp;

  if((fp = fopen(filename,"w"))==NULL) {
      printf("\nError: Postscript file not opened properly.\n");
      return;
  }
  /*****************************/
  /* Loading the dictionary... */

  fprintf(fp, "%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(fp, "%%%%BoundingBox: 72 72 540 730\n");
  fprintf(fp, "%%%%Creator: XGrafix %s Copyright 1991-2010 The Regents of the University of California\n",
			 xgversion);
  fprintf(fp, "%%%%Title: %s\n", filename);
  fprintf(fp, "%%%%Pages: 1 1\n");
  fprintf(fp, "%%%%EndComments-----\n");
  fprintf(fp, "%%%%Begin XGrafix PS Macros\n\n");
  fprintf(fp, "/prtstrfrommidlow {dup stringwidth pop\n");
  fprintf(fp, " 2 div currentpoint pop exch sub currentpoint\n");
  fprintf(fp, " exch pop moveto show} def\n\n");

  fprintf(fp, "/prtstrfrommidup {dup stringwidth currentpoint\n");
  fprintf(fp, " 3 -1 roll sub exch 3 -1 roll 2 div sub exch\n");
  fprintf(fp, " moveto show} def\n\n");

  fprintf(fp, "/prtstrfromritlow {dup stringwidth pop\n");
  fprintf(fp, " currentpoint pop exch sub currentpoint\n");
  fprintf(fp, " exch pop moveto show} def\n\n");

  fprintf(fp, "/prtstrfromritup {dup stringwidth currentpoint\n");
  fprintf(fp, " 3 -1 roll sub exch 3 -1 roll sub exch\n");
  fprintf(fp, " moveto show} def\n\n");

  fprintf(fp, "/prtstrfromlftup {dup stringwidth exch pop\n");
  fprintf(fp, " currentpoint 3 -1 roll sub moveto show} def\n\n");

  fprintf(fp, "/rectangle {4 -1 roll 4 -1 roll\n");
  fprintf(fp, " dup 5 1 roll exch dup 6 1 roll exch\n");
  fprintf(fp, " moveto 3 1 roll dup 5 1 roll exch lineto dup\n");
  fprintf(fp, " 4 -1 roll exch lineto lineto closepath stroke} def\n\n");

  fprintf(fp, "/poly {newpath moveto lineto\n");
  fprintf(fp, " lineto lineto closepath stroke} def\n\n");

  fprintf(fp, "/bbwfpoly {newpath moveto lineto\n");
  fprintf(fp, " lineto lineto closepath gsave 1 setgray\n");
  fprintf(fp, " fill grestore 0 setgray stroke} def\n\n");

  fprintf(fp, "/bbcfpoly {newpath moveto lineto\n");
  fprintf(fp, " lineto lineto closepath gsave fill\n");
  fprintf(fp, " grestore 0 setgray stroke} def\n\n");

  fprintf(fp, "/nbcfpoly {newpath moveto lineto\n");
  fprintf(fp, " lineto lineto closepath fill} def\n\n");

  fprintf(fp, "/cbwfpoly {newpath moveto lineto\n");
  fprintf(fp, " lineto lineto closepath gsave 1\n");
  fprintf(fp, " setgray fill grestore stroke} def\n\n");

  fprintf(fp, "/sp0 {(.) prtstrfrommidlow} def\n");
  fprintf(fp, "/sp1 {(o) prtstrfrommidlow} def\n");
  fprintf(fp, "/sp2 {(*) prtstrfrommidlow} def\n");
  fprintf(fp, "/sp3 {(x) prtstrfrommidlow} def\n");
  fprintf(fp, "/sp4 {(+) prtstrfrommidlow} def\n");
  fprintf(fp, "/sp5 {(#) prtstrfrommidlow} def\n");
  fprintf(fp, "/sp6 {(@) prtstrfrommidlow} def\n\n");
  
  fprintf(fp, "%%Dash line type definitions\n"); 
  fprintf(fp, "/Dash0 {[] 0 setdash} def\n");
  fprintf(fp, "/Dash1 {[2 2] 1 setdash} def\n");
  fprintf(fp, "/Dash2 {[5 5] 2 setdash} def\n");
  fprintf(fp, "/Dash3 {[5 2 2 2] 2 setdash} def\n");
  fprintf(fp, "/Dash4 {[10 5] 5 setdash} def\n");
  fprintf(fp, "/Dash5 {[5 2 2 2 2 2] 2 setdash} def\n\n");

  fprintf(fp, "%%Define how to draw an arrow\n");
  fprintf(fp, "/arw\n");
  fprintf(fp, "{\ngsave\n");
  fprintf(fp, "/y1 exch def\n");
  fprintf(fp, "/x1 exch def\n");
  fprintf(fp, "/y0 exch def\n");
  fprintf(fp, "/x0 exch def\n");
  fprintf(fp, "/dx x1 x0 sub def\n");
  fprintf(fp, "/dy y1 y0 sub def\n");
  fprintf(fp, "0 dx ne 0 dy ne or {\n");
  fprintf(fp, "/angle dy dx atan def\n");
  fprintf(fp, "/length dx dx mul dy dy mul add sqrt .35 mul def\n");
  fprintf(fp, "x0 y0 moveto\n");
  fprintf(fp, "x1 y1 lineto\n");
  fprintf(fp, "stroke\n");
  fprintf(fp, "x1 y1 translate\n");
  fprintf(fp, "angle 70 add rotate\n");
  fprintf(fp, "newpath\n");
  fprintf(fp, "0 0 moveto\n");
  fprintf(fp, "0 length rmoveto\n");
  fprintf(fp, "0 length neg rlineto\n");
  fprintf(fp, "40 rotate\n");
  fprintf(fp, "0 length rlineto\n");
  fprintf(fp, "closepath fill\n");
  fprintf(fp, "stroke\n");
  fprintf(fp, "} {x0 y0 moveto sp0} ifelse grestore\n");
  fprintf(fp, "} def\n\n");
  
  fprintf(fp, "%%Scratch string space\n"); 
  fprintf(fp, "/str 20 string def\n\n"); 
  
  fprintf(fp, "%%Define macro for linear tick marks alog x\n");
  fprintf(fp, "/doxlintickmarks {\n");
  fprintf(fp, " xps2 xps2 xps1 sub xmax xmin sub div xmax mul sub yps1 moveto\n");
  fprintf(fp, " xps2 xps2 xps1 sub xmax xmin sub div xmax mul sub yps1 4 add lineto\n");
  fprintf(fp, " xps2 xps2 xps1 sub xmax xmin sub div xmax mul sub yps2 4 sub moveto\n");
  fprintf(fp, " xps2 xps2 xps1 sub xmax xmin sub div xmax mul sub yps2 lineto\n");
  fprintf(fp, " stroke} def\n\n");

  fprintf(fp, "%%Define macro for log tick marks alog x\n");
  fprintf(fp, "/doxlogtickmarks {\n");
  fprintf(fp, " dup xps2 xps1 sub mul dtklog div xps1 add yps1 moveto\n");
  fprintf(fp, " dup xps2 xps1 sub mul dtklog div xps1 add yps1 4 add lineto\n");
  fprintf(fp, " dup xps2 xps1 sub mul dtklog div xps1 add yps2 4 sub moveto\n");
  fprintf(fp, " xps2 xps1 sub mul dtklog div xps1 add yps2 lineto\n"); 
  fprintf(fp, " stroke} def\n\n");

  fprintf(fp, "%%Define macro for linear tick marks alog y\n");
  fprintf(fp, "/doylintickmarks {\n");
  fprintf(fp, " xps1 yps2 yps2 yps1 sub ymax ymin sub div ymax mul sub moveto\n");
  fprintf(fp, " xps1 4 add yps2 yps2 yps1 sub ymax ymin sub div ymax mul sub lineto\n");
  fprintf(fp, " xps2 4 sub yps2 yps2 yps1 sub ymax ymin sub div ymax mul sub moveto\n");
  fprintf(fp, " xps2 yps2 yps2 yps1 sub ymax ymin sub div ymax mul sub lineto\n");
  fprintf(fp, " stroke} def\n\n");

  fprintf(fp, "%%Define macro for log tick marks alog y\n");
  fprintf(fp, "/doylogtickmarks {\n");
  fprintf(fp, " dup yps2 yps1 sub mul dtklog div yps1 add xps1 exch moveto\n");
  fprintf(fp, " dup yps2 yps1 sub mul dtklog div yps1 add xps1 4 add exch lineto\n");
  fprintf(fp, " dup yps2 yps1 sub mul dtklog div yps1 add xps2 4 sub exch moveto\n");
  fprintf(fp, " yps2 yps1 sub mul dtklog div yps1 add xps2 exch lineto\n"); 
  fprintf(fp, " stroke} def\n\n");

  fprintf(fp, "%%Print the XGrafix logo\n");
  fprintf(fp, "90 rotate\n");
  fprintf(fp, "/Times-BoldItalic findfont 6 scalefont setfont\n");
  fprintf(fp, "770 -590 moveto (XGrafix %s) prtstrfromritlow\n", xgversion);
  fprintf(fp, "-90 rotate\n\n");
  
  fprintf(fp, "%%%%End XGrafix PS Macros\n\n");
  fclose(fp);
}

/****************************************************************/

void PostScript_ThreeD_Window(WindowType theWindow, const char *eps_file_name,
			      const char *plot_title)
{
  DataType        data_Ptr;
  LabelType       label_Ptr;
  MeshType      **theMeshPoint, theBox[8];
  StringType      theString[8];

  double           n1, n2, n3, v1, v2, v3;
  double           u1, u2, u3, up1, up2, up3;
  double           a1, a2, a3, b1, b2, b3;
  double           xprojmax, xprojmin;
  double           yprojmax, yprojmin;
  double           mag, c1, c2, d1, d2;
  double           xtemp, ytemp, ztemp;

  int             m, mstart, mend, n, nend, nstart, colornumber;
  int             i, j, x1, y1, x2, y2;
  int             phi_flag, theta_flag;
  int             xstart, xend, xstep, ystart, yend, ystep;

  FILE           *fp;

  data_Ptr = theWindow->data;
  label_Ptr = theWindow->label;
  if (!Is_Scatter(data_Ptr))
    m = *(data_Ptr->mpoints);
  n = *(data_Ptr->npoints);

  /****************************/
  /* Rescale the window first */

  RescaleThreeDWindow(theWindow,&mstart,&mend,&nstart,&nend);
  
  /******************************************************/
  /* Determining the octant where the viewer is located */

  theWindow->theta = fmod(180. + theWindow->theta, 180.);
  theWindow->phi = fmod(360. + theWindow->phi, 360.);

  if (0 <= theWindow->phi && theWindow->phi <= 90)
    phi_flag = FIRST_QUAD;
  else if (90 < theWindow->phi && theWindow->phi <= 180)
    phi_flag = SECOND_QUAD;
  else if (180 < theWindow->phi && theWindow->phi <= 270)
    phi_flag = THIRD_QUAD;
  else if (270 < theWindow->phi && theWindow->phi <= 360)
    phi_flag = FOURTH_QUAD;

  if (theWindow->theta == 0.0)
    theta_flag = ABOVE;
  else if (0 < theWindow->theta && theWindow->theta <= 90)
    theta_flag = UPPER;
  else
    theta_flag = LOWER;

  /************************************/
  /* Calculating the rotation vectors */

  n1 = sin(theWindow->theta * M_PI / 180.) * cos(theWindow->phi * M_PI / 180.);
  n2 = sin(theWindow->theta * M_PI / 180.) * sin(theWindow->phi * M_PI / 180.);
  n3 = cos(theWindow->theta * M_PI / 180.);

  if (theta_flag == ABOVE) {
    up1 = 0;
    up2 = 1;
    up3 = 0;
  } else {
    up1 = 0;
    up2 = 0;
    up3 = 1;
  }

  u1 = up2 * n3 - up3 * n2;
  u2 = up3 * n1 - up1 * n3;
  u3 = up1 * n2 - up2 * n1;
  mag = sqrt(u1 * u1 + u2 * u2 + u3 * u3);

  u1 /= mag;
  u2 /= mag;
  u3 /= mag;

  v1 = n2 * u3 - n3 * u2;
  v2 = n3 * u1 - n1 * u3;
  v3 = n1 * u2 - n2 * u1;

  /***********************************************************/
  /* Calculating the bounding box and the projection scaling */

  theBox[0].x = 0;
  theBox[1].x = u1;
  theBox[2].x = u1 + u2;
  theBox[3].x = u2;
  theBox[0].y = 0;
  theBox[1].y = v1;
  theBox[2].y = v1 + v2;
  theBox[3].y = v2;

  theBox[4].x = u2 + u3;
  theBox[5].x = u3;
  theBox[6].x = u1 + u3;
  theBox[7].x = u1 + u2 + u3;
  theBox[4].y = v2 + v3;
  theBox[5].y = v3;
  theBox[6].y = v1 + v3;
  theBox[7].y = v1 + v2 + v3;

  xprojmin = xprojmax = theBox[0].x;
  yprojmin = yprojmax = theBox[0].y;
  for (i = 1; i < 8; i++) {
    xprojmin = (xprojmin < theBox[i].x) ? xprojmin : theBox[i].x;
    xprojmax = (xprojmax > theBox[i].x) ? xprojmax : theBox[i].x;
    yprojmin = (yprojmin < theBox[i].y) ? yprojmin : theBox[i].y;
    yprojmax = (yprojmax > theBox[i].y) ? yprojmax : theBox[i].y;
  }

  /********************************************************************/
  /* selecting the labels to print based on the given point of view   */

  if (theta_flag != ABOVE) {
    switch (phi_flag) {
    case FIRST_QUAD:
      if (theta_flag == UPPER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->Y_Min);
	theString[0].x = u1;
	theString[0].y = v1;
	sprintf(theString[1].buffer, "%s", label_Ptr->Y_Label);
	theString[1].x = u1 + .5 * u2;
	theString[1].y = v1 + .5 * v2;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->Y_Max);
	theString[2].x = u1 + u2;
	theString[2].y = v1 + v2;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->X_Max);
	theString[3].x = u1 + u2;
	theString[3].y = v1 + v2;
	sprintf(theString[4].buffer, "%s", label_Ptr->X_Label);
	theString[4].x = .5 * u1 + u2;
	theString[4].y = .5 * v1 + v2;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->X_Min);
	theString[5].x = u2;
	theString[5].y = v2;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = u2;
	theString[6].y = v2;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u2 + u3;
	theString[7].y = v2 + v3;
      } else if (theta_flag == LOWER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->X_Max);
	theString[0].x = u1;
	theString[0].y = v1;
	sprintf(theString[1].buffer, "%s", label_Ptr->X_Label);
	theString[1].x = .5 * u1;
	theString[1].y = .5 * v1;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->X_Min);
	theString[2].x = 0.0;
	theString[2].y = 0.0;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->Y_Min);
	theString[3].x = 0.0;
	theString[3].y = 0.0;
	sprintf(theString[4].buffer, "%s", label_Ptr->Y_Label);
	theString[4].x = .5 * u2;
	theString[4].y = .5 * v2;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->Y_Max);
	theString[5].x = u2;
	theString[5].y = v2;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = u2;
	theString[6].y = v2;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u2 + u3;
	theString[7].y = v2 + v3;
      }
      break;

    case SECOND_QUAD:
      if (theta_flag == UPPER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->X_Max);
	theString[0].x = u1 + u2;
	theString[0].y = v1 + v2;
	sprintf(theString[1].buffer, "%s", label_Ptr->X_Label);
	theString[1].x = .5 * u1 + u2;
	theString[1].y = .5 * v1 + v2;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->X_Min);
	theString[2].x = u2;
	theString[2].y = v2;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->Y_Max);
	theString[3].x = u2;
	theString[3].y = v2;
	sprintf(theString[4].buffer, "%s", label_Ptr->Y_Label);
	theString[4].x = .5 * u2;
	theString[4].y = .5 * v2;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->Y_Min);
	theString[5].x = 0.0;
	theString[5].y = 0.0;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = 0.0;
	theString[6].y = 0.0;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u3;
	theString[7].y = v3;
      } else if (theta_flag == LOWER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->Y_Max);
	theString[0].x = u1 + u2;
	theString[0].y = v1 + v2;
	sprintf(theString[1].buffer, "%s", label_Ptr->Y_Label);
	theString[1].x = u1 + .5 * u2;
	theString[1].y = v1 + .5 * v2;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->Y_Min);
	theString[2].x = u1;
	theString[2].y = v1;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->X_Max);
	theString[3].x = u1;
	theString[3].y = v1;
	sprintf(theString[4].buffer, "%s", label_Ptr->X_Label);
	theString[4].x = .5 * u1;
	theString[4].y = .5 * v1;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->X_Min);
	theString[5].x = 0.0;
	theString[5].y = 0.0;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = 0.0;
	theString[6].y = 0.0;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u3;
	theString[7].y = v3;
      }
      break;

    case THIRD_QUAD:
      if (theta_flag == UPPER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->Y_Max);
	theString[0].x = u2;
	theString[0].y = v2;
	sprintf(theString[1].buffer, "%s", label_Ptr->Y_Label);
	theString[1].x = .5 * u2;
	theString[1].y = .5 * v2;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->Y_Min);
	theString[2].x = 0.0;
	theString[2].y = 0.0;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->X_Min);
	theString[3].x = 0.0;
	theString[3].y = 0.0;
	sprintf(theString[4].buffer, "%s", label_Ptr->X_Label);
	theString[4].x = .5 * u1;
	theString[4].y = .5 * v1;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->X_Max);
	theString[5].x = u1;
	theString[5].y = v1;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = u1;
	theString[6].y = v1;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u1 + u3;
	theString[7].y = v1 + v3;
      } else if (theta_flag == LOWER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->X_Min);
	theString[0].x = u2;
	theString[0].y = v2;
	sprintf(theString[1].buffer, "%s", label_Ptr->X_Label);
	theString[1].x = .5 * u1 + u2;
	theString[1].y = .5 * v1 + v2;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->X_Max);
	theString[2].x = u1 + u2;
	theString[2].y = v1 + v2;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->Y_Max);
	theString[3].x = u1 + u2;
	theString[3].y = v1 + v2;
	sprintf(theString[4].buffer, "%s", label_Ptr->Y_Label);
	theString[4].x = u1 + .5 * u2;
	theString[4].y = v1 + .5 * v2;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->Y_Min);
	theString[5].x = u1;
	theString[5].y = v1;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = u1;
	theString[6].y = v1;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u1 + u3;
	theString[7].y = v1 + v3;
      }
      break;

    case FOURTH_QUAD:
      if (theta_flag == UPPER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->X_Min);
	theString[0].x = 0.0;
	theString[0].y = 0.0;
	sprintf(theString[1].buffer, "%s", label_Ptr->X_Label);
	theString[1].x = .5 * u1;
	theString[1].y = .5 * v1;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->X_Max);
	theString[2].x = u1;
	theString[2].y = v1;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->Y_Min);
	theString[3].x = u1;
	theString[3].y = v1;
	sprintf(theString[4].buffer, "%s", label_Ptr->Y_Label);
	theString[4].x = u1 + .5 * u2;
	theString[4].y = v1 + .5 * v2;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->Y_Max);
	theString[5].x = u1 + u2;
	theString[5].y = v1 + v2;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = u1 + u2;
	theString[6].y = v1 + v2;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u1 + u2 + u3;
	theString[7].y = v1 + v2 + v3;
      } else if (theta_flag == LOWER) {
	sprintf(theString[0].buffer, "%.4G", label_Ptr->Y_Min);
	theString[0].x = 0.0;
	theString[0].y = 0.0;
	sprintf(theString[1].buffer, "%s", label_Ptr->Y_Label);
	theString[1].x = .5 * u2;
	theString[1].y = .5 * v2;
	sprintf(theString[2].buffer, "%.4G", label_Ptr->Y_Max);
	theString[2].x = u2;
	theString[2].y = v2;
	sprintf(theString[3].buffer, "%.4G", label_Ptr->X_Min);
	theString[3].x = u2;
	theString[3].y = v2;
	sprintf(theString[4].buffer, "%s", label_Ptr->X_Label);
	theString[4].x = .5 * u1 + u2;
	theString[4].y = .5 * v1 + v2;
	sprintf(theString[5].buffer, "%.4G", label_Ptr->X_Max);
	theString[5].x = u1 + u2;
	theString[5].y = v1 + v2;
	sprintf(theString[6].buffer, "%.4G", label_Ptr->Z_Min);
	theString[6].x = u1 + u2;
	theString[6].y = v1 + v2;
	sprintf(theString[7].buffer, "%.4G", label_Ptr->Z_Max);
	theString[7].x = u1 + u2 + u3;
	theString[7].y = v1 + v2 + v3;
      }
      break;
    }
  }
  /**************************************************/

  if (label_Ptr->Z_Max == label_Ptr->Z_Min || label_Ptr->Y_Max == label_Ptr->Y_Min ||
      label_Ptr->X_Max == label_Ptr->X_Min)
    return;

  /****************************************************/
  /* Initialize the PS file and load the dictionary  */

  if (!(fp = fopen(eps_file_name, "a"))) {
    printf("PostScriptOpenWindow(): File was not opened properly!");
    return;
  }

  /************************************************/
  /* Rotate, translate, and draw the boundaries */

  fprintf(fp, "\n1 setlinejoin\n");
  fprintf(fp, "612 0 translate\n");
  fprintf(fp, "90 rotate\n");
  fprintf(fp, "1 setlinewidth\n");
  fprintf(fp, "0 setgray\n");

  x1 = 132;
  x2 = 684;
  y1 = 108;
  y2 = 504;

  fprintf(fp, "%%Limits of the bounding box (PS and physical)\n"); 
  fprintf(fp, "/xps1 %d def\n",x1); 
  fprintf(fp, "/yps1 %d def\n",y1); 
  fprintf(fp, "/xps2 %d def\n",x2);
  fprintf(fp, "/yps2 %d def\n",y2);

  c1 = (x2 - x1) / (xprojmax - xprojmin + DBL_MIN);
  d1 = (xprojmax * x1 - xprojmin * x2) / (xprojmax - xprojmin + DBL_MIN) + 0.5;
  c2 = (y2 - y1) / (yprojmax - yprojmin + DBL_MIN);
  d2 = (yprojmax * y1 - yprojmin * y2) / (yprojmax - yprojmin + DBL_MIN) + 0.5;

  if (theta_flag != ABOVE) {
    switch (phi_flag) {
    case FIRST_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 0, BACK, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 5, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 1, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 4, BACK, fp);
      break;

    case SECOND_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 0, BACK, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 5, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 1, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 2, BACK, fp);
      break;

    case THIRD_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 0, BACK, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 5, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 2, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 3, BACK, fp);
      break;

    case FOURTH_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 0, BACK, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 5, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 3, BACK, fp);
      PaintWall(theBox, c1, d1, c2, d2, 4, BACK, fp);
      break;
    }
  }
  /*******************************/
  /* painting the labels         */

  if (strcmp(plot_title, label_Ptr->Z_Label)){ /* print title only if different */
    fprintf(fp, "\n/Times-BoldItalic findfont 14 scalefont setfont\n");
    fprintf(fp, "xps1 xps2 add 2 div yps2 18 add moveto\n");
    fprintf(fp, "(%s) prtstrfrommidlow\n", plot_title);
  }

  fprintf(fp, "\n/Times-BoldItalic findfont 12 scalefont setfont\n");
  fprintf(fp, "xps1 xps2 add 2 div yps2 8 add moveto\n");
  fprintf(fp, "(%s) prtstrfrommidlow\n", label_Ptr->Z_Label);

  if (theta_flag == ABOVE) {
    fprintf(fp, "90 rotate\n");
    fprintf(fp, "yps1 yps2 add 2 div -100 moveto\n");
    fprintf(fp, "(%s) prtstrfrommidlow\n", label_Ptr->Y_Label);
    fprintf(fp, "-90 rotate\n");

    fprintf(fp, "xps1 xps2 add 2 div yps1 16 sub moveto\n");
    fprintf(fp, "(%s) prtstrfrommidup\n", label_Ptr->X_Label);

    fprintf(fp, "/Times-Bold findfont 12 scalefont setfont\n");
    fprintf(fp, "xps1 yps1 16 sub moveto\n");
    fprintf(fp, "(%.3G) prtstrfromritup\n", label_Ptr->X_Min);
    fprintf(fp, "xps2 yps1 16 sub moveto\n");
    fprintf(fp, "(%.3G) prtstrfromritup\n", label_Ptr->X_Max);

    fprintf(fp, "xps1 8 sub yps2 8 sub moveto\n");
    fprintf(fp, "(%.3G) prtstrfromritup\n", label_Ptr->Y_Max);
    fprintf(fp, "xps1 8 sub yps1 moveto\n");
    fprintf(fp, "(%.3G) prtstrfromritlow\n", label_Ptr->Y_Min);
    fprintf(fp, "xps1 yps1 xps2 yps2 rectangle\n");
  } else {
    fprintf(fp, "%d %d moveto\n", (int) (c1 * theString[1].x + d1 - 15), (int) (c2 * theString[1].y + d2 - 15));
    fprintf(fp, "(%s) prtstrfromritup\n", theString[1].buffer);

    fprintf(fp, "%d %d moveto\n", (int) (c1 * theString[4].x + d1 + 15), (int) (c2 * theString[4].y + d2 - 15));
    fprintf(fp, "(%s) prtstrfromlftup\n", theString[4].buffer);

    fprintf(fp, "/Times-Bold findfont 12 scalefont setfont\n");
    fprintf(fp, "%d %d moveto\n", x1, (int) (c2 * theString[0].y + d2 - 15));
    fprintf(fp, "(%s) prtstrfromritup\n", theString[0].buffer);

    fprintf(fp, "%d %d moveto\n", (int) (c1 * theString[2].x + d1 - 15), y1 - 15);
    fprintf(fp, "(%s) prtstrfromritup\n", theString[2].buffer);

    fprintf(fp, "%d %d moveto\n", (int) (c1 * theString[3].x + d1 + 15), y1 - 15);
    fprintf(fp, "(%s) prtstrfromlftup\n", theString[3].buffer);

    fprintf(fp, "%d %d moveto\n", x2, (int) (c2 * theString[5].y + d2 - 15));
    fprintf(fp, "(%s) prtstrfromlftup\n", theString[5].buffer);

    fprintf(fp, "%d %d moveto\n", x2 + 5, (int) (c2 * theString[6].y + d2 + 5));
    fprintf(fp, "(%s) show\n", theString[6].buffer);

    fprintf(fp, "%d %d moveto\n", x2 + 5, (int) (c2 * theString[7].y + d2));
    fprintf(fp, "(%s) prtstrfromlftup\n", theString[7].buffer);
  }

  /***********************************************/
  /* Used for scaling the arrays before rotating */

  if (Is_X_Log(theWindow)) {
    a1 = 1.0 / log10(max(label_Ptr->X_Max/(label_Ptr->X_Min+DBL_MIN),DBL_MIN));
    b1 = log10(max(label_Ptr->X_Min / (label_Ptr->X_Scale+DBL_MIN),DBL_MIN)) / 
	 log10(max(label_Ptr->X_Max / (label_Ptr->X_Min+DBL_MIN),DBL_MIN));
  } else {
    a1 = label_Ptr->X_Scale / (label_Ptr->X_Max - label_Ptr->X_Min+DBL_MIN);
    b1 = label_Ptr->X_Min / (label_Ptr->X_Max - label_Ptr->X_Min+DBL_MIN);
  }
  if (Is_Y_Log(theWindow)) {
    a2 = 1.0 / log10(max(label_Ptr->Y_Max/(label_Ptr->Y_Min+DBL_MIN),DBL_MIN));
    b2 = log10(max(label_Ptr->Y_Min / (label_Ptr->Y_Scale+DBL_MIN),DBL_MIN)) / 
	 log10(max(label_Ptr->Y_Max / (label_Ptr->Y_Min+DBL_MIN),DBL_MIN));
  } else {
    a2 = label_Ptr->Y_Scale / (label_Ptr->Y_Max - label_Ptr->Y_Min+DBL_MIN);
    b2 = label_Ptr->Y_Min / (label_Ptr->Y_Max - label_Ptr->Y_Min+DBL_MIN);
  }
  if (Is_Z_Log(theWindow)) {
    a3 = 1.0 / log10(max(label_Ptr->Z_Max/(label_Ptr->Z_Min+DBL_MIN),DBL_MIN));
    b3 = log10(max(label_Ptr->Z_Min / (label_Ptr->Z_Scale+DBL_MIN),DBL_MIN)) / 
	 log10(max(label_Ptr->Z_Max / (label_Ptr->Z_Min+DBL_MIN),DBL_MIN));
  } else {
    a3 = label_Ptr->Z_Scale / (label_Ptr->Z_Max - label_Ptr->Z_Min+DBL_MIN);
    b3 = label_Ptr->Z_Min / (label_Ptr->Z_Max - label_Ptr->Z_Min+DBL_MIN);
  }

  /********************************************/
  /* do the actual 3d drawing onto the pixmap */

  if (Is_Scatter(data_Ptr)) {
    fprintf(fp, "0 setgray\n");
    fprintf(fp, "/Times-Bold findfont 6 scalefont setfont\n");
    fprintf(fp, "newpath\n");
    for (i = 0; i < n; i++) {
      if (Is_X_Log(theWindow))
		  xtemp = a1 * log10(max(ScatterX(data_Ptr,i),DBL_MIN)) - b1;
      else
		  xtemp = a1 * ScatterX(data_Ptr,i) - b1;
      if (Is_Y_Log(theWindow))
		  ytemp = a2 * log10(max(ScatterY(data_Ptr,i),DBL_MIN)) - b2;
      else
		  ytemp = a2 * ScatterY(data_Ptr,i) - b2;
      if (Is_Z_Log(theWindow))
		  ztemp = a3 * log10(max(ScatterZ(data_Ptr,i),DBL_MIN)) - b3;
      else
		  ztemp = a3 * ScatterZ(data_Ptr,i) - b3;
      if (0.0 < xtemp && xtemp < 1.0 && 0.0 < ytemp && ytemp < 1.0 && 
	  0.0 < ztemp && ztemp < 1.0)
	fprintf(fp, "%g %g moveto sp0\n", c1 * (u1 * xtemp + u2 * ytemp + 
						u3 * ztemp) + d1,
		c2 * (v1 * xtemp + v2 * ytemp + v3 * ztemp) + d2);
    }
    fprintf(fp, "stroke\n\n");
  } else {
    theMeshPoint = (MeshType **) malloc(m * sizeof(MeshType *));
    for (i = 0; i < m; i++) {
      theMeshPoint[i] = (MeshType *) malloc(n * sizeof(MeshType));

      if (Is_Irr_Surf(data_Ptr)) { 
	for (j = 0; j < n; j++) {
	  if (Is_X_Log(theWindow))
	    xtemp = a1 * log10(fabs(IrregularX(data_Ptr,i,j))+DBL_MIN) - b1;
	  else
	    xtemp = a1 * IrregularX(data_Ptr,i,j) - b1;
	  if (Is_Y_Log(theWindow))
	    ytemp = a2 * log10(fabs(IrregularY(data_Ptr,i,j))+DBL_MIN) - b2;
	  else
	    ytemp = a2 * IrregularY(data_Ptr,i,j) - b2;
	  if (Is_Z_Log(theWindow))
	    ztemp = a3 * log10(fabs(IrregularZ(data_Ptr,i,j))+DBL_MIN) - b3;
	  else
	    ztemp = a3 * IrregularZ(data_Ptr,i,j) - b3;
	  if (ztemp > 1.0)
	    ztemp = 1.0;
	  else if (ztemp < 0.0)
	    ztemp = 0.0;
	  theMeshPoint[i][j].x = u1 * xtemp + u2 * ytemp + u3 * ztemp;
	  theMeshPoint[i][j].y = v1 * xtemp + v2 * ytemp + v3 * ztemp;
	}
      } else {
	for (j = 0; j < n; j++) {
	  if (Is_X_Log(theWindow))
	    xtemp = a1 * log10(max(SurfaceX(data_Ptr,i),DBL_MIN)) - b1;
	  else
	    xtemp = a1 * SurfaceX(data_Ptr,i) - b1;
	  if (Is_Y_Log(theWindow))
	    ytemp = a2 * log10(max(SurfaceY(data_Ptr,j),DBL_MIN)) - b2;
	  else
	    ytemp = a2 * SurfaceY(data_Ptr,j) - b2;
	  if (Is_Z_Log(theWindow))
	    ztemp = a3 * log10(max(SurfaceZ(data_Ptr,i,j),DBL_MIN)) - b3;
	  else
	    ztemp = a3 * SurfaceZ(data_Ptr,i,j) - b3;
	  if (ztemp > 1.0)
	    ztemp = 1.0;
	  else if (ztemp < 0.0)
	    ztemp = 0.0;
	  theMeshPoint[i][j].x = u1 * xtemp + u2 * ytemp + u3 * ztemp;
	  theMeshPoint[i][j].y = v1 * xtemp + v2 * ytemp + v3 * ztemp;
	}
      }
    }
    /*********************************/

    switch (phi_flag) {
    case FIRST_QUAD:
      xstart = mstart;
      xend = mend - 1;
      xstep = 1;
      ystart = nstart;
      yend = nend - 1;
      ystep = 1;
      break;

    case SECOND_QUAD:
      xstart = mend - 2;
      xend = mstart - 1;
      xstep = -1;
      ystart = nstart;
      yend = nend - 1;
      ystep = 1;
      break;

    case THIRD_QUAD:
      xstart = mend - 2;
      xend = mstart - 1;
      xstep = -1;
      ystart = nend - 2;
      yend = nstart - 1;
      ystep = -1;
      break;

    case FOURTH_QUAD:
      xstart = mstart;
      xend = mend - 1;
      xstep = 1;
      ystart = nend - 2;
      yend = nstart - 1;
      ystep = -1;
      break;
    }

    if ((!Is_Shading_On(theWindow) && Is_Grid_On(theWindow)) ||
	(Is_Shading_On(theWindow) && Is_ColorCode_On(theWindow)) ||
	(Is_Shading_On(theWindow) && Is_Grid_On(theWindow)
	 && !Is_ColorCode_On(theWindow))) {
      fprintf(fp, "\n.4 setlinewidth\n");
      for (i = xstart; i != xend; i += xstep)
	for (j = ystart; j != yend; j += ystep) {
	  if (Is_ColorCode_On(theWindow)) {
	    if (Is_Z_Log(theWindow))
	      colornumber = MAXTHREEDCOLORS * 
		(.25 * a3 * log10(fabs(SurfaceZ(data_Ptr,i,j) * 
				       SurfaceZ(data_Ptr,i + 1,j) *
				       SurfaceZ(data_Ptr,i + 1,j + 1) * 
				       SurfaceZ(data_Ptr,i,j + 1)) + 
				  DBL_MIN) - b3);
	    else
	      colornumber = MAXTHREEDCOLORS * 
		(.25 * a3 * (SurfaceZ(data_Ptr,i,j) + SurfaceZ(data_Ptr,i+1,j)+
			     SurfaceZ(data_Ptr,i + 1,j + 1) + 
			     SurfaceZ(data_Ptr,i,j + 1)) - b3);
	    
	    if (colornumber >= MAXTHREEDCOLORS)
	      colornumber = MAXTHREEDCOLORS - 1;
	    else if (colornumber < 0)
	      colornumber = 0;
	    fprintf(fp, "%s setrgbcolor\n", ThreeDPSColor[colornumber]);
	  }
	  fprintf(fp, "%d %d %d %d %d %d %d %d",
		  (int) (c1 * theMeshPoint[i][j].x + d1), (int) (c2 * theMeshPoint[i][j].y + d2),
		  (int) (c1 * theMeshPoint[i + 1][j].x + d1), (int) (c2 * theMeshPoint[i + 1][j].y + d2),
		  (int) (c1 * theMeshPoint[i + 1][j + 1].x + d1), (int) (c2 * theMeshPoint[i + 1][j + 1].y + d2),
		  (int) (c1 * theMeshPoint[i][j + 1].x + d1), (int) (c2 * theMeshPoint[i][j + 1].y + d2));
	  
	  if (Is_Grid_On(theWindow) && !Is_ColorCode_On(theWindow))
	    fprintf(fp, " bbwfpoly\n");
	  else if (!Is_Shading_On(theWindow) && Is_ColorCode_On(theWindow)
		   && Is_Grid_On(theWindow))
	    fprintf(fp, " cbwfpoly\n");
	  else if (Is_Shading_On(theWindow) && Is_ColorCode_On(theWindow)
		   && !Is_Grid_On(theWindow))
	    fprintf(fp, " nbcfpoly\n");
	  else if (Is_Shading_On(theWindow) && Is_ColorCode_On(theWindow)
		   && Is_Grid_On(theWindow))
	    fprintf(fp, " bbcfpoly\n");
	}
    }
    /*********************************************************/
    /* Free up the memory for the memory for the mesh points */
    
    for (i = 0; i < m; i++)
      free(theMeshPoint[i]);
    free(theMeshPoint);
  }
  /***********************/
  /* Draw the front walls */

  fprintf(fp, "\n1 setlinewidth\n");
  fprintf(fp, "0 setgray\n");

  if (theta_flag != ABOVE) {
    switch (phi_flag) {
    case FIRST_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 5, FRONT, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 0, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 2, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 3, FRONT, fp);
      break;
      
    case SECOND_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 5, FRONT, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 0, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 3, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 4, FRONT, fp);
      break;
      
    case THIRD_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 5, FRONT, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 0, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 1, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 4, FRONT, fp);
      break;

    case FOURTH_QUAD:
      if (theta_flag == UPPER)
	PaintWall(theBox, c1, d1, c2, d2, 5, FRONT, fp);
      else
	PaintWall(theBox, c1, d1, c2, d2, 0, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 1, FRONT, fp);
      PaintWall(theBox, c1, d1, c2, d2, 2, FRONT, fp);
      break;
    }
  }
  /*********************************************************/
  /*  fprintf(fp, "\ngrestore\n"); no matching gsave! */
  fprintf(fp, "\nshowpage\n");
  if (fclose(fp))
    printf("PostScriptOpenWindow(): File was not closed properly!");
}

/****************************************************************/

/****************************************************************/

void Ascii_TwoD_Window(WindowType theWindow, const char *filename, char type)
{
  FILE *fp;
  DataType  theData = theWindow->data;
  LabelType theLabel= theWindow->label;
  int i,n;
  double xscale, yscale;
  double xoffset, yoffset;

  xscale = theLabel->X_Scale;
  yscale = theLabel->Y_Scale;
  xoffset = theLabel->X_Offset;
  yoffset = theLabel->Y_Offset;

  if (type == 'a') {
    fp = fopen(filename,"a");
  } else {
    fp = fopen(filename,"w");
  }
  
  if(fp == NULL) {
    printf("Ascii_TwoD_Window couldn't create file\n");
    return;
  }
  
  while(theData) {
    
    n = *(theData->npoints);
    
    fprintf(fp,"\n##### time= %g \n", *theTimeStep);
    
    for(i=0;i<n;i++)
      fprintf(fp,"%.6G\t\t%.6G\n", xscale*CurveX(theData,i)+xoffset,
	      yscale*CurveY(theData,i)+yoffset);
    
    theData = theData->next;
  }
  
  fclose(fp);
}

/****************************************************************/

void Ascii_ThreeD_Window(WindowType theWindow, const char *filename, char type)
{ 
  FILE *fp;
  DataType theData = theWindow->data;
  LabelType theLabel= theWindow->label;
  int i,j,m,n;
  double xscale, yscale, zscale;

  xscale = theLabel->X_Scale;
  yscale = theLabel->Y_Scale;
  zscale = theLabel->Z_Scale;
  
  if (type == 'a') {
    fp = fopen(filename,"a");
  } else {
    fp = fopen(filename,"w");
  }

  if(fp == NULL) {
    printf("Ascii_ThreeD_Window couldn't create file\n");
    return;
  }

  fprintf(fp,"\n##### time= %g \n", *theTimeStep);
    
  n = *(theData->npoints);
  
  if (Is_Scatter(theData)) {
    for(i=0;i<n;i++) {
      fprintf(fp,"%.6G\t\t%.6G\t\t%.6G\n", xscale*(ScatterX(theData,i)),
	      yscale*(ScatterY(theData,i)), zscale*(ScatterZ(theData,i)));
    }
  } else {
    m = *(theData->mpoints);
    
    if (Is_Irr_Surf(theData)) {
      for (i=0;i<m;i++) {
	for (j=0;j<n;j++) {
	  fprintf(fp,"%.6G\t\t%.6G\t\t%.6G\n", xscale*(IrregularX(theData,i,j)),
		  yscale*(IrregularY(theData,i,j)), zscale*(IrregularZ(theData,i,j)));
	}
      }
    } else {
      for (i=0;i<m;i++) {
	for (j=0;j<n;j++) {
	  fprintf(fp,"%.6G\t\t%.6G\t\t%.6G\n", xscale*(SurfaceX(theData,i)),
		  yscale*(SurfaceY(theData,j)), zscale*(SurfaceZ(theData,i,j)));
	}
      }
    }
  }

  fclose(fp);
}

/****************************************************************/

void Ascii_Vector_Window(WindowType theWindow, const char *filename, char type)
{
  FILE *fp;
  DataType theData = theWindow->data;
  int i,j,m,n;

  if (type == 'a') {
    fp = fopen(filename,"a");
  } else {
    fp = fopen(filename,"w");
  }

  if(fp == NULL) {
    printf("Ascii_Vector_Window couldn't create file\n");
    return;
  }

  fprintf(fp,"\n####################\n");

  m = *(theData->mpoints);
  n = *(theData->npoints);

  for (i=0;i<m;i++) {
    for (j=0;j<n;j++) {
      fprintf(fp,"%.6G\t%.6G\t%.6G\t%.6G\n",VectorX(theData,i),
	      VectorY(theData,j),VectorW(theData,i,j),
	      VectorZ(theData,i,j));
    }
  }
  
  fclose(fp);
}


void Bin_TwoD_Window(WindowType theWindow, const char *filename, char type) {

  BXG_Write_2D(theWindow->data,theWindow->label,theWindow->structures, filename, type, theTimeStep);
}

void Bin_ThreeD_Window(WindowType theWindow, const char *filename, char type) {

  BXG_Write_3D(theWindow->data,theWindow->label,filename,type, theTimeStep);
}

void Bin_Vector_Window(WindowType theWindow, const char *filename, char type) {
  
  BXG_Write_Vector(theWindow->data,theWindow->label,theWindow->structures,filename,type,theTimeStep);
}
