/* C Test Code for XGrafix. */



/*Note that SCALAR should be defined as "SCALAR" in XGrafix for this test*/
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "../src/xgrafix.h"

#define SIZE  21
#define False 0
#define True  1
#define HISTMAX 100

double          t;
int             index_;
int             nx, ny, nx_irr, ny_irr;
SCALAR           xg[31], yg[31], **zg, **wg, **vg;
SCALAR           yg1[31], yg2[31], yg3[31];
SCALAR           **x_irr, **y_irr, **z_irr;
SCALAR           dx, dy;
int             openFlag1, openFlag2, openFlag3, openFlag4;

int		       testInt = 100, oldtestInt;
SCALAR		       testFloat = 101.0, oldtestFloat;
double	       testDouble = 102.0, oldtestDouble;
char		       testChar = 'a', oldtestChar;

struct data_struct {
  SCALAR x;
  SCALAR y;
};
struct data_struct *data_array;

void Setup(void);
void InitVars(void);

void Setup()
{
  int             i, j;
  double          r, theta;

  oldtestInt = testInt;
  oldtestFloat = testFloat;
  oldtestDouble = testDouble;
  oldtestChar = testChar;

  nx = SIZE;
  ny = SIZE;
  dx = 2*M_PI / nx;
  dy = M_PI / ny;

/* The 2-d plot stuff */
  for (i = 0; i < nx; i++) {
    xg[i] = i*dx;
    yg1[i] = cos(2*i*dy);
    yg2[i] = 1e3*exp(-i/5.0);
    yg3[i] = sin(2*i*dy);
  }

/* The regular 3-d plot stuff */  
  zg = (SCALAR **) malloc(nx * sizeof(SCALAR *));
  wg = (SCALAR **) malloc(nx * sizeof(SCALAR *));
  vg = (SCALAR **) malloc(nx * sizeof(SCALAR *));
  for (i = 0; i < nx; i++) {
    zg[i] = (SCALAR *) malloc(ny * sizeof(SCALAR));
    wg[i] = (SCALAR *) malloc(ny * sizeof(SCALAR));
    vg[i] = (SCALAR *) malloc(ny * sizeof(SCALAR));
    for (j = 0; j < ny; j++) {
      yg[j] = j * dy;
      zg[i][j] = sin(i*dx)*cos(j*dy);
      wg[i][j] = 0.25*cos(i*dx);
      vg[i][j] = 1e3*exp(-i*dx)*cos(0.5*M_PI*j*dy);
    }
  }
    

/* the irregular 3-d plot stuff */
  nx_irr = 31;
  ny_irr = 31;
  x_irr = (SCALAR **)malloc((nx_irr+1)*sizeof(SCALAR *));
  y_irr = (SCALAR **)malloc((nx_irr+1)*sizeof(SCALAR *));
  z_irr = (SCALAR **)malloc((nx_irr+1)*sizeof(SCALAR *));
  for (i=0; i<nx_irr; i++)
  {
    x_irr[i] = (SCALAR *)malloc((ny_irr+1)*sizeof(SCALAR));
    y_irr[i] = (SCALAR *)malloc((ny_irr+1)*sizeof(SCALAR));
    z_irr[i] = (SCALAR *)malloc((ny_irr+1)*sizeof(SCALAR));
    for (j=0; j<ny_irr; j++)
    {
      r     = 1.0*i/(nx_irr-1.0);
      theta = j*2*M_PI/(ny_irr-1);
      x_irr[i][j] = r*cos(theta);
      y_irr[i][j] = r*sin(theta);
      z_irr[i][j] = cos(0.5*M_PI*r)*(1 +.5*r*cos(3*theta));
    }
  }

/* the curve-vector stuff */
  data_array = (struct data_struct *) malloc(HISTMAX * sizeof(struct data_struct));
  for(i=0; i<HISTMAX; i++) {
    data_array[i].x = i;
    data_array[i].y = cos(i*.1);
  }
}


