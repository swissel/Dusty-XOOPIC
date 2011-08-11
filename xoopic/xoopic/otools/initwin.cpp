//  This initializes the windows for diagnostics.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "sptlrgn.h"

#include <stdlib.h>
#include "oopiclist.h"
#include "ovector.h"
#include "diagn.h"
#include "ptclgrp.h"
#include "globalvars.h"
#include "boundary.h"
#include "gpdist.h"
#include "history.h"

extern "C" {
#include <math.h>
#include <stdio.h>
//#include <malloc/malloc.h>

#ifdef NOX
#include <xgmini.h>
#else
#include <xgrafix.h>
void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label,
             const char *State, int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
             int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
             Scalar X_Max, Scalar Y_Min, Scalar Y_Max);
#endif
}

#define True 1
#define False 0

//  These are xgrafix flags!
//  They turn on/off the copying of the diagnostics from the pic
//  structures to the arrays that xgrafix must have.


void Diagnostics::InitWin(void) {
  // initialize the U ostrings 
  ostring U1, U2, U3;
  ostring U="u";
  //  We'll put the diagnostic calls to xgrafix here.

  jm = theSpace->getJ()+1;
  km = theSpace->getK()+1;
  km2 = km/2+1;

  jmNGD = theSpace->getJ();
  kmNGD = theSpace->getK();

  ostring prepend="";
  int i, isp;
	if(RegionName != (ostring)"Noname" && RegionName != (ostring)"") 
		prepend = RegionName + (ostring)": ";


  switch(theSpace->get_grid()->query_geometry())
	 {
	 case ZRGEOM:
		{
		  X1="z";
		  X2="r";
		  X3="phi";
		  K1="kz";
		  K2="kr";
                  U1=U+X1;
                  U2=U+X2;
                  U3=U+X3;
		  E1= prepend + "Ez";
		  E2= prepend + "Er";
		  E3= prepend + "Ephi";
		  B1= prepend + "Bz";
		  B2= prepend + "Br";
		  B3= prepend + "Bphi";
		  I1= prepend + "Iz";
		  I2= prepend + "Ir";
		  I3= prepend + "Iphi";
		  break;
		}
	 case ZXGEOM:
		{
		  X1="x";
		  X2="y";
		  X3="z";
		  K1="kx";
		  K2="ky";
                  U1=U+X1;
                  U2=U+X2;
                  U3=U+X3;
		  E1= prepend + "Ex";
		  E2= prepend + "Ey";
		  E3= prepend + "Ez";
		  B1= prepend + "Bx";
		  B2= prepend + "By";
		  B3= prepend + "Bz";
		  I1= prepend + "Ix";
		  I2= prepend + "Iy";
		  I3= prepend + "Iz";
		  break;
		}
	 };			
  char buf1[100];
  sprintf(buf1,"u%s vs %s, all species",X1.c_str(),X1.c_str());
  U1vsX1 =   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X2.c_str(),X1.c_str());
  U2vsX1 =   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X3.c_str(),X1.c_str());
  U3vsX1 =   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X1.c_str(),X2.c_str());
  U1vsX2=   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X2.c_str(),X2.c_str());
  U2vsX2=   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X3.c_str(),X2.c_str());
  U3vsX2=   prepend + buf1;

#ifndef NOX
  //  Set up the diagnostics 
  //----------------------------------------------------------------------//
  // Set up the new diagnostics

  DiagList *dlist = theSpace->getDiagList();
	oopicListIter<Diag> nextd(*dlist);
  for(nextd.restart();!nextd.Done(); nextd++) 
		nextd.current()->initwin();

  //----------------------------------------------------------------------//
  //set up electric fields
  XGSet3D( "linlinlin",X1,X2,E1,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,0);

  XGSet3D( "linlinlin",X1,X2,E2,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,1);

  XGSet3D( "linlinlin",X1,X2,E3,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,2);

