#include "xgrafixint.h"

#define deg2rad(x) (double)((x)*M_PI/180.0)
#define rad2deg(x) (double)((x)*180.0/M_PI)

#define  BACK         0
#define  FRONT        1

#define  ABOVE        0
#define  UPPER        1
#define  LOWER        2

#define  FIRST_QUAD   0
#define  SECOND_QUAD  1
#define  THIRD_QUAD   2
#define  FOURTH_QUAD  3

typedef enum position {NONE, TOP, BELOW} POSITION;

int CheckHiddenCrosshair(WindowType theWindow, int direction, 
			 int xIndex, int yIndex, int LeaveIndex,
			 double tan_phi, double theOtherIndexStep)
{
  int theCurrentIndex, theLastIndex, theIndexStep = 1;
  LabelType theLabel = theWindow->label;
  DataType theData = (DataType)malloc(sizeof(struct data));
  double theLineZStep, theLineZValue, theOtherIndex, theSurfZValue;
  double xmin, xmax, ymin, ymax, zmin, zmax, rho, phi;
  int xindex1, xindex2, yindex1, yindex2;
  POSITION linePos = NONE;
  double theta = theWindow->theta;

  phi = rad2deg(atan(tan_phi));
  xmin = theWindow->label->X_Min;
  xmax = theWindow->label->X_Max;
  ymin = theWindow->label->Y_Min;
  ymax = theWindow->label->Y_Max;
  zmin = theWindow->label->Z_Min;
  zmax = theWindow->label->Z_Max;
  theData->z = theWindow->data->z;
  theData->zArraySize = theWindow->data->zArraySize;
  theData->zArrayOffset = theWindow->data->zArrayOffset;

  switch(direction) {
  case 0:                    /* The X direction */
    theCurrentIndex = xIndex;
    theOtherIndex = yIndex;
    theLastIndex = LeaveIndex;
    if (xIndex > LeaveIndex) {
      theIndexStep = -1;
    }
    rho = 1.0/(sin(deg2rad(theta))*fabs(cos(deg2rad(phi))));
    theLineZStep = rho*cos(deg2rad(theta))*(zmax-zmin)/
      (double)(theWindow->mend-1-theWindow->mstart);
    break;
  case 1:                    /* the Y direction */
    theCurrentIndex = yIndex;
    theOtherIndex = xIndex;
    theLastIndex = LeaveIndex;
    if (yIndex > LeaveIndex) {
      theIndexStep = -1;
    }
    rho = 1.0/(sin(deg2rad(theta))*fabs(sin(deg2rad(phi))));
    theLineZStep = rho*cos(deg2rad(theta))*(zmax-zmin)/
      (double)(theWindow->nend-1-theWindow->nstart);
    break;
  }

  theLineZValue = SurfaceZ(theData,xIndex,yIndex)*theLabel->Z_Scale;

  theCurrentIndex += theIndexStep;
  theLastIndex += theIndexStep;
  for( ; theCurrentIndex != theLastIndex ; theCurrentIndex += theIndexStep) {
    theLineZValue += theLineZStep;

    theOtherIndex += theOtherIndexStep;
    if(direction == 0)  {
      xindex1 = theCurrentIndex;
      yindex1 = theOtherIndex + 1;
      xindex2 = theCurrentIndex;
      yindex2 = theOtherIndex;
    } else {
      xindex1 = theOtherIndex + 1;
      yindex1 = theCurrentIndex;
      xindex2 = theOtherIndex ;
      yindex2 = theCurrentIndex;
    }

    if ((int)theOtherIndex == theOtherIndex)
      theSurfZValue = SurfaceZ(theData,xindex2,yindex2)*theLabel->Z_Scale;
    else
      theSurfZValue = - (((int)theOtherIndex - theOtherIndex)*
			 SurfaceZ(theData,xindex1,yindex1))*theLabel->Z_Scale +
			   (((int)(fabs(theOtherIndex)+1)-theOtherIndex)*
			    SurfaceZ(theData,xindex2,yindex2))
			     *theLabel->Z_Scale;

    if(theSurfZValue > theLineZValue) {
      if (linePos == TOP)
	return 0;
      linePos = BELOW;
    } else if (theSurfZValue < theLineZValue) {
      if (linePos == BELOW)
	return 0;
      linePos = TOP;
    } else {
      return 0;
    }
  }

  return 1;
}

