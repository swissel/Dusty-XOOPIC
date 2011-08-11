#include "xgrafixint.h"

void RescaleTwoDWindow(WindowType theWindow)
{
  DataType ptr, data_Ptr = theWindow->data;
  LabelType label_Ptr = theWindow->label;

  int i,n,MinMaxInit;
  double temp;
  double x_min_temp, x_max_temp;
  double y_min_temp, y_max_temp;
  SCALAR xc;

  /***************************/
  /* AutoRescaling the x-axis */

  if (Is_X_AutoRescale(theWindow) && (n = *(theWindow->data->npoints))) {
    x_max_temp = x_min_temp = CurveX(data_Ptr,0);
    for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
      n = *(ptr->npoints);
      ptr->w = (SCALAR**)0;
      ptr->z = (SCALAR**)n;
      for (i = 0; i < n; i++) {
	x_max_temp = max(x_max_temp, CurveX(ptr,i));
	x_min_temp = min(x_min_temp, CurveX(ptr,i));
      }
    }
    if(label_Ptr->X_Scale < 0.0) {
      temp = x_max_temp;
      x_max_temp = x_min_temp;
      x_min_temp = temp;
    }

    /* mindgame: update label_Ptr->X */
    label_Ptr->X_Max = x_max_temp*label_Ptr->X_Scale+label_Ptr->X_Offset;
    label_Ptr->X_Min = x_min_temp*label_Ptr->X_Scale+label_Ptr->X_Offset;

    if (Is_X_Log(theWindow))
      label_Ptr->X_Min = max(label_Ptr->X_Min, label_Ptr->X_Max / 1e6);
  }
  else if (!Is_X_AutoRescale(theWindow) && (n = *(theWindow->data->npoints))) {
    /* mindgame: label_Ptr->X does not change */
    x_max_temp = (label_Ptr->X_Max-label_Ptr->X_Offset)/label_Ptr->X_Scale;
    x_min_temp = (label_Ptr->X_Min-label_Ptr->X_Offset)/label_Ptr->X_Scale;

    for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
      n = *(ptr->npoints);
      ptr->w = (SCALAR**)n;
      ptr->z = (SCALAR**)n;
        /* now z-w = number of points in x-range, RT */
      if (!Is_Scatter(data_Ptr)) {
	/* this routine does not work for scattered data, RT */
        for (i = 0; i < n; i++) {
	  if (x_min_temp <= CurveX(ptr,i)) {
	    ptr->w = (SCALAR**)i;
	    break;
	  }
        }
        for (; i<n; i++) {
	  if (x_max_temp < CurveX(ptr,i)) {
	    ptr->z = (SCALAR**)i;
	    break;
	  } else if (x_max_temp == CurveX(ptr,i)) {
	    ptr->z = (SCALAR**)(i+1);
	    break;
	  }
        }
      }
    }
  }
  if (Is_X_Log(theWindow)) {
	 if(label_Ptr->X_Max==label_Ptr->X_Min && label_Ptr->X_Max==0) {
		/* cheesy defaults. */
		label_Ptr->X_Max = 10;
		label_Ptr->X_Min = 1;
	 }
	 else {
		label_Ptr->X_Max = pow(10.0, ceil(log10(max(label_Ptr->X_Max, DBL_MIN))));
		label_Ptr->X_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->X_Min,
							DBL_MIN))));
	 }
  }

  /****************************/
  /* AutoRescaling the y-axis */

  if (Is_Y_AutoRescale(theWindow) && (n = *(theWindow->data->npoints))) {
    /* mindgame: do not forget that label_Ptr->X and CurveX have different scales */ 
    x_max_temp = (label_Ptr->X_Max-label_Ptr->X_Offset)/label_Ptr->X_Scale;
    x_min_temp = (label_Ptr->X_Min-label_Ptr->X_Offset)/label_Ptr->X_Scale;
    y_max_temp = y_min_temp = 0;
    MinMaxInit = 0;
    /* Init of Y_Min and Y_Max postponed, RT */
    if (Is_Scatter(data_Ptr) && (!Is_X_AutoRescale(theWindow))) {
      /* The problematic case, RT */
      for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
        n = *(ptr->npoints);
	i = 0;
	if (!MinMaxInit) { /* find first point in x-range, RT */
          for (; i < n; i++) {
	    xc = CurveX(ptr,i); /* check for points in x-range, RT */
            if ((xc >= x_min_temp) && (xc <= x_max_temp)) {
	      y_max_temp = y_min_temp = CurveY(ptr,i);
              MinMaxInit = 1; break;
	    }
	  }
	}
        for (; i < n; i++) { /* find other points, RT */
	  xc = CurveX(ptr,i);
          if ((xc >= x_min_temp) && (xc <= x_max_temp)) {
	    y_max_temp = max(y_max_temp, CurveY(ptr,i));
	    y_min_temp = min(y_min_temp, CurveY(ptr,i));
	  }
        }
      }
    } else { /* other cases, RT */
      for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
        n = *(ptr->npoints);
        if (((int)(ptr->z) > (int)(ptr->w)) && (!MinMaxInit)) {
	  /* init Y_min, Y_max only at a suitable point, RT */
	  y_max_temp = y_min_temp = CurveY(ptr,(int)ptr->w);
          MinMaxInit = 1;
	}
        for (i = (int)(ptr->w); i < (int)(ptr->z); i++) {
	  y_max_temp = max(y_max_temp, CurveY(ptr,i));
	  y_min_temp = min(y_min_temp, CurveY(ptr,i));
        }
      }
    }
    if(label_Ptr->Y_Scale < 0.0) {
      temp = y_max_temp;
      y_max_temp = y_min_temp;
      y_min_temp = temp;
    }
    /* mindgame: update label_Ptr->Y */
    label_Ptr->Y_Max = y_max_temp*label_Ptr->Y_Scale+label_Ptr->Y_Offset;
    label_Ptr->Y_Min = y_min_temp*label_Ptr->Y_Scale+label_Ptr->Y_Offset;
	 
    if (Is_Y_Log(theWindow))
      label_Ptr->Y_Min = max(label_Ptr->Y_Min, label_Ptr->Y_Max / 1e6);
  }
  if (Is_Y_Log(theWindow)) {
	 if(label_Ptr->Y_Max==label_Ptr->Y_Min && label_Ptr->Y_Max==0) {
		/* cheesy defaults. */
		label_Ptr->Y_Max = 10;
		label_Ptr->Y_Min = 1;
	 }
	 else {
		label_Ptr->Y_Max = pow(10.0, ceil(log10(max(label_Ptr->Y_Max, DBL_MIN))));
		label_Ptr->Y_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->Y_Min,
							DBL_MIN))));
	 }
  }
}