#ifdef DEBUG_FIELDS
  XGSet3D( "linlinlin",X1,X2,"intEdl1",45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)intEdl, &jm, &km, 1 ,1,0,1,0,3,0);

  XGSet3D( "linlinlin",X1,X2,"intEdl2",45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)intEdl, &jm, &km, 1 ,1,0,1,0,3,1);

  XGSet3D( "linlinlin",X1,X2,"intEdl3",45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)intEdl, &jm, &km, 1 ,1,0,1,0,3,2);
#endif
  //set up magnetic fields

  XGSet3D( "linlinlin",X1,X2,B1,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,0 );

  XGSet3D( "linlinlin",X1,X2,B2,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,1 );

  XGSet3D( "linlinlin",X1,X2,B3,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,2 );

#ifdef DEBUG_FIELDS
  XGSet3D( "linlinlin",X1,X2,"intBdS1",45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)intBdS, &jm, &km, 1 ,1,0,1,0,3,0);

  XGSet3D( "linlinlin",X1,X2,"intBdS2",45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)intBdS, &jm, &km, 1 ,1,0,1,0,3,1);

  XGSet3D( "linlinlin",X1,X2,"intBdS3",45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)intBdS, &jm, &km, 1 ,1,0,1,0,3,2);
#endif

  //set up currents
  if(!electrostaticFlag) {
	 XGSet3D( "linlinlin",X1,X2,I1,45.0,225.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,0 );

	 XGSet3D( "linlinlin",X1,X2,I2,45.0,225.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,1 );

	 XGSet3D( "linlinlin",X1,X2,I3,45.0,225.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,2 );
  }
  //set up energies

  XGSet3D( "linlinlin",X1,X2,strdup((prepend +"Ue")),45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,Ue, &jm, &km, 1 );

  XGSet3D( "linlinlin",X1,X2,strdup((prepend +"Ub")),45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,Ub, &jm, &km, 2 );

  // Set up poynting vector
  if(!electrostaticFlag)
	 {
		XGSetVec("vecvec",X1,X2,strdup(prepend+"Poynting Vector"),"closed",1,1,1.0
					,1.0,0,0,x1min,x1max,x2min,x2max);

		XGVectorVector(x1_array,x2_array,(Scalar **)S_array,
							(Scalar **)S_array,&jm,&km,2,1,0,1,0,3,0,3,1);

	 }


  //  set up rho
	char buffer[100];
	if (boltzmannFlag)
		sprintf(buffer,"PIC ");
	else 
		sprintf(buffer,"%s","");

  XGSet3D( "linlinlin",X1,X2,strdup(prepend + buffer + "rho"),45.0,250.0,"closed",1,400,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,rho, &jm, &km, 3 );

  /** 
   *  Plot the neutral gas densities, dad: 01/24/2001
   */ 
  oopicListIter<NGD> NGDIter(*ptrNGDList);
  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) {
    XGSet3D( "linlinlin",X1,X2,
               strdup(prepend + "Neutral Gas Density for " + (NGDIter.current())->getGasType()),
               45.0,250.0,"closed",1,400,
               1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
    XGSurf( x1_arrayNGD,x2_arrayNGD, (NGDIter.current())->getNGDdata(), &jmNGD, &kmNGD, 3 );
  }
  
  //  set up boltzmann rho
  if (boltzmannFlag)
    {
      XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Total Rho"),45.0,250.0,"closed",1,1,
               1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
      XGSurf( x1_array,x2_array,totalRho, &jm, &km, 3 );
    }
  if(electrostaticFlag) {
    //  set up phi
    XGSet3D( "linlinlin",X1,X2,strdup(prepend +"phi"),45.0,250.0,"closed",1,1,
             1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
    XGSurf( x1_array,x2_array,phi, &jm, &km, 3 );
  }
#ifdef DEBUG_PHI
  XGSet3D( "linlinlin",X1,X2,strdup(prepend +"phi error"),45.0,250.0,"closed",1,1,
           1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,phi_err, &jm, &km, 3 );
#endif
  //  set up divderror
  if(!electrostaticFlag) {
    XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Divergence Error"),45.0,250.0,"closed",1,1,
             1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
    XGSurf( x1_array,x2_array,divderror, &jm, &km, 3 );
  }
  
  //  The phase space diagnostics
  //	X2 vs X1
  for(isp=0;isp<number_of_species;isp++) {
    char * buf = new char[80];
    sprintf(buf,"%s-%s phase space for %s ",X1.c_str(),X2.c_str(),theSpecies[isp].name);
    
    WinSet2D( "linlin", X1, X2, strdup(prepend +buf),"open"
              ,700,400,1,1,0,0,x1min,x1max,x2min,x2max);
    delete [] buf; 
    
    XGScat2D(theSpecies[isp].x1positions,theSpecies[isp].x2positions,
             &(theSpecies[isp].nparticles_plot),isp);
    //draw any boundaries or structures
    oopicListIter<Boundary> nextb(*blist);
    for(nextb.restart();!nextb.Done(); nextb++) {
      Scalar x1=0.;
      Scalar x2=0.;
      Scalar y1=0.;
      Scalar y2=0.;
      int j1,j2,k1,k2;
      
      if (nextb.current()->queryFill()){
        Boundary ***NB;
        j1=nextb.current()->getMinJ();
        j2=nextb.current()->getMaxJ();
        k1=nextb.current()->getMinK();
        k2=nextb.current()->getMaxK();
        NB=nextb.current()->getFields()->get_grid()->GetCellMask();
        
        int j,k;
        int on=0;
        for (j=j1; j<j2; j++){
          for (k=k1; k<k2; k++){
            if (NB[j][k]==nextb.current()){
              if (!on){
                on=1;
                x1 = x1_array[j]; 
                y1 = x2_array[k];
              }
            }
            else 
              if (on) {
                on=0;
                int bcolor = nextb.current()->getBoundaryType();
                Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
                x2 = x1_array[j+1];
                y2 = x2_array[k];
                fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
                fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
                fpoints[8]=x1;fpoints[9]=y1;
                XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
                free(fpoints);
              }
          }
          if (on){
            on = 0;
            Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
            int bcolor = nextb.current()->getBoundaryType();
            fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
            fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
            fpoints[8]=x1;fpoints[9]=y1;
            x2 = x1_array[j+1];
            y2 = x2_array[k2];
            XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
            free(fpoints);
          }
        }
      }
      
      oopicListIter <LineSegment> lsI(*nextb.current()->getSegmentList());
      for(lsI.restart();!lsI.Done();lsI++) {
        int j1,j2,k1,k2,normal,*points;  //local copies of above
        j1=(int)lsI.current()->A.e1();
        k1=(int)lsI.current()->A.e2();
        j2=(int)lsI.current()->B.e1();
        k2=(int)lsI.current()->B.e2();
        points=lsI.current()->points;
        normal=lsI.current()->normal;
        
        x1 = x1_array[j1]; 
        x2 = x1_array[j2];
        y1 = x2_array[k1];
        y2 = x2_array[k2];
        int bcolor = nextb.current()->getBoundaryType();
        
        // Different sorts of boundaries need to be drawn differently
        switch(bcolor) {
        case DIELECTRIC_TRIANGLE_UP: // positive-normal DielectricTriangle
          {				 
            Scalar * fpoints = (Scalar *) calloc(4*abs(j2-j1+3),sizeof(Scalar));
            int j;
            for(j=0;j<4*(j2-j1)+4;j+=2)
              { fpoints[j] = x1_array[points[j]];
              fpoints[j+1] = x2_array[points[j+1]];
              }
            fpoints[j] = (y1 < y2)? x1 :x2;
            fpoints[j+1] = MAX(y1,y2);
            fpoints[j+2]= fpoints[0];
            fpoints[j+3]= fpoints[1];
            XGStructureArray(2*(j2-j1)+4,FILLED,bcolor - 100, bcolor -100,
                             fpoints);
            
            //				delete [] points;
            free(fpoints);
            
            break;
          }
        case DIELECTRIC_TRIANGLE_DOWN: //negative-normal DielectricTriangle
          {
            Scalar * fpoints = (Scalar *) calloc(4*abs(j2-j1+3),sizeof(Scalar));
            int *points = segmentate(j1,k1,j2,k2);
            int j;
            for(j=0;j<4*(j2-j1)+4;j+=2)
              { fpoints[j] = x1_array[points[j]];
              fpoints[j+1] = x2_array[points[j+1]];
              }
            fpoints[j] = (y1 < y2)? x2 :x1;
            fpoints[j+1] = MIN(y1,y2);
            fpoints[j+2]= fpoints[0];
            fpoints[j+3]= fpoints[1];
            XGStructureArray(2*(j2-j1)+4,FILLED,-(bcolor + 100), -(bcolor +100),
                             fpoints);
            
            delete [] points;
            free(fpoints);
            break;
          }
        case DIELECTRIC_REGION:  //filled square
          {
            Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
            fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
            fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
            fpoints[8]=x1;fpoints[9]=y1;
            XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
            free(fpoints);
            break;
          }
        case CURRENT_REGION:  //filled square
          {
            Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
            fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
            fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
            fpoints[8]=x1;fpoints[9]=y1;
            XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
            free(fpoints);
            break;
          }
        case ILOOP: // Hollow square
          {
            Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
            fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
            fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
            fpoints[8]=x1;fpoints[9]=y1;
            XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
            free(fpoints);
            break;
          }
        case PERIODIC:
          {
            //do nothing
            break;
          }
        default: {   // simple or oblique line
            if(x1==x2 || y1==y2) {
              Scalar xa,ya;
              Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
              xa = (x1+x2)/2.0;
              ya = (y1+y2)/2.0;
              fpoints[0]=x1;fpoints[1]=y1;
              fpoints[2]=x2;fpoints[3]=y2;
              XGStructureArray(2,HOLLOW,bcolor,bcolor,fpoints);
              fpoints[0]=xa;fpoints[1]=ya;
              fpoints[2]=((x1==x2)?xa+(normal*(x1_array[1]-x1_array[0]))/4.0 :xa);
              fpoints[3]=((y1==y2)?ya+(normal*(x2_array[1]-x2_array[0]))/4.0 :ya);
              XGStructureArray(2,HOLLOW,bcolor,bcolor,fpoints);
              free(fpoints);
            }
            else
              {  
                int *points = segmentate(j1,k1,j2,k2);
                Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
                int j, jl,kl,jh,kh;
                /* start with the second point, advance one point at a time */
                for(j=2;j<4*abs(j2-j1)+4;j+=2) {
                  jl=points[j-2];
                  kl=points[j-1];
                  jh=points[j];
                  kh=points[j+1];
                  fpoints[0]=x1_array[jl];fpoints[1]=x2_array[kl];
                  fpoints[2]=x1_array[jh];fpoints[3]=x2_array[kh];
                  XGStructureArray(2,HOLLOW,bcolor,bcolor,fpoints);
                }
                delete[] points;
                free(fpoints);
                
              }
            break;
          }
        }
      }
    }
  }
  
  
  // 	U1 vs X1
  
  WinSet2D( "linlin", X1,strdup(U1.c_str()), U1vsX1,"closed",700,400,1,1,0,1,x1min,x1max,x2min,x2max);
  
  for(i=0;i<number_of_species;i++)
    XGScat2D(theSpecies[i].x1positions,theSpecies[i].x1velocities,
             &(theSpecies[i].nparticles_plot),i);
  
  //	U2 vs X1
  WinSet2D( "linlin", X1,strdup(U2.c_str()),U2vsX1,"closed",700,400,1,1,0,1,x1min,x1max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
    XGScat2D(theSpecies[i].x1positions,theSpecies[i].x2velocities,
             &(theSpecies[i].nparticles_plot),i);
  
  //	U3 vs X1
  WinSet2D( "linlin", X1,strdup(U3.c_str()),U3vsX1,"closed",700,400,1,1,0,1,x1min,x1max,x2min,x2max);
  
  for(i=0;i<number_of_species;i++)
    XGScat2D(theSpecies[i].x1positions,theSpecies[i].x3velocities,
             &(theSpecies[i].nparticles_plot),i);
  
  //	U1 vs X2
  WinSet2D( "linlin", X2,strdup(U1.c_str()),U1vsX2,"closed",700,400,1,1,0,1,x2min,x2max,x2min,x2max);
  
  for(i=0;i<number_of_species;i++)
    XGScat2D(theSpecies[i].x2positions,theSpecies[i].x1velocities,
             &(theSpecies[i].nparticles_plot),i);
  
  //	U2 vs X2
  WinSet2D( "linlin", X2,strdup(U2.c_str()),U2vsX2,"closed",700,400,1,1,0,1,x2min,x2max,x2min,x2max);
  
  for(i=0;i<number_of_species;i++)
    XGScat2D(theSpecies[i].x2positions,theSpecies[i].x2velocities,
             &(theSpecies[i].nparticles_plot),i);
  
  //	U3 vs X2
  WinSet2D( "linlin", X2,strdup(U3.c_str()),U3vsX2,"closed",700,400,1,1,0,1,x2min,x2max,x2min,x2max);
  
  for(i=0;i<number_of_species;i++)
    XGScat2D(theSpecies[i].x2positions,theSpecies[i].x3velocities,
             &(theSpecies[i].nparticles_plot),i);
  
  
  //  vector plot Er,Ez
  
  XGSetVec("vecvec",X1,X2,strdup(prepend +"E"),"closed",1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);
  
  XGVectorVector(x1_array,x2_array,(Scalar **)E,(Scalar **)E,&jm,&km,2,1,0,1,0,3,0,3,1);
  
  //  vector plot Br,Bz
  
  XGSetVec("vecvec",X1,X2,strdup(prepend +"B"),"closed",1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);
  
  XGVectorVector(x1_array,x2_array,(Scalar **)B,(Scalar **)B,&jm,&km,2,1,0,1,0,3,0,3,1);
  
  //  vector plot Ir,Iz
  if(!electrostaticFlag) {
    XGSetVec("vecvec",X1,X2,strdup(prepend +"I"),"closed",1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);

    XGVectorVector(x1_array,x2_array,(Scalar **)I,(Scalar **)I,&jm,&km,2,1,0,1,0,3,0,3,1);
  }

  //  Time domain plots

  //  boundary current history diagnostics
  oopicListIter<Ihistdiag> nextdiag(*BoundaryIhist);
  
  for(nextdiag.restart();!nextdiag.Done();nextdiag++){
    char *buf = new char[80];
    sprintf(buf,"I history for boundary %s",nextdiag.current()->name);
    WinSet2D("linlin","Time","History",strdup(prepend+ buf),"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
    XGCurve(nextdiag.current()->Ihist->get_time_array(),nextdiag.current()->Ihist->get_data(),nextdiag.current()->Ihist->get_hi(),1);
    delete [] buf;
    
    // repeat for species
    char *buf2 = new char[80];
    sprintf(buf2,"I species history for boundary %s",nextdiag.current()->name);
    WinSet2D("linlin","Time","History",strdup(prepend+ buf2),"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
    for (i=0; i<number_of_species; i++){
      XGCurve(nextdiag.current()->Ihist_sp[i]->get_time_array(),nextdiag.current()->Ihist_sp[i]->get_data(),nextdiag.current()->Ihist_sp[i]->get_hi(),i);
    }
    delete [] buf2;
  }
  
  oopicListIter<PFhistdiag> nextdiag2(*BoundaryPFhist);
  
  for(nextdiag2.restart();!nextdiag2.Done();nextdiag2++){
    char *buf = new char[80];
    sprintf(buf,"%sPoynting history for %s",prepend.c_str(),nextdiag2.current()->name);
    WinSet2D("linlin","Time","Poynting History",strdup(buf),"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
    XGCurve(nextdiag2.current()->PFhist->get_time_array(),nextdiag2.current()->PFhist->get_data(),nextdiag2.current()->PFhist->get_hi(),1);
    delete [] buf;
    char *buf2 = new char[80];
    sprintf(buf2,"%sPoynting Local history for %s",prepend.c_str(),nextdiag2.current()->name);
    WinSet2D("linlin","Time","Poynting Local History",strdup(buf2),"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
    XGCurve(nextdiag2.current()->PFlocal->get_time_array(),nextdiag2.current()->PFlocal->get_data(),nextdiag2.current()->PFlocal->get_hi(),1);
    delete [] buf2;
  }

  WinSet2D("linlog", "Time","Poynting History",
	   strdup(prepend + "TE-beige KE-green Efield-yellow Bfield-Orange"),
	   "closed", 800, 200, 1.0, 1.0, True, True,0,0,0.0,0.0); 
  XGCurve(energy_all.get_time_array(), energy_all.get_data(), energy_all.get_hi(), 1);
  XGCurve(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), 2);
  XGCurve(energy_e.get_time_array(), energy_e.get_data(), energy_e.get_hi(), 3);
  XGCurve(energy_b.get_time_array(), energy_b.get_data(), energy_b.get_hi(), 4);

  if(!electrostaticFlag) {
	 WinSet2D("linlin","Time","Error",strdup(prepend + "Average magnitude of error in Div(D) at t"),
				"closed", 800, 200, 1.0, 1.0, True, True,0,0,0.0,0.0);
	 XGCurve(divderrorhis.get_time_array(), divderrorhis.get_data(), divderrorhis.get_hi(),0);
  }

  WinSet2D("linlin", "Time","number(t)",strdup(prepend + "number(t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) {
	 XGCurve(number[i]->get_time_array(), number[i]->get_data(),number[i]->get_hi(), i);
	 //	  XGCurve(ngroups[i]->get_time_array(), ngroups[i]->get_data(),ngroups[i]->get_hi(), i);

  }

  WinSet2D("linlin", "Time","Kinetic Energy",strdup(prepend + "Kinetic Energy(t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) 
		XGCurve(ke_species[i]->get_time_array(), ke_species[i]->get_data(),ke_species[i]->get_hi(), i);
	
  XGCurve(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), i+1);

  WinSet2D("linlin", "Time","total density",strdup(prepend + "total density (t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) 
	 XGCurve(total_density[i]->get_time_array(), total_density[i]->get_data(), total_density[i]->get_hi(), i);

  /**********************************************************/
  // New time history plot for RMS beam parameters
  // Bruhwiler/Dimitrov 10/09/2000
  // search for the name/number for the species with rmsBeamSizeFlag set to 1 (default is 0)
  bool Species_Flag = false;
  oopicListIter<Species> siter(*theSpace->getSpeciesList());
  for(siter.restart();(!siter.Done()) && (!Species_Flag); siter++) {
      if( siter.current()->get_rmsDiagnosticsFlag() ) {
	char* rms_buf = new char[80];
	// Ave for Beam Size
	sprintf(rms_buf,"Averages for %s: %s-beige %s-green",
		static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str());
	WinSet2D("linlin", "Time", "Average Beam Sizes",strdup(prepend + rms_buf), "closed", 
		700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(aveBeamSize[0]->get_time_array(), aveBeamSize[0]->get_data(), aveBeamSize[0]->get_hi(), 1);
	XGCurve(aveBeamSize[1]->get_time_array(), aveBeamSize[1]->get_data(), aveBeamSize[1]->get_hi(), 2);
	// RMS for Beam Size
	sprintf(rms_buf,"RMS' for %s: %s-beige %s-green",
		static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str());
	WinSet2D("linlin", "Time", "RMS Beam Sizes",strdup(prepend + rms_buf), "closed", 
		700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(rmsBeamSize[0]->get_time_array(), rmsBeamSize[0]->get_data(), rmsBeamSize[0]->get_hi(), 1);
	XGCurve(rmsBeamSize[1]->get_time_array(), rmsBeamSize[1]->get_data(), rmsBeamSize[1]->get_hi(), 2);
	// Ave for Velocities
	sprintf(rms_buf,"Ave Velocities for %s: U%s-beige U%s-green U%s-yellow",
		static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str(), X3.c_str());
	WinSet2D("linlin", "Time",  "Average Velocities",strdup(prepend + rms_buf), "closed", 700, 400,
		1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(aveVelocity[0]->get_time_array(), aveVelocity[0]->get_data(), aveVelocity[0]->get_hi(), 1);
	XGCurve(aveVelocity[1]->get_time_array(), aveVelocity[1]->get_data(), aveVelocity[1]->get_hi(), 2);
	XGCurve(aveVelocity[2]->get_time_array(), aveVelocity[2]->get_data(), aveVelocity[2]->get_hi(), 3);
	// RMS for velocities
	sprintf(rms_buf,"Velocity RMS' for %s: U%s-beige U%s-green U%s-yellow",
		static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str(), X3.c_str());
	WinSet2D("linlin", "Time", "RMS Velocities",strdup(prepend + rms_buf), "closed", 
		700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(rmsVelocity[0]->get_time_array(), rmsVelocity[0]->get_data(), rmsVelocity[0]->get_hi(), 1);
	XGCurve(rmsVelocity[1]->get_time_array(), rmsVelocity[1]->get_data(), rmsVelocity[1]->get_hi(), 2);
	XGCurve(rmsVelocity[2]->get_time_array(), rmsVelocity[2]->get_data(), rmsVelocity[2]->get_hi(), 3);
	//
	// RMS for Emittance
	sprintf(rms_buf,"RMS Emittance for %s: E%s-beige E%s-green",
		static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str());
	WinSet2D("linlin", "Time", "RMS Emittance",strdup(prepend + rms_buf), "closed", 
		700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(rmsEmittance[0]->get_time_array(), rmsEmittance[0]->get_data(), rmsEmittance[0]->get_hi(), 1);
	XGCurve(rmsEmittance[1]->get_time_array(), rmsEmittance[1]->get_data(), rmsEmittance[1]->get_hi(), 2);
	// ave for Energy in eV
	/**************************************************
	sprintf(rms_buf,"Ave Energy for %s", static_cast<char*>(siter.current()->get_name()));
	WinSet2D("linlin", "Time", "Average Energy", strdup(prepend + rms_buf), "closed", 700, 400,
		1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(aveEnergy_eV->get_time_array(), aveEnergy_eV->get_data(), aveEnergy_eV->get_hi(), 1);
	// rms for Energy in eV
	sprintf(rms_buf,"RMS Energy for %s", static_cast<char*>(siter.current()->get_name()));
	WinSet2D("linlin", "Time", "RMS Energy", strdup(prepend + rms_buf), "closed", 700, 400,
		1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(rmsEnergy_eV->get_time_array(), rmsEnergy_eV->get_data(), rmsEnergy_eV->get_hi(), 1);
	***************************************************/
	Species_Flag = true;
	delete [] rms_buf;
      }
  }
  //end of Dimitrov/Bruhwiler changes
  /**********************************************************/

  WinSet2D("linlin", "Time","Ave KE (eV)", strdup(prepend + "Ave KE(t)"), 
	"closed", 700, 400, 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) 
	 XGCurve(Ave_KE[i]->get_time_array(), Ave_KE[i]->get_data(), Ave_KE[i]->get_hi(), i);

  //  set u the special button

  SetUpNewVar(&PhaseSpacePlots,(char *)"Phase Plots\0",(char *)ScalarChar);
  SetUpNewVar(&AllDiagnostics,(char *)"All Diags\0",(char *)ScalarChar);
  SetUpNewVar(&EnerPlots,(char *)"Energy Diags\0",(char *)ScalarChar);

// Give us nice species diagnostics.
  for(i=0;i<number_of_species;i++) {
    char * buf = new char[80];
    sprintf(buf,"%sNumber density for %s ", (char *)(prepend), 
	theSpecies[i].name);
    XGSet3D("linlinlin", X1, X2, strdup(buf), 45.0, 225.0, "closed", 1, 1, 
	1.0, 1.0, 1.0/theSpecies[i].spec->get_q(), 0, 0, 1, x1min, x1max, 
	x2min, x2max, 0.0, 1.0);
    XGSurf(x1_array, x2_array, rho_species[i], &jm, &km, 3 );
    delete [] buf;
  }


  for(i=0;i<number_of_species;i++) {
      if (Show_loaded_densityFlag)
        {
          char * buf = new char[80];
          sprintf(buf,"%sInitial number density for %s ",prepend.c_str(),theSpecies[i].name);
          
          XGSet3D( "linlinlin",X1,X2,strdup(buf),45.0,225.0,"closed",1,1,
                   1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
          XGSurf(x1_array,x2_array,loaded_density[i], &jm, &km, 3 );
          delete [] buf;
        }
    }
  
  /*  Boundary diagnostics */
  oopicListIter<Boundary> nextb(*blist);
  for(nextb.restart();!nextb.Done(); nextb++) {
    Boundary *B = nextb.current();
    PDistCol *PD = B->get_particle_diag();
    if (PD) if (PD->get_nxbins()>0) {
      ostring bid = B->getBoundaryName();

      char *buf1 = new char[80];
      sprintf(buf1, "nxpoints=%d for boundary %s\n", *PD->get_nxpoints(), bid.c_str());
      printf("%s", buf1);
      delete [] buf1;

      char *buf = new char[80];
      sprintf(buf,"%sf(x) on boundary %s", prepend.c_str(), bid.c_str());
      WinSet2D("linlin", "x", "sf(x)", strdup(buf), "closed", 200, 200,
               1.0, 1.0, False, True, PD->get_x_MIN(), PD->get_x_MAX(), 0.0, 0.0);
      delete [] buf;

      for(i=0;i<number_of_species;i++)
        XGCurve(PD->get_x(), PD->get_fxdata(i), PD->get_nxpoints(), i);
      if (*PD->get_nepoints()) {
        for (i=0; i<number_of_species; i++){
          char *buf = new char[80];
          sprintf(buf, "%sf(x,energy) for %s on boundary %s", prepend.c_str(), 
                  theSpecies[i].name, B->getBoundaryName().c_str());
          XGSet3D("linlinlin", "x", "energy", strdup(buf), 45, 225, "closed", 1, 1,
                  1, 1, 1, 0, 0, 1, PD->get_x_MIN(), PD->get_x_MAX(), 
                  PD->get_energy_MIN(), PD->get_energy_MAX(), 0, 1);
          XGSurf(PD->get_x(), PD->get_energy(), PD->get_fedata(i), PD->get_nxpoints(),
                 PD->get_nepoints(), i);
          delete [] buf;
        }
      }
      if (*PD->get_ntpoints()){
        for (i=0; i<number_of_species; i++){
          char *buf = new char[80];
          sprintf(buf, "%sf(x,theta) for %s on boundary %s", prepend.c_str(), 
                  theSpecies[i].name, B->getBoundaryName().c_str());
          XGSet3D("linlinlin", "x", "theta", strdup(buf), 45, 225, "closed", 1, 1,
                  1, 1, 1, 0, 0, 1, PD->get_x_MIN(), PD->get_x_MAX(), 
                  PD->get_theta_MIN(), PD->get_theta_MAX(), 0, 1);
          XGSurf(PD->get_x(), PD->get_theta(), PD->get_ftdata(i), PD->get_nxpoints(),
                 PD->get_ntpoints(), i);
          delete [] buf;
        }
      }
      // add theta and energy plots here
    }
  }
#endif
}