void DrawCrosshair(WindowType theWindow, int xindex, int yindex)
{
  Pixmap pm;
  XRectangle      theClientArea;
  Tk_Window tkwin = theWindow->tkwin;
  DataType theData = theWindow->data;
  LabelType theLabel = theWindow->label;
  MeshType **theMesh = theWindow->theMesh;
  double xtemp, ytemp;
  double xmin, xmax, ymin, ymax, phi, theta, tan_phi, gamma, tan_180minusphi
    , theOtherIndexStep1, theOtherIndexStep2, tan_360minusphi;
  int result = 0, LeaveIndex1, LeaveIndex2;

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

  phi = theWindow->phi;
  theta = theWindow->theta;
  xmin = theLabel->X_Min;
  xmax = theLabel->X_Max;
  ymin = theLabel->Y_Min;
  ymax = theLabel->Y_Max;

  theWindow->data->xCoord = xindex;
  theWindow->data->yCoord = yindex;

  if ((xindex==theWindow->mstart && 90.0 < phi && phi < 270.0) ||
      (xindex==theWindow->mend-1 && (phi < 90.0 || phi > 270.0)) ||
      (yindex==theWindow->nend-1 && 0.0 < phi && phi < 180.0) ||
      (yindex==theWindow->nstart && 180.0 < phi && phi < 360.0) ||
      (theta == 0.0) ||
      (theta == 180.0)) {

    result = 1;
  } else {

    tan_phi = tan(deg2rad(phi))*(double)(theWindow->nend-1-theWindow->nstart)/(double)(theWindow->mend-1-theWindow->mstart);
    tan_180minusphi = tan(deg2rad(180.0-phi))*(double)(theWindow->nend-1-theWindow->nstart)/(double)(theWindow->mend-1-theWindow->mstart);
    tan_360minusphi = tan(deg2rad(360.0-phi))*(double)(theWindow->nend-1-theWindow->nstart)/(double)(theWindow->mend-1-theWindow->mstart);

    if (phi == 0.0) {
      result = CheckHiddenCrosshair(theWindow, 0, xindex, yindex,
				    theWindow->mend-1, tan_phi, 0);
    } else if (0.0 < phi && phi < 90.0) {
      gamma = rad2deg(atan((double)(theWindow->nend-1-yindex)/
			   (double)(theWindow->mend-1-xindex)*
			   (double)(theWindow->mend-1-theWindow->mstart)/
			   (double)(theWindow->nend-1-theWindow->nstart)));
      if (gamma >= phi) {
	LeaveIndex1 = theWindow->mend-1;
	theOtherIndexStep1 = tan_phi;

	LeaveIndex2 = tan_phi * (theWindow->mend-1 - xindex) + yindex;
	theOtherIndexStep2 = 1.0/tan_phi;
      } else {
	LeaveIndex1 = (theWindow->nend-1-yindex)/tan_phi + xindex;
	theOtherIndexStep1 = tan_phi;

	LeaveIndex2 = theWindow->nend-1;
	theOtherIndexStep2 = 1.0/tan_phi;
      }

      result = (CheckHiddenCrosshair(theWindow, 0, xindex, 
				     yindex, LeaveIndex1, tan_phi, 
				     theOtherIndexStep1) &&
		CheckHiddenCrosshair(theWindow, 1, xindex, 
				     yindex, LeaveIndex2, tan_phi,
				     theOtherIndexStep2));
    } else if (phi == 90.0) {
      result = CheckHiddenCrosshair(theWindow, 1, xindex, yindex,
				    theWindow->nend-1, tan_phi, 0);
    } else if (90.0 < phi && phi < 180.0) {
      gamma = rad2deg(atan((double)(theWindow->nend-1-yindex)/
			   (double)(xindex-theWindow->mstart)*
			   (double)(theWindow->mend-1-theWindow->mstart)/
			   (double)(theWindow->nend-1-theWindow->nstart)));
      if (gamma >= (180.0 - phi)) {
	LeaveIndex1 = theWindow->mstart;
	theOtherIndexStep1 = tan_180minusphi;

	LeaveIndex2 = (xindex-theWindow->mstart)*tan_180minusphi+yindex;
	theOtherIndexStep2 = -1.0/tan_180minusphi;
      } else {
	LeaveIndex1 = xindex-(theWindow->nend-1-yindex)/tan_180minusphi + 1;
	theOtherIndexStep1 = tan_180minusphi;

	LeaveIndex2 = theWindow->nend-1;
	theOtherIndexStep2 = -1.0/tan_180minusphi;
      }
      result = (CheckHiddenCrosshair(theWindow, 0, xindex,
				     yindex, LeaveIndex1, tan_phi,
				     theOtherIndexStep1) &&
		CheckHiddenCrosshair(theWindow, 1, xindex,
				     yindex, LeaveIndex2, tan_phi,
				     theOtherIndexStep2));
    } else if (phi == 180.0) {
      result = CheckHiddenCrosshair(theWindow, 0, xindex, yindex,
				    theWindow->mstart, tan_phi, 0);
    } else if (180.0 < phi && phi < 270.0) {
      gamma = rad2deg(atan((double)(yindex-theWindow->nstart)/
			   (double)(xindex-theWindow->mstart)*
			   (double)(theWindow->mend-1-theWindow->mstart)/
			   (double)(theWindow->nend-1-theWindow->nstart)));
      if (gamma >= (phi - 180.0)) {
	LeaveIndex1 = theWindow->mstart;
	theOtherIndexStep1 = 0.0 - tan_phi;

	LeaveIndex2 = yindex - (xindex - theWindow->mstart)*tan_phi + 1;
	theOtherIndexStep2 = -1.0/tan_phi;
      } else {
	LeaveIndex1 = xindex - (yindex - theWindow->nstart)/tan_phi + 1;
	theOtherIndexStep1 = 0.0 - tan_phi;

	LeaveIndex2 = theWindow->nstart;
	theOtherIndexStep2 = -1.0/tan_phi;
      }
      result = (CheckHiddenCrosshair(theWindow, 0, xindex,
				     yindex, LeaveIndex1, tan_phi,
				     theOtherIndexStep1) &&
		CheckHiddenCrosshair(theWindow, 1, xindex,
				     yindex, LeaveIndex2, tan_phi,
				     theOtherIndexStep2));
    } else if (phi == 270.0) {
      result = CheckHiddenCrosshair(theWindow, 1, xindex, yindex,
				    theWindow->nstart, tan_phi, 0);
    } else {
      gamma = rad2deg(atan((double)(yindex-theWindow->nstart)/
			   (double)(theWindow->mend-1-xindex)*
			   (double)(theWindow->mend-1-theWindow->mstart)/
			   (double)(theWindow->nend-1-theWindow->nstart)));
      if (gamma >= 360.0 - phi) {
	LeaveIndex1 = theWindow->mend-1;
	theOtherIndexStep1 = -tan_360minusphi;

	LeaveIndex2 = yindex - (theWindow->mend-1-xindex) * tan_360minusphi+1;
	theOtherIndexStep2 = 1.0/tan_360minusphi;
      } else {
	LeaveIndex1 = (yindex - theWindow->nstart)/tan_360minusphi + xindex;
	theOtherIndexStep1 = -tan_360minusphi;
	
	LeaveIndex2 = theWindow->nstart;
	theOtherIndexStep2 = 1.0/tan_360minusphi;
      }
      result = (CheckHiddenCrosshair(theWindow, 0, xindex,
				     yindex, LeaveIndex1, tan_phi,
				     theOtherIndexStep1) &&
		CheckHiddenCrosshair(theWindow, 1, xindex,
				     yindex, LeaveIndex2, tan_phi,
				     theOtherIndexStep2));
    }
  }
			 
  xtemp = theWindow->c1 * theMesh[xindex][yindex].x + theWindow->d1 - 4;
  ytemp = theWindow->c2 * theMesh[xindex][yindex].y + theWindow->d2 - 4;
  XSetForeground(theDisplay, theWindow->xwingc, theWhitePixel);

  if(result) {
    XFillArc(theDisplay, pm, theWindow->xwingc, (int)xtemp, 
	     (int)ytemp, 8, 8, 0, 23040);

  } else {
    XDrawArc(theDisplay, pm, theWindow->xwingc, (int)xtemp, 
	     (int)ytemp, 8, 8, 0, 23040);
  }

  XCopyArea(theDisplay, pm, Tk_WindowId(tkwin), 
	    theWindow->xwingc, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 
	    0, 0);
  XFlush(theDisplay);
  XFreePixmap(theDisplay, pm);
  XSetClipMask(theDisplay, theWindow->xwingc, None);

  sprintf(TclCommand,"Update3dCrosshairValues \"%s\" %g %g %g\n",
	  theWindow->plot_title,SurfaceX(theData,xindex)*theLabel->X_Scale,
	  SurfaceY(theData,yindex)*theLabel->Y_Scale,
	  SurfaceZ(theData,xindex,yindex)*theLabel->Z_Scale);
  Tcl_Eval(interp,TclCommand);

}