void RescaleVectorDWindow(WindowType theWindow, int *ms, int *me, int *ns, 
			  int *ne)
{
  DataType data_Ptr = theWindow->data;
  LabelType label_Ptr = theWindow->label;
  int i,m,n,mstart,mend,nstart,nend;

  m = *(data_Ptr->mpoints);
  n = *(data_Ptr->npoints);

  /****************************/
  /* AutoRescaling the x-axis */

  if (Is_X_AutoRescale(theWindow)) {
    if (n) {
      mstart = 0;
      mend = m - 1;
      label_Ptr->X_Max = label_Ptr->X_Min = VectorX(data_Ptr,0);
      for (i = 1; i < m; i++) {
	label_Ptr->X_Max = max(label_Ptr->X_Max, VectorX(data_Ptr,i));
	label_Ptr->X_Min = min(label_Ptr->X_Min, VectorX(data_Ptr,i));
      }
    }
    label_Ptr->X_Max *= label_Ptr->X_Scale;
    label_Ptr->X_Min *= label_Ptr->X_Scale;
  } else {
    mstart = 0;
    while (VectorX(data_Ptr,mstart) * label_Ptr->X_Scale < label_Ptr->X_Min - DBL_MIN)
      mstart++;
    mend = m - 1;
    while (VectorX(data_Ptr,mend) * label_Ptr->X_Scale > label_Ptr->X_Max + DBL_MIN)
      mend--;
  }

  /****************************/
  /* AutoRescaling the y-axis */

  if (Is_Y_AutoRescale(theWindow)) {
    if (m) {
      nstart = 0;
      nend = n - 1;
      label_Ptr->Y_Max = label_Ptr->Y_Min = VectorY(data_Ptr,0);
      for (i = 1; i < n; i++) {
	label_Ptr->Y_Max = max(label_Ptr->Y_Max, VectorY(data_Ptr,i));
	label_Ptr->Y_Min = min(label_Ptr->Y_Min, VectorY(data_Ptr,i));
      }
    }
    label_Ptr->Y_Max *= label_Ptr->Y_Scale;
    label_Ptr->Y_Min *= label_Ptr->Y_Scale;
  } else {
    nstart = 0;
    while (VectorY(data_Ptr,nstart) * label_Ptr->Y_Scale < label_Ptr->Y_Min - DBL_MIN)
      nstart++;
    nend = n - 1;
    while (VectorY(data_Ptr,nend) * label_Ptr->Y_Scale > label_Ptr->Y_Max + DBL_MIN)
      nend--;
  }

  *ms = mstart;
  *me = mend;
  *ns = nstart;
  *ne = nend;
}


