#ifndef	__NEWDIAG_H
#define	__NEWDIAG_H

/*
====================================================================

NEWDIAG.H

The pure virtual base class that represents a generic diagnostic object.
All diagnostic objects (e.g., point, line) must inherit from this class.


1.01	(PG, KC, 9-21-95) After release. First draft.
1.02	(JohnV 03-08-96) Added initwin(params...) for PC version

====================================================================
*/

#include "sptlrgn.h"

extern "C" {
void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label,
             const char *State, int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
             int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
             Scalar X_Max, Scalar Y_Min, Scalar Y_Max);
}

class GUIConfig;

//--------------------------------------------------------------------
//	Diag:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class History;
class Diagnostics;

class Diag
{
protected:
	SpatialRegion* region;
	Diagnostics *theDiagnostics;
	History* history;
	int j1, k1, j2, k2;
	Scalar A1,A2,B1,B2;
	int nfft, HistMax, Ave, Comb;
	ostring VarName, x1Label, x2Label, x3Label, title;
	int save;

public:
#ifdef HAVE_HDF5
	virtual void dumpDiagH5(dumpHDF5 *dumpObj){}
#endif
	ostring getVarName(){return VarName;}
	Diag(SpatialRegion* SR, int j1, int k1, int j2, int k2, int nfft, int HistMax,
				 int Ave, int Comb, ostring VarName, ostring x1Label,
				 ostring x2Label, ostring x3Label, ostring title, int save);
	virtual ~Diag(){};
	virtual int Restore_2_00(FILE *DMPFile, int j1, int k1, int j2, int k2, ostring title);
	virtual int Restore(FILE *DMPFile,Scalar A1,Scalar A2,Scalar B1,Scalar B2,
							  int xl,int yl, const char *title);
#ifdef HAVE_HDF5
	virtual int dumpH5(dumpHDF5 &dumpObj,int number);
#endif
	virtual int Dump(FILE *DMPFile);
	virtual void MaintainDiag(Scalar t)=0;
	void setDiagnostics(Diagnostics *_theDiagnostics) { theDiagnostics=_theDiagnostics;};
	virtual void initwin()=0;
	virtual void initwin(GUIConfig* gui, int itemNumber) {}
/**
	virtual void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label, 
             const char *State, int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
             int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
             Scalar X_Max, Scalar Y_Min, Scalar Y_Max);
*/
	int get_j1() { return j1;};
	int get_j2() { return j2;};
	int get_k1() { return k1;};
	int get_k2() { return k2;};
};

#endif	//	ifndef __NEWDIAG_H