void DrawWall(WindowType theWindow, MeshType *theBox, double c1, double d1, 
	      double c2, double d2, int wall, int side)
{
  XPoint          thePoint[4];
  
  if (wall == 0) {
    thePoint[0].x = ToShort(c1 * theBox[0].x + d1);
    thePoint[0].y = ToShort(c2 * theBox[0].y + d2);
    thePoint[1].x = ToShort(c1 * theBox[1].x + d1);
    thePoint[1].y = ToShort(c2 * theBox[1].y + d2);
    thePoint[2].x = ToShort(c1 * theBox[2].x + d1);
    thePoint[2].y = ToShort(c2 * theBox[2].y + d2);
    thePoint[3].x = ToShort(c1 * theBox[3].x + d1);
    thePoint[3].y = ToShort(c2 * theBox[3].y + d2);
  } else if (wall == 1) {
    thePoint[0].x = ToShort(c1 * theBox[0].x + d1);
    thePoint[0].y = ToShort(c2 * theBox[0].y + d2);
    thePoint[1].x = ToShort(c1 * theBox[1].x + d1);
    thePoint[1].y = ToShort(c2 * theBox[1].y + d2);
    thePoint[2].x = ToShort(c1 * theBox[6].x + d1);
    thePoint[2].y = ToShort(c2 * theBox[6].y + d2);
    thePoint[3].x = ToShort(c1 * theBox[5].x + d1);
    thePoint[3].y = ToShort(c2 * theBox[5].y + d2);
  } else if (wall == 2) {
    thePoint[0].x = ToShort(c1 * theBox[1].x + d1);
    thePoint[0].y = ToShort(c2 * theBox[1].y + d2);
    thePoint[1].x = ToShort(c1 * theBox[2].x + d1);
    thePoint[1].y = ToShort(c2 * theBox[2].y + d2);
    thePoint[2].x = ToShort(c1 * theBox[7].x + d1);
    thePoint[2].y = ToShort(c2 * theBox[7].y + d2);
    thePoint[3].x = ToShort(c1 * theBox[6].x + d1);
    thePoint[3].y = ToShort(c2 * theBox[6].y + d2);
  } else if (wall == 3) {
    thePoint[0].x = ToShort(c1 * theBox[2].x + d1);
    thePoint[0].y = ToShort(c2 * theBox[2].y + d2);
    thePoint[1].x = ToShort(c1 * theBox[3].x + d1);
    thePoint[1].y = ToShort(c2 * theBox[3].y + d2);
    thePoint[2].x = ToShort(c1 * theBox[4].x + d1);
    thePoint[2].y = ToShort(c2 * theBox[4].y + d2);
    thePoint[3].x = ToShort(c1 * theBox[7].x + d1);
    thePoint[3].y = ToShort(c2 * theBox[7].y + d2);
  } else if (wall == 4) {
    thePoint[0].x = ToShort(c1 * theBox[3].x + d1);
    thePoint[0].y = ToShort(c2 * theBox[3].y + d2);
    thePoint[1].x = ToShort(c1 * theBox[0].x + d1);
    thePoint[1].y = ToShort(c2 * theBox[0].y + d2);
    thePoint[2].x = ToShort(c1 * theBox[5].x + d1);
    thePoint[2].y = ToShort(c2 * theBox[5].y + d2);
    thePoint[3].x = ToShort(c1 * theBox[4].x + d1);
    thePoint[3].y = ToShort(c2 * theBox[4].y + d2);
  } else if (wall == 5) {
    thePoint[0].x = ToShort(c1 * theBox[4].x + d1);
    thePoint[0].y = ToShort(c2 * theBox[4].y + d2);
    thePoint[1].x = ToShort(c1 * theBox[5].x + d1);
    thePoint[1].y = ToShort(c2 * theBox[5].y + d2);
    thePoint[2].x = ToShort(c1 * theBox[6].x + d1);
    thePoint[2].y = ToShort(c2 * theBox[6].y + d2);
    thePoint[3].x = ToShort(c1 * theBox[7].x + d1);
    thePoint[3].y = ToShort(c2 * theBox[7].y + d2);
  }
  if (side == BACK) {
    XSetForeground(theDisplay, theWindow->xwingc, theBlackPixel);
    XFillPolygon(theDisplay, theWindow->pixmap_buffer,
		 theWindow->xwingc, thePoint, 4, Complex, CoordModeOrigin);
  }
  XSetForeground(theDisplay, theWindow->xwingc, theWhitePixel);
  XDrawLines(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, thePoint,
	     4, CoordModeOrigin);
}