void InitWindows(int argc, char **argv)
{
  XGInit(argc,argv,&t);

/* Initialize an XGrafix window for an X-Y line curve plot. */
  XGSet2D("linlin", "X Axis", "X-Y Line Plot", "closed", 150, 450, 1.0, 
	   1.0, False, False, xg[0], xg[nx-1], -1.0, 1.0);
  XGCurve(xg, yg3, &nx, 1);
  XGCurve(xg, yg1, &nx, 0);
 
/* Initialize an XGrafix window for an X-Y line curve plot with structures. */
  XGSet2DFlag("linlin", "X Axis", "X-Y Line Plot & Structures", "closed", 
   150, 150, 1.0, 1.0, False, False, xg[0], xg[nx-1],-1.0,1.0, &openFlag1);
  XGStructure(4, HOLLOW, 0, 2, 1.0, 0.0, 1.0, 0.5, 0.5, 0.5, 0.0, 0.0);
  XGStructure(4, FILLED, 1, 3, 0.0, -0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5);
  XGCurve(xg, yg1, &nx, 5);
 
/* Initialize an Xgrafix window for an X-Y semi-log plot */
  XGSet2D("linlog", "X Axis", "X-Y Semi-Log Plot", "closed", 350, 450, 1.0, 1.0,
          False, True, xg[0],xg[nx-1], 1.0, 1.0);
  XGCurve(xg, yg2, &nx, 0);  

/* Initialize an Xgrafix window for an X-Y scatter plot. */
  XGSet2D( "linlin", "X Axis", "X-Y Scatter Plot", "closed", 250, 550, 1.0, 1.0,
          False, False, xg[0],xg[nx-1],-1.0, 1.0);
  XGScat2D(xg, yg1, &nx, 3);                  

/* Initialize an XGrafix window for a 2-D vector plot with structures. */
  XGSetVecFlag("vecvec", "X Axis", "Y Axis", "2D Vector Plot & Structures",
  "closed", 600, 175, 1.0, 1.0, False, False, xg[0],xg[nx-1],yg[0],yg[ny-1],
   &openFlag2);
  XGVector(xg, yg, zg, wg, &nx, &ny, 3);
  XGStructure(4, FILLED, 1, 2, -1.0, 0.5, 1.5, 1.0, 0.0, 0.0, 1.0, 0.5);
  XGStructure(4, HOLLOW, 0, 3, 1.0, 0.0, 1.0, 0.5, 0.5, 0.5, 0.0, 0.0);

/* Initialize an Xgrafix window for a 3-D Surface plot. */
  XGSet3DFlag("linlinlin", "X Axis", "Y Axis", "3D Surface Plot", 45.0, 45.0,
  "open", 10, 15, 1.0, 1.0, 1.0, False,False,True, xg[0],xg[nx-1],yg[0],yg[ny-1],
   1.0,1.0, &openFlag3);
  XGSurf(xg, yg, vg, &nx, &ny, 3);

/* Initialize an Xgrafix window for a 3-D irregular surface plot. */
  XGSet3D("linlinlin","X Axis","Y Axis","3D Irregular Surface Plot",
          45.0, 225.0, "closed", 100, 175, 1.0, 1.0, 1.0, False,False,False,
          -1.0, 1.0, -1.0, 1.0, 0.0, 1.05);
  XGIRSurf(x_irr, y_irr, z_irr, &nx_irr, &ny_irr, 3);

/* Initialize an Xgrafix window for a 2-D contour plot. */
  XGSet2DCFlag("linlinlin","X Axis","Y Axis","2D Contour Plot","closed",100,100,
	  1.0,1.0,1.0,False,False,True,xg[0],xg[nx-1],yg[0],yg[ny-1],0.0,0.0, &openFlag4);
  XGCont(xg,yg,z_irr,&nx,&ny,3);

/* Initialize an Xgrafix window for a Curve Vector plot */
  XGSet2D("linlin", "t", "Cos(t) CurveVector Plot", "closed", 150, 450, 1.0, 1.0,
          True, True, 1.0, 1.0, 1.0, 1.0);
  XGCurveVector((SCALAR *) data_array, (SCALAR *) data_array,  &index_, 0, 2, 0, 2, 1);
}


void InitVars()
{
  SetUpNewVar((void*)&testInt,"Test Int","integer");
  SetUpNewVar((void*)&testFloat,"Test Float","float");
  SetUpNewVar((void*)&testDouble,"Test Double","double");
  SetUpNewVar((void*)&testChar,"Test Char","char");
}

int main(int argc, char **argv)
{
  putenv("XGTCL = ../src");
  index_=90;
  t=0.0;

  Setup();
  InitWindows(argc,argv);
  InitVars();
  XGStart(); 
  return 0;
}

/* Do your physice here. */
void XGMainLoop()
{
  int             i, j;
  double           r, theta;
  
  t += 1.0/8.0;

  for (i = 0; i < nx; i++) {
    yg1[i] = cos(2*i*dy)*cos(M_PI*0.1*t);
  } 

  for (j = 0; j < ny; j++)
    for (i = 0; i < nx; i++) {
      zg[i][j] = zg[i][j]*cos(0.01*t*M_PI)/cos(0.01*(t-1)*M_PI);
 	   vg[i][j] = 1e3*exp(-i*dx)*cos(t*0.5*M_PI*j*dy);
    }
  
  for (i=0; i<nx_irr; i++)
    for (j=0; j<ny_irr; j++) {
      r     = 1.0*i/(nx_irr-1.0);
      theta = j*2*M_PI/(ny_irr-1);
      z_irr[i][j] = cos(0.5*M_PI*r)*(1 +.5*r*cos(3*theta +M_PI*t));
    }

  data_array[index_].x=(SCALAR) t;
  data_array[index_].y= cos((SCALAR) 5*t);
  index_++;
  if(index_>=HISTMAX) index_ -= HISTMAX;

  /* Test if dynamic variable changing works. */
  if (oldtestInt != testInt) { 
      printf("New value of testInt = %d\n", testInt);
      oldtestInt= testInt;
  }
  if (oldtestFloat != testFloat) {
      printf("New value of testFloat = %f\n",testFloat);
      oldtestFloat = testFloat;
  }
  if (oldtestDouble != testDouble){
      printf("New value of testDouble = %f\n",testDouble);
      oldtestDouble = testDouble;
  }
  if (oldtestChar != testChar) {
     printf("New value of testChar = %c\n", testChar);
     oldtestChar = testChar;
  }
  
}

void Dump(char *filename)
{
  printf("Dumping to file %s\n",filename);
}

void Quit()
{
  printf("Quitting\n");
}
