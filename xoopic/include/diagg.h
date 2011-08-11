//diagg.h

#ifndef   __DIAGG_H
#define   __DIAGG_H

#ifdef UNIX
#include <config.h>
#endif

#include "gridg.h"
#include "newdiag.h"

class SpatialRegionGroup;

//===================DiagParams  Class
// base class for all diagnostics
// abstract

class DiagParams : public BoundaryParams
{protected:
 GridParams* GP;
 SpatialRegionGroup* spatialRegionGroup;
 Diag* dPtr;
 SpatialRegion* region;
 

public:
 IntParameter nfft;
 IntParameter save;
 IntParameter HistMax;
 IntParameter Ave;
 IntParameter Comb;
 StringParameter VarName;
 StringParameter integral;
 StringParameter x1Label;
 StringParameter x2Label;
 StringParameter x3Label;
 StringParameter title;

 /** 
  * The variables:
  * StringParameter polarizationEB;
  * IntParameter psd1dFlag; 
  * StringParameter windowName;
  * are associated with the extention of the the
  * WaveDirDiag class for handling different linear 
  * combinations of the E and B components together 
  * with the corresponding power spectral densities.
  * psd1dFlag is for the turning on/off the 1d 
  * power spectral density calculation of the 
  * corresponding linear combinations of E and B
  * specified by the "polarizationEB" parameter.
  * psd1dFlag == 0 turns off this calculation. 
  * windowName == "Blackman" is the default for windowing 
  *                data in the 1d power spectrum calculation.
  * dad, Mon Aug 13 10:49:47 MDT 2001 
  */
 StringParameter polarizationEB;
 IntParameter psd1dFlag; 
 StringParameter windowName;

 /**
  * Variables associated with the PSDFieldDiag:
  */
 StringParameter fieldName; // E or B
 IntParameter fieldComponentLabel; // 1, 2, or 3 

 BOOL HasLength();

	  DiagParams(GridParams *_GP,SpatialRegionGroup* srg);
	  //  since boundaries are all placed on grid, this class needs
	  //  to know the dimensions of the grid

	  virtual ~DiagParams() {};

	  //ostring AlignmentConstraint();
	  // ensures that boundary is aligned with grid

	//  ostring OnGridConstraint();
	  // ensures that boundary is on grid

	  virtual void checkRules();
	  // checks all rules associated with the class

	  virtual Diag* CreateCounterPart(SpatialRegion* region) throw(Oops);

// void toGrid(Grid *g);

	// query function for GUI
#ifndef UNIX
	// Segment getSegment();
	// void setSegment(Segment& s);
#endif
};                                       

#endif  //  __DIAGG_H