void RescaleThreeDWindow(WindowType theWindow, int *ms, int *me, int *ns, 
			 int *ne)
{
  DataType data_Ptr = theWindow->data;
  LabelType label_Ptr = theWindow->label;
  int i,j,n,m,mstart,mend,nstart,nend,MinMaxInit;
  double temp;
  SCALAR xc,yc;
  double x_min_temp, x_max_temp;
  double y_min_temp, y_max_temp;

  /****************************/
  /* AutoRescaling the x-axis */
  
  if (!Is_Scatter(data_Ptr))
    m = *(data_Ptr->mpoints);
  n = *(data_Ptr->npoints);

  if (Is_X_AutoRescale(theWindow)) {
    if (Is_Scatter(data_Ptr)) {
      if (n) {
	label_Ptr->X_Max = label_Ptr->X_Min = ScatterX(data_Ptr,0);
	for (i = 1; i < n; i++) {
	  label_Ptr->X_Max = max(label_Ptr->X_Max, ScatterX(data_Ptr,i));
	  label_Ptr->X_Min = min(label_Ptr->X_Min, ScatterX(data_Ptr,i));
	}
	if(label_Ptr->X_Max == label_Ptr->X_Min) return;
      }
    } else if (Is_Irr_Surf(data_Ptr)) {
      if (n && m) {
	label_Ptr->X_Max = label_Ptr->X_Min = IrregularX(data_Ptr,0,0);
	for (i = 1; i < m; i++)
	  for (j = 0; j < n; j++) {
	    label_Ptr->X_Max = max(label_Ptr->X_Max, IrregularX(data_Ptr,i,j));
	    label_Ptr->X_Min = min(label_Ptr->X_Min, IrregularX(data_Ptr,i,j));
	  }
	  if(label_Ptr->X_Max == label_Ptr->X_Min) return;
      }
    } else if (m) {
      label_Ptr->X_Max = SurfaceX(data_Ptr,m - 1);
      label_Ptr->X_Min = SurfaceX(data_Ptr,0);
		if(label_Ptr->X_Max == label_Ptr->X_Min) return;
    }

    if(label_Ptr->X_Scale < 0.0) {
      temp = label_Ptr->X_Max;
      label_Ptr->X_Max = label_Ptr->X_Min;
      label_Ptr->X_Min = temp;
    }

    label_Ptr->X_Max *= label_Ptr->X_Scale;
    label_Ptr->X_Min *= label_Ptr->X_Scale;

    if (Is_X_Log(theWindow))
      label_Ptr->X_Min = max(label_Ptr->X_Min, 1e-6 * label_Ptr->X_Max);
  }
  if (Is_X_Log(theWindow)) {
    label_Ptr->X_Max = pow(10.0, ceil(log10(max(label_Ptr->X_Max, DBL_MIN))));
    label_Ptr->X_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->X_Min,
							DBL_MIN))));
  }
  if (!Is_Scatter(data_Ptr)) {
    /* mindgame: Do not repeat the scaling calculation. */
    x_min_temp = label_Ptr->X_Min/label_Ptr->X_Scale;
    x_max_temp = label_Ptr->X_Max/label_Ptr->X_Scale;
    if (Is_Irr_Surf(data_Ptr)) {
      for (i = 0, j = 0; j < n; j++) {
	while (x_min_temp > IrregularX(data_Ptr,i,j)
	       && i < m - 1)
	  i++;
	mstart = i;
      }
      for (i = m - 1, j = 0; j < n; j++) {
	while (x_max_temp < IrregularX(data_Ptr,i,j)
	       && i)
	  i--;
	mend = i + 1;
      }
    } else {
      mstart = m * (x_min_temp - SurfaceX(data_Ptr,0)) / (SurfaceX(data_Ptr,m-1) - SurfaceX(data_Ptr,0)) + .5;
      mend = m * (x_max_temp - SurfaceX(data_Ptr,0)) / (SurfaceX(data_Ptr,m-1) - SurfaceX(data_Ptr,0));
    }
    if (mstart < 0)
      mstart = 0;
    else if (mstart > m - 1)
      mstart = m - 1;
    if (mend < 0)
      mend = 0;
    else if (mend > m)
      mend = m;
  }
  /****************************/
  /* AutoRescaling the y-axis */

  if (Is_Y_AutoRescale(theWindow)) {
    if (Is_Scatter(data_Ptr)) {
      if (n) {
	label_Ptr->Y_Max = label_Ptr->Y_Min = ScatterY(data_Ptr,0);
	for (i = 1; i < n; i++) {
	  label_Ptr->Y_Max = max(label_Ptr->Y_Max, ScatterY(data_Ptr,i));
	  label_Ptr->Y_Min = min(label_Ptr->Y_Min, ScatterY(data_Ptr,i));
	}
	if(label_Ptr->Y_Max == label_Ptr->Y_Min) return;
      }
    } else if (Is_Irr_Surf(data_Ptr)) {
      if (n && m) {
	label_Ptr->Y_Max = label_Ptr->Y_Min = IrregularY(data_Ptr,0,0);
	for (i = 1; i < m; i++)
	  for (j = 0; j < n; j++) {
	    label_Ptr->Y_Max = max(label_Ptr->Y_Max, IrregularY(data_Ptr,i,j));
	    label_Ptr->Y_Min = min(label_Ptr->Y_Min, IrregularY(data_Ptr,i,j));
	  }
	if(label_Ptr->Y_Max == label_Ptr->Y_Min) return;
      }
    } else if (n) {
      label_Ptr->Y_Max = SurfaceY(data_Ptr,n - 1);
      label_Ptr->Y_Min = SurfaceY(data_Ptr,0);
		if(label_Ptr->Y_Max == label_Ptr->Y_Min) return;
    }
    if(label_Ptr->Y_Scale < 0.0) {
      temp = label_Ptr->Y_Max;
      label_Ptr->Y_Max = label_Ptr->Y_Min;
      label_Ptr->Y_Min = temp;
    }
    label_Ptr->Y_Max *= label_Ptr->Y_Scale;
    label_Ptr->Y_Min *= label_Ptr->Y_Scale;
    if (Is_Y_Log(theWindow))
      label_Ptr->Y_Min = max(label_Ptr->Y_Min, label_Ptr->Y_Max / 1e6);
 }
  if (Is_Y_Log(theWindow)) {
    label_Ptr->Y_Max = pow(10.0, ceil(log10(max(label_Ptr->Y_Max, DBL_MIN))));
    label_Ptr->Y_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->Y_Min,
							DBL_MIN))));
  }
  if (!Is_Scatter(data_Ptr)) {
    /* mindgame: Do not repeat the scaling calculation. */
    y_min_temp = label_Ptr->Y_Min/label_Ptr->Y_Scale;
    y_max_temp = label_Ptr->Y_Max/label_Ptr->Y_Scale;
    if (Is_Irr_Surf(data_Ptr)) {
      for (j = 0, i = 0; i < m; i++) {
	while (y_min_temp > IrregularY(data_Ptr,i,j)
	       && j < n - 1)
	  j++;
	nstart = j;
      }
      for (j = n - 1, i = 0; i < m; i++) {
	while (y_max_temp < IrregularY(data_Ptr,i,j)
	       && j)
	  j--;
	nend = j + 1;
      }
    } else {
      nstart = n * (y_min_temp - SurfaceY(data_Ptr,0)) / (SurfaceY(data_Ptr,n - 1) - SurfaceY(data_Ptr,0)) + .5;
      nend = n * (y_max_temp - SurfaceY(data_Ptr,0)) / (SurfaceY(data_Ptr,n - 1) - SurfaceY(data_Ptr,0));
    }
    if (nstart < 0)
      nstart = 0;
    else if (nstart > n - 1)
      nstart = n - 1;
    if (nend < 0)
      nend = 0;
    else if (nend > n)
      nend = n;
  }
  /****************************/
  /* AutoRescaling the z-axis */

  if (Is_Z_AutoRescale(theWindow) && (n || m)) {
    if (Is_Scatter(data_Ptr)) {
      /* also a problematic case, RT */
      label_Ptr->Z_Max = label_Ptr->Z_Min = 0;
    /* mindgame: Scaling for X and Y is needed, too. */
      y_min_temp = label_Ptr->Y_Min/label_Ptr->Y_Scale;
      y_max_temp = label_Ptr->Y_Max/label_Ptr->Y_Scale;
      x_min_temp = label_Ptr->X_Min/label_Ptr->X_Scale;
      x_max_temp = label_Ptr->X_Max/label_Ptr->X_Scale;
      MinMaxInit = 0;
      /* init of Z_Min, Z_Max postponed, RT */
      i = 0;
      if (!MinMaxInit) { /* find first point in x-range, RT */
        for (; i < n; i++) {
	  xc = ScatterX(data_Ptr,i); yc = ScatterY(data_Ptr,i); 
          if ((xc >= x_min_temp) && (xc <= x_max_temp) &&
	      (yc >= y_min_temp) && (yc <= y_max_temp)) {
	    label_Ptr->Z_Max = label_Ptr->Z_Min = ScatterZ(data_Ptr,i);
            MinMaxInit = 1; break;
	  }
	}
      }
      for (; i < n; i++) { /* find other points, RT */
	xc = ScatterX(data_Ptr,i); yc = ScatterY(data_Ptr,i); 
        if ((xc >= x_min_temp) && (xc <= x_max_temp) &&
	    (yc >= y_min_temp) && (yc <= y_max_temp)) {
	  label_Ptr->Z_Max = max(label_Ptr->Z_Max, ScatterZ(data_Ptr,i));
	  label_Ptr->Z_Min = min(label_Ptr->Z_Min, ScatterZ(data_Ptr,i));
	}
      }
    } else {
      label_Ptr->Z_Max = label_Ptr->Z_Min = SurfaceZ(data_Ptr,mstart,nstart);
      for (i = mstart; i < mend; i++)
	for (j = nstart; j < nend; j++) {
	  label_Ptr->Z_Max = max(label_Ptr->Z_Max, SurfaceZ(data_Ptr,i,j));
	  label_Ptr->Z_Min = min(label_Ptr->Z_Min, SurfaceZ(data_Ptr,i,j));
	}
    }
    if(label_Ptr->Z_Scale < 0.0) {
      temp = label_Ptr->Z_Max;
      label_Ptr->Z_Max = label_Ptr->Z_Min;
      label_Ptr->Z_Min = temp;
    }
    label_Ptr->Z_Max *= label_Ptr->Z_Scale;
    label_Ptr->Z_Min *= label_Ptr->Z_Scale;
    if (Is_Z_Log(theWindow))
      label_Ptr->Z_Min = max(label_Ptr->Z_Min, label_Ptr->Z_Max / 1e6);
  }
  if (Is_Z_Log(theWindow)) {
    label_Ptr->Z_Max = pow(10.0, ceil(log10(max(label_Ptr->Z_Max, DBL_MIN))));
    label_Ptr->Z_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->Z_Min,
							DBL_MIN))));
  }

  *ms = mstart;
  *me = mend;
  *ns = nstart;
  *ne = nend;
}