/****************************************************************/

void Paint_ThreeD_Window(WindowType theWindow)
{
  DataType        data_Ptr;
  LabelType       label_Ptr;
  MeshType      **theMeshPoint=theWindow->theMesh, theBox[8];
  StringType      theString[8];
  XPoint          thePolygon[5];
  Tk_Window       tkwin;
  XRectangle      theClientArea;

  double           n1, n2, n3, v1, v2, v3;
  double           u1, u2, u3, up1, up2, up3;
  double           a1, a2, a3, b1, b2, b3;
  double           xprojmax, xprojmin;
  double           yprojmax, yprojmin;
  double           mag, c1, c2, d1, d2;
  double           xtemp, ytemp, ztemp;

  int             m, n, mstart, mend, nend, nstart;
  int             i, j, k, x1, y1, x2, y2, colornumber;
  int             phi_flag, theta_flag, i1, j1, k1, maxlen;
  int             xstart, xend, xstep, ystart, yend, ystep;

  tkwin = theWindow->tkwin;

  data_Ptr = theWindow->data;
  label_Ptr = theWindow->label;

  if (!Is_Scatter(data_Ptr)) {
    m = *(data_Ptr->mpoints);
  } 
  else {
    m = *(data_Ptr->npoints);
  }
  n = *(data_Ptr->npoints);
  
  if(m<3 || n<2) return;

  if (theWindow->theMesh == NULL) {
    theWindow->theMesh = (MeshType **) malloc(m * sizeof(MeshType *));
    theMeshPoint = theWindow->theMesh;
    for (i = 0; i < m; i++) {
      theMeshPoint[i] = (MeshType *) malloc(n * sizeof(MeshType));
    }

    theWindow->xSize = m;
    theWindow->ySize = n;
  } else if ((m != theWindow->xSize) || n != (theWindow->ySize)) {
    for (i=0; i<theWindow->xSize; i++) {
      free(theMeshPoint[i]);
    }
    free(theMeshPoint);

    theWindow->theMesh = (MeshType **) malloc(m * sizeof(MeshType *));
    theMeshPoint = theWindow->theMesh;
    for (i = 0; i < m; i++) {
      theMeshPoint[i] = (MeshType *) malloc(n * sizeof(MeshType));
    }

    theWindow->xSize = m;
    theWindow->ySize = n;
  }

  RescaleThreeDWindow(theWindow,&mstart,&mend,&nstart,&nend);

  if (label_Ptr->Z_Max == label_Ptr->Z_Min || 
      label_Ptr->Y_Max == label_Ptr->Y_Min ||
      label_Ptr->X_Max == label_Ptr->X_Min)
    return;
  
  x1 = 0;
  x2 = Tk_Width(tkwin);
  y1 = 1;
  y2 = Tk_Height(tkwin) - (theFontAscent + theFontDescent + 7);

  theWindow->ulxc = x1;
  theWindow->ulyc = y1;
  theWindow->lrxc = x2;
  theWindow->lryc = y2;

  /* Clear client area. */

  theClientArea.x = x1;
  theClientArea.y = y1;
  theClientArea.width = x2 - x1;
  theClientArea.height = y2 + (theFontAscent + theFontDescent + 7);
  XSetClipRectangles(theDisplay, theWindow->xwingc, 0, 0, &theClientArea, 1, Unsorted);

  /*******************************************************************/

  XSetForeground(theDisplay, theWindow->xwingc, GrayShade[3]);
  XFillRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		 0, 0, Tk_Width(tkwin), Tk_Height(tkwin));
  
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

  if (theta_flag == ABOVE) {
    sprintf(theString[0].buffer, "%.4G", label_Ptr->Y_Max);
    sprintf(theString[1].buffer, "%s", label_Ptr->Y_Label);
    sprintf(theString[2].buffer, "%.4G", label_Ptr->Y_Min);
    sprintf(theString[3].buffer, "%.4G", label_Ptr->X_Min);
    sprintf(theString[4].buffer, "%s", label_Ptr->X_Label);
    sprintf(theString[5].buffer, "%.4G", label_Ptr->X_Max);
  } else {
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

  /*******************************/
  /* painting the color spectra  */
  
  if (Is_ColorCode_On(theWindow)) {
    for (i = 0; i < 2 * MAXTHREEDCOLORS; i += 2) {
      XSetForeground(theDisplay, theWindow->xwingc, ThreeDColor[i / 2]);
      thePolygon[0].x = ToShort(theWindow->ulxc + i);
      thePolygon[0].y = 0;
      thePolygon[1].x = ToShort(theWindow->ulxc + i);
      thePolygon[1].y = ToShort(theWindow->ulyc);
      XDrawLines(theDisplay, theWindow->pixmap_buffer,
		 theWindow->xwingc, thePolygon, 2, CoordModeOrigin);

      thePolygon[0].x = ToShort(theWindow->ulxc + i + 1);
      thePolygon[0].y = 0;
      thePolygon[1].x = ToShort(theWindow->ulxc + i + 1);
      thePolygon[1].y = ToShort(theWindow->ulyc);
      XDrawLines(theDisplay, theWindow->pixmap_buffer,
		 theWindow->xwingc, thePolygon, 2, CoordModeOrigin);
    }
    XSetForeground(theDisplay, theWindow->xwingc, ThreeDColor[0]);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		theWindow->ulxc, theWindow->ulyc + theFontAscent + 2, "L", 1);
    
    XSetForeground(theDisplay, theWindow->xwingc, ThreeDColor[MAXTHREEDCOLORS - 1]);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		theWindow->ulxc + 2 * MAXTHREEDCOLORS - XTextWidth(theFontStruct, "H", 1),
		theWindow->ulyc + theFontAscent + 2, "H", 1);
    y1 += theFontHeight + 2;
  }
  /***************************************/
  /* painting the labels                 */
  
  theWindow->c2 = c2 = (y1 - y2) / (yprojmax - yprojmin + DBL_MIN);
  theWindow->d2 = d2 = (yprojmax * y2 - yprojmin * y1) / 
    (yprojmax - yprojmin + DBL_MIN) + 0.5;

  XSetForeground(theDisplay, theWindow->xwingc, theWinTextColor);

  if (theta_flag == ABOVE) {
    i = strlen(theString[0].buffer);
    i1 = XTextWidth(theFontStruct, theString[0].buffer, i);
    j = strlen(theString[1].buffer);
    j1 = XTextWidth(theFontStruct, theString[1].buffer, j);
    k = strlen(theString[2].buffer);
    k1 = XTextWidth(theFontStruct, theString[2].buffer, k);
    maxlen = (i1 > j1) ? i1 : j1;
    maxlen = (k1 > maxlen) ? k1 : maxlen;
    
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x1 + maxlen - i1, y1 + theFontAscent, theString[0].buffer, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x1 + (maxlen - j1) / 2, (y1 + y2) / 2, theString[1].buffer, j);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x1 + maxlen - k1, y2 - theFontDescent, theString[2].buffer, k);
    x1 += maxlen + 2;
    
    i = strlen(theString[3].buffer);
    j = strlen(theString[4].buffer);
    k = strlen(theString[5].buffer);
    k1 = XTextWidth(theFontStruct, theString[5].buffer, k);
    
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x1, y2 + theFontAscent + 2, theString[3].buffer, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		(x1 + x2) / 2, y2 + theFontAscent + 2, theString[4].buffer, j);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x2 - k1 - 1, y2 + theFontAscent + 2, theString[5].buffer, k);
    
    theWindow->c1 = c1 = (x2 - x1) / (xprojmax - xprojmin + DBL_MIN);
    theWindow->d1 = d1 = (xprojmax * x1 - xprojmin * x2) / (xprojmax - xprojmin + DBL_MIN) + 0.5;
    
    if (Is_Scatter(data_Ptr)) {
      XSetForeground(theDisplay, theWindow->xwingc, theBKGDColor);
      XFillRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, x1, y1, x2 - x1, y2 - y1 + 1);
      XSetForeground(theDisplay, theWindow->xwingc, theWhitePixel);
      XDrawRectangle(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc, x1, y1, x2 - x1, y2 - y1);
    }
  } else {
    i = strlen(theString[0].buffer);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x1, (int) (c2 * theString[0].y + d2 + theFontAscent), theString[0].buffer, i);
    x1 += XTextWidth(theFontStruct, theString[0].buffer, i) + 2;
    
    i = strlen(theString[5].buffer);
    i1 = XTextWidth(theFontStruct, theString[5].buffer, i);
    j = strlen(theString[6].buffer);
    j1 = XTextWidth(theFontStruct, theString[6].buffer, j);
    k = strlen(theString[7].buffer);
    k1 = XTextWidth(theFontStruct, theString[7].buffer, k);
    
    maxlen = (i1 > j1) ? i1 : j1;
    maxlen = (k1 > maxlen) ? k1 : maxlen;
    x2 -= maxlen;
    
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x2, (int) (c2 * theString[5].y + d2 + theFontAscent + 5), theString[5].buffer, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x2 + maxlen - j1, (int) (c2 * theString[6].y + d2 - theFontDescent - 2), theString[6].buffer, j);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		x2 + maxlen - k1, (int) (c2 * theString[7].y + d2 + theFontAscent), theString[7].buffer, k);
    x2 -= 2;
    
    theWindow->c1 = c1 = (x2 - x1) / (xprojmax - xprojmin + DBL_MIN);
    theWindow->d1 = d1 = (xprojmax * x1 - xprojmin * x2) / (xprojmax - xprojmin + DBL_MIN) + 0.5;
    
    i = strlen(theString[2].buffer);
    i1 = XTextWidth(theFontStruct, theString[2].buffer, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		(int) (c1 * theString[2].x + d1 - i1 - 5), y2 + theFontAscent + 2, theString[2].buffer, i);
    
    j = strlen(theString[3].buffer);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		(int) (c1 * theString[3].x + d1 + 5), y2 + theFontAscent + 2, theString[3].buffer, j);
    
    i = strlen(theString[1].buffer);
    i1 = XTextWidth(theFontStruct, theString[1].buffer, i);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		(int) (c1 * theString[1].x + d1 - i1 - 5), (int) (c2 * theString[1].y + d2 + theFontAscent), theString[1].buffer, i);
    
    j = strlen(theString[4].buffer);
    XDrawString(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		(int) (c1 * theString[4].x + d1 + 5), (int) (c2 * theString[4].y + d2 + theFontAscent + 5), theString[4].buffer, j);
  }
  
  theClientArea.x = x1;
  theClientArea.y = y1;
  theClientArea.width = x2 - x1 + 1;
  theClientArea.height = y2 - y1 + 1;
  XSetClipRectangles(theDisplay, theWindow->xwingc, 0, 0, &theClientArea, 1, Unsorted);
  
    /***********************/
    /* Draw the back walls */

    if (theta_flag != ABOVE) {
      switch (phi_flag) {
      case FIRST_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, BACK);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 1, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 4, BACK);
	break;

      case SECOND_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, BACK);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 1, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 2, BACK);
	break;

      case THIRD_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, BACK);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 2, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 3, BACK);
	break;

      case FOURTH_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, BACK);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 3, BACK);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 4, BACK);
	break;
      }
    }
    /***********************************************/
    /* Used for scaling the arrays before rotating */

    if (Is_X_Log(theWindow)) {
      a1 = 1.0 / log10(max(label_Ptr->X_Max / (label_Ptr->X_Min + DBL_MIN),
			   DBL_MIN));
      b1 = log10(max(label_Ptr->X_Min / (label_Ptr->X_Scale+DBL_MIN),DBL_MIN))/
	   log10(max(label_Ptr->X_Max / (label_Ptr->X_Min+DBL_MIN), DBL_MIN));
    } else {
      a1 = label_Ptr->X_Scale / (label_Ptr->X_Max - label_Ptr->X_Min +DBL_MIN);
      b1 = label_Ptr->X_Min / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN);
    }
    if (Is_Y_Log(theWindow)) {
      a2 = 1.0 / log10(max(label_Ptr->Y_Max / (label_Ptr->Y_Min+DBL_MIN),
			   DBL_MIN));
      b2 = log10(max(label_Ptr->Y_Min/(label_Ptr->Y_Scale+DBL_MIN),DBL_MIN)) / 
	   log10(max(label_Ptr->Y_Max / (label_Ptr->Y_Min+DBL_MIN),DBL_MIN));
    } else {
      a2 = label_Ptr->Y_Scale / (label_Ptr->Y_Max - label_Ptr->Y_Min +DBL_MIN);
      b2 = label_Ptr->Y_Min / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN);
    }
    if (Is_Z_Log(theWindow)) {
      a3 = 1.0 / log10(max(label_Ptr->Z_Max / (label_Ptr->Z_Min+DBL_MIN),
			   DBL_MIN));
      b3 = log10(max(label_Ptr->Z_Min/(label_Ptr->Z_Scale+DBL_MIN),DBL_MIN)) / 
	   log10(max(label_Ptr->Z_Max / (label_Ptr->Z_Min+DBL_MIN),DBL_MIN));
    } else {
      a3 = label_Ptr->Z_Scale / (label_Ptr->Z_Max - label_Ptr->Z_Min+DBL_MIN);
      b3 = label_Ptr->Z_Min / (label_Ptr->Z_Max - label_Ptr->Z_Min + DBL_MIN);
    }

    /********************************************/
    /* do the actual 3d drawing onto the pixmap */

    if (Is_Scatter(data_Ptr)) {
      XSetForeground(theDisplay, theWindow->xwingc, data_Ptr->color);
      for (i = 0; i < n; i++) {
	if (Is_X_Log(theWindow))
	  xtemp = max(a1 * log10(fabs(ScatterX(data_Ptr,i)) + DBL_MIN) - 
		      b1,DBL_MIN);
	else
	  xtemp = a1 * ScatterX(data_Ptr,i) - b1;
	if (Is_Y_Log(theWindow))
	  ytemp = max(a2 * log10(fabs(ScatterY(data_Ptr,i)) + DBL_MIN) - 
		      b2,DBL_MIN);
	else
	  ytemp = a2 * ScatterY(data_Ptr,i) - b2;
	if (Is_Z_Log(theWindow))
	  ztemp = max(a3 * log10(fabs(ScatterZ(data_Ptr,i)) + DBL_MIN) - 
		      b3,DBL_MIN);
	else
	  ztemp = a3 * ScatterZ(data_Ptr,i) - b3;
	if (0.0 < xtemp && xtemp < 1.0 && 0.0 < ytemp && ytemp < 1.0 && 0.0 < ztemp && ztemp < 1.0) {
	  if (Is_ColorCode_On(theWindow)) {
	    if (Is_Z_Log(theWindow))
	      colornumber = MAXTHREEDCOLORS * 
		(a3 * log10(fabs(ScatterZ(data_Ptr,i)) + DBL_MIN) - b3);
	    else
	      colornumber = MAXTHREEDCOLORS * 
		(a3 * (ScatterZ(data_Ptr,i)) - b3);
	    if (colornumber >= MAXTHREEDCOLORS)
	      colornumber = MAXTHREEDCOLORS - 1;
	    else if (colornumber < 0)
	      colornumber = 0;
	    XSetForeground(theDisplay, theWindow->xwingc, ThreeDColor[colornumber]);
	  }
	  XDrawPoint(theDisplay, theWindow->pixmap_buffer, theWindow->xwingc,
		     (int) (c1 * (u1 * xtemp + u2 * ytemp + u3 * ztemp) + d1),
		     (int) (c2 * (v1 * xtemp + v2 * ytemp + v3 * ztemp) + d2));
	}
      }
    } else {
      for (i=0; i<m; i++) {

	if (Is_Irr_Surf(data_Ptr)) {
	  for (j = 0; j < n; j++) {
	    if (Is_X_Log(theWindow))
	      xtemp = max(a1 * log10(fabs(IrregularX(data_Ptr,i,j))+ DBL_MIN) -
			  b1,DBL_MIN);
	    else
	      xtemp = a1 * IrregularX(data_Ptr,i,j) - b1;
	    if (Is_Y_Log(theWindow))
	      ytemp = max(a2 * log10(fabs(IrregularY(data_Ptr,i,j))+ DBL_MIN) -
			  b2,DBL_MIN);
	    else
	      ytemp = a2 * IrregularY(data_Ptr,i,j) - b2;
	    if (Is_Z_Log(theWindow))
	      ztemp = max(a3 * log10(fabs(IrregularZ(data_Ptr,i,j))+ DBL_MIN) -
			  b3,DBL_MIN);
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
	      xtemp = max(a1 * log10(fabs(SurfaceX(data_Ptr,i)+DBL_MIN)) - 
			  b1,DBL_MIN);
	    else
	      xtemp = a1 * SurfaceX(data_Ptr,i) - b1;
	    if (Is_Y_Log(theWindow))
	      ytemp = max(a2 * log10(fabs(SurfaceY(data_Ptr,j)+DBL_MIN)) - 
			  b2,DBL_MIN);
	    else
	      ytemp = a2 * SurfaceY(data_Ptr,j) - b2;
	    if (Is_Z_Log(theWindow))
	      ztemp = max(a3 * log10(fabs(SurfaceZ(data_Ptr,i,j)+DBL_MIN)) - 
			  b3,DBL_MIN);
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

      if (Is_Crosshair_On(theWindow)) {
	theWindow->mstart = mstart;
	theWindow->mend = mend;
	theWindow->nstart = nstart;
	theWindow->nend = nend;
	if (theWindow->data->xCoord < mstart)
	  theWindow->data->xCoord = mstart;
	else if (theWindow->data->xCoord > mend-1)
	  theWindow->data->xCoord = mend-1;
	if(theWindow->data->yCoord < nstart)
	  theWindow->data->yCoord = nstart;
	else if (theWindow->data->yCoord > nend-1)
	  theWindow->data->yCoord = nend;
	sprintf(TclCommand,"Set3dCross \"%s\" %d %d %d %d\n",theWindow->plot_title,mstart,mend,nstart,nend);
	Tcl_Eval(interp,TclCommand);
	Open_Before(theWindow);
      }

      for (i = xstart; i != xend; i += xstep)
	for (j = ystart; j != yend; j += ystep) {
	  thePolygon[0].x = 
	    ToShort(thePolygon[4].x = c1 * theMeshPoint[i][j].x + d1);
	  thePolygon[0].y = thePolygon[4].y = 
	    ToShort(c2 * theMeshPoint[i][j].y + d2);
	  
	  thePolygon[1].x = ToShort(c1 * theMeshPoint[i + 1][j].x + d1);
	  thePolygon[1].y = ToShort(c2 * theMeshPoint[i + 1][j].y + d2);
	  
	  thePolygon[2].x = ToShort(c1 * theMeshPoint[i + 1][j + 1].x + d1);
	  thePolygon[2].y = ToShort(c2 * theMeshPoint[i + 1][j + 1].y + d2);

	  thePolygon[3].x = ToShort(c1 * theMeshPoint[i][j + 1].x + d1);
	  thePolygon[3].y = ToShort(c2 * theMeshPoint[i][j + 1].y + d2);
	  
	  if (Is_Shading_On(theWindow)) {
	    if (Is_ColorCode_On(theWindow)) {
	      if (Is_Z_Log(theWindow))
		colornumber = MAXTHREEDCOLORS * 
		  (.25 * a3 * log10(fabs(SurfaceZ(data_Ptr,i,j) * 
					 SurfaceZ(data_Ptr,i+1,j) *
					 SurfaceZ(data_Ptr,i+1,j+1) * 
					 SurfaceZ(data_Ptr,i,j+1)) + 
					 DBL_MIN) - b3);
	      else
		colornumber = MAXTHREEDCOLORS * 
		   (.25 * a3 * (SurfaceZ(data_Ptr,i,j) + 
				SurfaceZ(data_Ptr,i+1,j) + 
				SurfaceZ(data_Ptr,i+1,j+1) + 
				SurfaceZ(data_Ptr,i,j+1)) - b3);
	      if (colornumber >= MAXTHREEDCOLORS)
		colornumber = MAXTHREEDCOLORS - 1;
	      else if (colornumber < 0)
		colornumber = 0;
	      XSetForeground(theDisplay, theWindow->xwingc, ThreeDColor[colornumber]);
	    } else
	      XSetForeground(theDisplay, theWindow->xwingc, data_Ptr->color);
	  } else
	    XSetForeground(theDisplay, theWindow->xwingc, theBlackPixel);
	  
	  XFillPolygon(theDisplay, theWindow->pixmap_buffer,
		       theWindow->xwingc, thePolygon, 5, Complex, CoordModeOrigin);

	  if (Is_Shading_On(theWindow))
	    XSetForeground(theDisplay, theWindow->xwingc, theBlackPixel);
	  else {
	    if (Is_ColorCode_On(theWindow)) {
	      if (Is_Z_Log(theWindow))
		colornumber = MAXTHREEDCOLORS * 
		  (.25 * a3 * log10(fabs(SurfaceZ(data_Ptr,i,j) * 
					 SurfaceZ(data_Ptr,i+1,j) * 
					 SurfaceZ(data_Ptr,i+1,j+1) * 
					 SurfaceZ(data_Ptr,i,j+1)) + 
				    DBL_MIN) - b3);
	      else
		colornumber = MAXTHREEDCOLORS * 
		   (.25 * a3 * (SurfaceZ(data_Ptr,i,j) + 
				SurfaceZ(data_Ptr,i+1,j) + 
				SurfaceZ(data_Ptr,i+1,j+1) + 
				SurfaceZ(data_Ptr,i,j+1)) - b3);
	      if (colornumber >= MAXTHREEDCOLORS)
		colornumber = MAXTHREEDCOLORS - 1;
	      else if (colornumber < 0)
		colornumber = 0;
	      XSetForeground(theDisplay, theWindow->xwingc, ThreeDColor[colornumber]);
	    } else
	      XSetForeground(theDisplay, theWindow->xwingc, data_Ptr->color);
	  }
	  if (Is_Grid_On(theWindow))
	    XDrawLines(theDisplay, theWindow->pixmap_buffer,
		       theWindow->xwingc, thePolygon, 5, CoordModeOrigin);
	}
    }

    /***********************/
    /* Draw the front walls */

    if (theta_flag != ABOVE) {
      switch (phi_flag) {
      case FIRST_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, FRONT);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 2, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 3, FRONT);
	break;

      case SECOND_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, FRONT);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 3, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 4, FRONT);
	break;

      case THIRD_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, FRONT);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 1, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 4, FRONT);
	break;

      case FOURTH_QUAD:
	if (theta_flag == UPPER)
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 5, FRONT);
	else
	  DrawWall(theWindow, theBox, c1, d1, c2, d2, 0, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 1, FRONT);
	DrawWall(theWindow, theBox, c1, d1, c2, d2, 2, FRONT);
	break;
      }
    }

  XSetClipMask(theDisplay, theWindow->xwingc, None);
}

  /****************************************************************/

  /****************************************************************/







