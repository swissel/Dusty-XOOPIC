#include "xgrafixint.h"

#define Vlength(x,y) (sqrt((x)*(x)+(y)*(y)))

void Paint_Vector_Window(WindowType theWindow)
{
  DataType        data_Ptr;
  LabelType       label_Ptr;
  char            buffer1[20], buffer2[20];
  XPoint          Tick_Marks[2], points[7], *StructPoints;
  XRectangle      theClientArea;
  Tk_Window       tkwin;
  StructType     *structures;

  register double maxvlen, mx, bx, my, by, w1, w2, X0, Y0, mx2, my2;
  register double X1, Y1, X2, Y2, PX1, PY1, PX2, PY2;
  int    n, k1, k2, maxlen, mstart, mend, nstart, nend;
  register int    i, j, x1, y1, x2, y2;

  tkwin = theWindow->tkwin;

  data_Ptr = theWindow->data;
  label_Ptr = theWindow->label;

  RescaleVectorDWindow(theWindow,&mstart,&mend,&nstart,&nend);
  /* printf("%d %d %d %d\n",mstart,mend,nstart,nend); */

  /****************************/
  /* Find  longest vector in the system */

  maxvlen = Vlength(VectorZ(data_Ptr,mstart,nstart), 
		    VectorW(data_Ptr,mstart,nstart));
  for (i = mstart; i <= mend; i++)
    for (j = nstart; j <= nend; j++)
      maxvlen = max(maxvlen, Vlength(VectorZ(data_Ptr,i,j), 
				     VectorW(data_Ptr,i,j)));
   if(maxvlen == 0.0) return;

  /* printf("%f\n",maxvlen); */

  /********************************************************************/

  if (!Was_Open_Before(theWindow) || Is_Y_AutoRescale(theWindow)) {
    XSetForeground(theDisplay, theWindow->xwingc, GrayShade[3]);
    XFillRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		   0, 0, Tk_Width(tkwin), Tk_Height(tkwin));
  }

  if (!Was_Open_Before(theWindow) || Is_Y_AutoRescale(theWindow) ||
      Is_Trace_On(theWindow)) {
    sprintf(buffer1, "%.3G", label_Ptr->Y_Max);
    sprintf(buffer2, "%.3G", label_Ptr->Y_Min);

    i = strlen(buffer1);
    j = strlen(buffer2);

    k1 = XTextWidth(theFontStruct, buffer1, i);
    k2 = XTextWidth(theFontStruct, buffer2, j);
    maxlen = (k1 > k2) ? k1 : k2;

    x1 = maxlen + 3;
    x2 = Tk_Width(tkwin) - 3;
    y1 = 3;
    y2 = Tk_Height(tkwin) - (theFontAscent + theFontDescent + 7); 

    XSetForeground(theDisplay, theWindow->xwingc, theWinTextColor);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		maxlen - k1 + 1, y1 + theFontAscent, buffer1, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		maxlen - k2 + 1, y2 - 2, buffer2, j);

    /* Draw Tick Marks */
    XSetForeground(theDisplay, theWindow->xwingc, theTickMarkColor);

    if ((label_Ptr->Y_Min < 0.0 && 0.0 < label_Ptr->Y_Max) ||
	(label_Ptr->Y_Max < 0.0 && 0.0 < label_Ptr->Y_Min)) {
      Tick_Marks[0].x = x1 - 3;
      Tick_Marks[0].y = y1 + label_Ptr->Y_Max * (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
      Tick_Marks[1].x = x1 - 1;
      Tick_Marks[1].y = y1 + label_Ptr->Y_Max * (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
      XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 Tick_Marks, 2, CoordModeOrigin);
    }
    /* Draw Red Border around Client Area */
    XSetForeground(theDisplay, theWindow->xwingc, theBorderColor);
    XDrawRectangle(theDisplay, theWindow->pixmap_buffer,
	       theWindow->xwingc, x1 - 1, y1 - 1, x2 - x1 + 2, y2 - y1 + 2);

    theWindow->ulxc = x1;
    theWindow->ulyc = y1;
    theWindow->lrxc = x2;
    theWindow->lryc = y2;
  }
  x1 = theWindow->ulxc;
  y1 = theWindow->ulyc;
  x2 = theWindow->lrxc;
  y2 = theWindow->lryc;

  if (!Was_Open_Before(theWindow) || Is_Y_AutoRescale(theWindow) ||
      Is_X_AutoRescale(theWindow) || Is_Trace_On(theWindow)) {
    if ((label_Ptr->X_Min < 0.0 && 0.0 < label_Ptr->X_Max) ||
	(label_Ptr->X_Max < 0.0 && 0.0 < label_Ptr->X_Min)) {
      Tick_Marks[0].x = x2 - label_Ptr->X_Max * (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min);
      Tick_Marks[0].y = y2 + 3;
      Tick_Marks[1].x = x2 - label_Ptr->X_Max * (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min);
      Tick_Marks[1].y = y2 + 5;
      XSetForeground(theDisplay, theWindow->xwingc, theTickMarkColor);
      XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 Tick_Marks, 2, CoordModeOrigin);
    }
    XSetForeground(theDisplay, theWindow->xwingc, GrayShade[3]);
    XFillRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		   0, y2 + 6, x2, Tk_Height(tkwin));

    sprintf(buffer1, "%g", label_Ptr->X_Max);
    sprintf(buffer2, "%g", label_Ptr->X_Min);

    i = strlen(buffer1);
    j = strlen(buffer2);

    maxlen = XTextWidth(theFontStruct, buffer1, i);

    XSetForeground(theDisplay, theWindow->xwingc, theWinTextColor);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, x2 - maxlen - 1,
		y2 + theFontAscent + 6, buffer1, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, x1,
		y2 + theFontAscent + 6, buffer2, j);

    i = strlen(label_Ptr->X_Label);
    maxlen = XTextWidth(theFontStruct, label_Ptr->X_Label, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		(x1 + x2 - maxlen) / 2,
		y2 + theFontAscent + 6, label_Ptr->X_Label, i);
  }
