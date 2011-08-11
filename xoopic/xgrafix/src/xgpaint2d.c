#include "xgrafixint.h"

#define MAX_SHORT 32767
#define MIN_SHORT -32768

short ToShort(SCALAR x) 
{return ((x<MIN_SHORT) ? MIN_SHORT : ((x>MAX_SHORT) ? MAX_SHORT : (short)x));}

void Paint_Window(WindowType theWindow)
{
  DataType        data_Ptr, ptr;
  LabelType       label_Ptr;
  XPoint         *X_Points, Tick_Marks[4], *StructPoints;
  XRectangle      theClientArea;
  Tk_Window       tkwin;
  StructType     *structures;

  int             n, i, j, k1, k2, maxlen, x1, y1, x2, y2;
  char            buffer1[20], buffer2[20];
  double          m, b, m1, b1, temp, m2, m3, dum1, dum2;

  tkwin = theWindow->tkwin;
  data_Ptr = theWindow->data;
  label_Ptr = theWindow->label;

  RescaleTwoDWindow(theWindow);

  if (label_Ptr->X_Max == label_Ptr->X_Min) {
    dum1 = max(1.0,fabs(label_Ptr->X_Min)*1e-3);
    label_Ptr->X_Min -= dum1;
    label_Ptr->X_Max += dum1;
  }

  if (label_Ptr->Y_Max == label_Ptr->Y_Min) {
    dum1 = max(1.0,fabs(label_Ptr->Y_Min)*1e-3);
    label_Ptr->Y_Min -= dum1;
    label_Ptr->Y_Max += dum1;
  }

  /*******************************/
  /* Draw labels and plot border */
  
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
	 
    /**********************************************************/
    /* Drawing the boundary of the client area and tick marks */

    XSetForeground(theDisplay, theWindow->xwingc, theBorderColor);
    XDrawRectangle(theDisplay, theWindow->pixmap_buffer,
		   theWindow->xwingc, x1, y1, x2 - x1, y2 - y1);

    XSetForeground(theDisplay, theWindow->xwingc, theTickMarkColor);
    if (Is_Y_Log(theWindow)) {
      temp = log10(max(DBL_MIN,
		       label_Ptr->Y_Max / (label_Ptr->Y_Min+DBL_MIN)));
      for (i = 1; i < temp; i++) {
	Tick_Marks[0].x = x1 - 3;
	Tick_Marks[0].y = y1 + i * (y2 - y1) / temp;
	Tick_Marks[1].x = x1 - 1;
	Tick_Marks[1].y = y1 + i * (y2 - y1) / temp;
	XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		   Tick_Marks, 2, CoordModeOrigin);
      }
    } else if ((label_Ptr->Y_Min < 0.0 && 0.0 < label_Ptr->Y_Max) ||
	       (label_Ptr->Y_Max < 0.0 && 0.0 < label_Ptr->Y_Min)) {
      Tick_Marks[0].x = x1 - 3;
      Tick_Marks[0].y = y1 + label_Ptr->Y_Max * (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
      Tick_Marks[1].x = x1 - 1;
      Tick_Marks[1].y = y1 + label_Ptr->Y_Max * (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
      Tick_Marks[2].x = x2 + 3;
      Tick_Marks[2].y = y1 + label_Ptr->Y_Max * (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
      Tick_Marks[3].x = x2 + 1;
      Tick_Marks[3].y = y1 + label_Ptr->Y_Max * (y2 - y1) / (label_Ptr->Y_Max - label_Ptr->Y_Min);
      XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 Tick_Marks, 2, CoordModeOrigin);
      XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 &Tick_Marks[2], 2, CoordModeOrigin);
    }
    theWindow->ulxc = x1;
    theWindow->ulyc = y1;
    theWindow->lrxc = x2;
    theWindow->lryc = y2;
  }
  x1 = theWindow->ulxc + 1;
  y1 = theWindow->ulyc + 1;
  x2 = theWindow->lrxc - 1;
  y2 = theWindow->lryc - 1;
  
  if (!Was_Open_Before(theWindow) || Is_X_AutoRescale(theWindow) || Is_Y_AutoRescale(theWindow)) {
    XSetForeground(theDisplay, theWindow->xwingc, theTickMarkColor);
    if (Is_X_Log(theWindow)) {
      temp = log10(max(DBL_MIN,
		       label_Ptr->X_Max / (label_Ptr->X_Min+DBL_MIN)));
      for (i = 1; i < temp; i++) {
	Tick_Marks[0].x = x1 + i * (x2 - x1) / temp;
	Tick_Marks[0].y = y2 + 3;
	Tick_Marks[1].x = x1 + i * (x2 - x1) / temp;
	Tick_Marks[1].y = y2 + 5;
	XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		   Tick_Marks, 2, CoordModeOrigin);
      }
    } else if ((label_Ptr->X_Min < 0.0 && 0.0 < label_Ptr->X_Max) ||
	       (label_Ptr->X_Max < 0.0 && 0.0 < label_Ptr->X_Min)) {
      Tick_Marks[0].x = x2 - label_Ptr->X_Max * (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min);
      Tick_Marks[0].y = y2 + 3;
      Tick_Marks[1].x = x2 - label_Ptr->X_Max * (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min);
      Tick_Marks[1].y = y2 + 5;
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
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, (x1 + x2 - maxlen) / 2,
		y2 + theFontAscent + 6, label_Ptr->X_Label, i);
  }

  theClientArea.x = x1;
  theClientArea.y = y1;
  theClientArea.width = x2 - x1;
  theClientArea.height = y2 - y1 + 1;
  XSetClipRectangles(theDisplay, theWindow->xwingc, 0, 0, &theClientArea, 1, Unsorted);

  if (!Was_Open_Before(theWindow) || Is_X_AutoRescale(theWindow) ||
      Is_Y_AutoRescale(theWindow) || !Is_Trace_On(theWindow)) {
    XSetForeground(theDisplay, theWindow->xwingc, theBKGDColor);
    XFillRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, x1, y1, x2 - x1, y2 - y1 + 1);
    Open_Before(theWindow);
  }

  if (Is_Y_Log(theWindow)) {
    theWindow->c2 = m = (y1 - y2) / log10(max(DBL_MIN,label_Ptr->Y_Max / 
					      (label_Ptr->Y_Min+DBL_MIN)));
    m2 = (y1 - y2) / log10(max(DBL_MIN,label_Ptr->Y_Max / 
			       (label_Ptr->Y_Min+DBL_MIN)));
    theWindow->d2 = b = (log10(max(label_Ptr->Y_Max,DBL_MIN)) * y2 - 
			 log10(max(label_Ptr->Y_Min,DBL_MIN)) * y1) / 
			   log10(max(DBL_MIN,label_Ptr->Y_Max / 
				     (label_Ptr->Y_Min+DBL_MIN))) + 0.5;
  } else {
    m = (y1 - y2) / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) * 
      label_Ptr->Y_Scale;
    theWindow->c2 = m2 = (y1 - y2)/(label_Ptr->Y_Max-label_Ptr->Y_Min+DBL_MIN);
    theWindow->d2 = b = (label_Ptr->Y_Max * y2 - label_Ptr->Y_Min * y1) / 
      (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN) + 0.5;
  }
  if (Is_X_Log(theWindow)) {
    theWindow->c1 = m1= (x2 - x1) / log10(max(label_Ptr->X_Max / 
					      (label_Ptr->X_Min+DBL_MIN), 
					      DBL_MIN));
    m3 = (x2 - x1) / log10(max(label_Ptr->X_Max / 
			       (label_Ptr->X_Min+DBL_MIN),DBL_MIN));
    theWindow->d1 = b1 = (log10(max(label_Ptr->X_Max, DBL_MIN)) * x1 - 
			  log10(max(label_Ptr->X_Min, DBL_MIN)) * x2) / 
			    log10(max(label_Ptr->X_Max / 
				      (label_Ptr->X_Min+DBL_MIN),
				      DBL_MIN)) + 0.5;
  } else {
    m1 = (x2 - x1) / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) * 
      label_Ptr->X_Scale;
    theWindow->c1 = m3 = (x2 - x1)/(label_Ptr->X_Max-label_Ptr->X_Min+DBL_MIN);
    theWindow->d1 = b1 = (label_Ptr->X_Max * x1 - label_Ptr->X_Min * x2) / 
      (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN) + 0.5;
  }

  /**********************************************/
  /* Plotting the structure(s) into the window. */

  structures = theWindow->structures;
  while(structures) {
    n = structures->numberPoints;
    
    if (!(StructPoints = (XPoint *) malloc(n * sizeof(XPoint))))
      putchar(7);
	 
    if (Is_X_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].x = 
	  ToShort(m3 * log10(max(structures->x[i], DBL_MIN)) + b1);
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].x = ToShort(m3 * structures->x[i] + b1);
    }	 

    if (Is_Y_Log(theWindow)) {
      for (i = 0; i < n; i++)
	StructPoints[i].y = 
	  ToShort(m2 * log10(max(structures->y[i], DBL_MIN)) + b);
    } else {
      for (i = 0; i < n; i++)
	StructPoints[i].y = ToShort(m2 * structures->y[i] + b);
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

  /*******************************************/
  /* Plotting the graph(es) into the window. */
  
  if (!Is_X_Log(theWindow))
  {
    if (label_Ptr->X_Scale != 0)
      dum1 = label_Ptr->X_Offset/label_Ptr->X_Scale;
    else dum1 = 0;
  }

  if (!Is_Y_Log(theWindow))
  {
    if (label_Ptr->Y_Scale != 0)
      dum2 = label_Ptr->Y_Offset/label_Ptr->Y_Scale;
    else dum2 = 0;
  }

  for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
    XSetForeground(theDisplay, theWindow->xwingc, Color[ptr->color]);
    n = *(ptr->npoints);
	 
    if (!(X_Points = (XPoint *) malloc(n * sizeof(XPoint))))
      putchar(7);
	 
    if (Is_X_Log(theWindow))
      for (i = 0; i < n; i++)
	X_Points[i].x = ToShort(m1 * 
	    log10(max(label_Ptr->X_Scale * CurveX(ptr,i) + label_Ptr->X_Offset,DBL_MIN)) + b1);
    else
      for (i = 0; i < n; i++)
	X_Points[i].x = ToShort(m1 * (CurveX(ptr,i) + dum1)+ b1);
	 
    if (Is_Y_Log(theWindow))
      for (i = 0; i < n; i++)
	X_Points[i].y = ToShort(m * 
            log10(max(label_Ptr->Y_Scale * CurveY(ptr,i) + label_Ptr->Y_Offset,DBL_MIN)) + b);
    else
      for (i = 0; i < n; i++)
	X_Points[i].y = ToShort(m * (CurveY(ptr,i) + dum2) + b);
	 
    if (Is_Scatter(ptr))
      XDrawPoints(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, X_Points, n, CoordModeOrigin);
    else
      XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, X_Points, n, CoordModeOrigin);
	 
    free(X_Points);
  }
  
  XSetClipMask(theDisplay, theWindow->xwingc, None);
}