/*  printf("%d %d %d %d\n",x1,y1,x2,y2); */

  theClientArea.x = x1 + 1;
  theClientArea.y = y1 + 1;
  theClientArea.width = x2 - x1;
  theClientArea.height = y2 - y1;
  XSetClipRectangles(theDisplay, theWindow->xwingc, 0, 0, &theClientArea, 1, Unsorted);

  /* Clear client area. */

  if (!Was_Open_Before(theWindow) || Is_X_AutoRescale(theWindow) ||
      Is_Y_AutoRescale(theWindow) || !Is_Trace_On(theWindow)) {
    XSetForeground(theDisplay, theWindow->xwingc, theBKGDColor);
    XFillRectangle(theDisplay, theWindow->pixmap_buffer,
		   theWindow->xwingc, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    Open_Before(theWindow);
  }

  /*******************/
  /* Draw structures */

  theWindow->c1 = mx = (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) * label_Ptr->X_Scale;
  mx2 = (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN);
  theWindow->d1 = bx = (label_Ptr->X_Max * x1 - label_Ptr->X_Min * x2)
    / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) + 0.5;
  theWindow->c2 = my = (y1 - y2) / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) * label_Ptr->Y_Scale;
  my2 = (y1 - y2) / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN);
  theWindow->d2 = by = (label_Ptr->Y_Max * y2 - label_Ptr->Y_Min * y1)
    / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) + 0.5;

  structures = theWindow->structures;
  while(structures) {
    n = structures->numberPoints;
    
    if (!(StructPoints = (XPoint *) malloc(n * sizeof(XPoint))))
      putchar(7);
	 
    if (Is_X_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].x = 
	  ToShort(mx2 * log10(max(structures->x[i], DBL_MIN)) + bx);
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].x = ToShort(mx2 * structures->x[i] + bx);
    }	 

    if (Is_Y_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].y = 
	  ToShort(my2 * log10(max(structures->y[i], DBL_MIN)) + by);
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].y = ToShort(my2 * structures->y[i] + by);
    }

    if (structures->fillFlag == FILLED) {
      XSetForeground(theDisplay, theWindow->xwingc, 
		     Color[structures->fillColor]);
      XFillPolygon(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		   StructPoints, n, Complex, CoordModeOrigin);
    }

    XSetForeground(theDisplay, theWindow->xwingc, 
		   Color[structures->lineColor]);
    XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, 
	       StructPoints, n, CoordModeOrigin);
	 
    free(StructPoints);
    structures = structures->next;
  }

  /********************************************************************/
  /* Plot Vectors                                                    */

  /*
   * if maxw or maxz ==0 then it means that all of the vectors are zero.
   * Seems kind of silly to have to check for this at first, but stranger
   * things have happened...
   */
  if (maxvlen != 0.0) {
    if (mend - mstart + 1 != 0.0)
      w1 = (x2 - x1) / (mend - mstart + 1) / maxvlen;
    else 
      w1 = 0.0;
    
    if (nend - nstart + 1 != 0.0)
      w2 = (y2 - y1) / (nend - nstart + 1) / maxvlen;
    else
      w2 = 0.0;
  }
  else { w1 = w2 = 0.0; }

  XSetForeground(theDisplay, theWindow->xwingc, Color[data_Ptr->color]);
  for (i = mstart; i <= mend; i++)
    for (j = nstart; j <= nend; j++) {
      X0 = mx * VectorX(data_Ptr,i) + bx;
      Y0 = my * VectorY(data_Ptr,j) + by;
      X1 = X0 + (X2 = VectorW(data_Ptr,i,j) * w1);
      Y1 = Y0 - (Y2 = VectorZ(data_Ptr,i,j) * w2);
      /*
       * printf("w=%1.1e z=%1.1e x2=%2.0f y2=%2.0f\n",data_Ptr->w[i][j],
       * data_Ptr->z[i][j],X2,Y2);
       */
      
      /*
	 PX1 = -.3464 * X2 - .2 * Y2 + X1; 
	 PY1 = -.3464 * Y2 + .2 * X2 + Y1;
	 PX2 = -.3464 * X2 + .2 * Y2 + X1; 
	 PY2 = -.3464 * Y2 - .2 * X2 + Y1;
	 */
      
      /* 10.12.93 PM, Fixed problem with vectors being plotted upside down */
      PX1 = -.3464 * X2 - .2 * Y2 + X1;
      PY1 =  .3464 * Y2 - .2 * X2 + Y1;
      PX2 = -.3464 * X2 + .2 * Y2 + X1;
      PY2 =  .3464 * Y2 + .2 * X2 + Y1;

      points[0].x = ToShort(X0);
      points[0].y = ToShort(Y0);
      points[1].x = ToShort(X1);
      points[1].y = ToShort(Y1);
      points[2].x = ToShort(PX1);
      points[2].y = ToShort(PY1);
      points[3].x = ToShort(X1);
      points[3].y = ToShort(Y1);
      points[4].x = ToShort(PX2);
      points[4].y = ToShort(PY2);

      XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 points, 5, CoordModeOrigin);
    }

  XSetClipMask(theDisplay, theWindow->xwingc, None);
}
