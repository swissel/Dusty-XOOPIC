//dielecg.h

#ifndef   __DIELECG_H
#define   __DIELECG_H

#include "part1g.h"
#include "boundg.h"
#include "gridg.h"
#include "dielectr.h"

class DielectricParams;

//-----------------------------------------------
// SecondaryParams class: provides secondary emission capability
// this can occur multiple times per boundary, representing multiple 
// emission phenomena

class SecondaryGroup
{
 protected:
  Scalar secondary;
  Scalar threshold;
  Scalar Eemit;
  ostring secSpeciesName; // must retain names, Species may not be read in yet
  ostring iSpeciesName;
 public:
  SecondaryGroup(Scalar _secondary, Scalar _threshold, Scalar _Eemit, 
		 ostring _secSpeciesName, ostring _iSpeciesName);
  //dad{begin} add a virtual destructor which does nothing
  virtual ~SecondaryGroup() {}
  //dad{end}
  void checkRules(oopicList<ostring> &ruleMessages, SpatialRegionGroup* srgPtr);
  virtual Secondary* CreateCounterPart(SpatialRegionGroup* srgPtr);
};

class Secondary2Group : public SecondaryGroup
{
 protected:
  Scalar fReflected;
  Scalar fScattered;
  // Scalar energy_w;
  Scalar energy_max0;
  Scalar ks;
 public:
  Secondary2Group(Scalar secondary, Scalar threshold, Scalar Eemit, ostring
		  secSpeciesName, ostring iSpeciesName, Scalar fReflected,
		  Scalar fScattered, /*Scalar energy_w,*/ Scalar energy_max0,
		  Scalar ks);
  //dad{begin} add an empty destructor
  ~Secondary2Group() {}
  //dad{end}
  virtual Secondary* CreateCounterPart(SpatialRegionGroup* srgPtr);
};

class SecondaryParams : public ParameterGroup
{
 protected:
  ScalarParameter secondary; //	secondary emission coeff.
  ScalarParameter threshold; // threshold for emission;
  ScalarParameter Eemit;     // max emission energy;
  StringParameter secSpeciesName; // secondary species to emit;
  StringParameter iSpeciesName;	// species which triggers sec;
  DielectricParams* boundary; // attach to boundary
 public:
  SecondaryParams();
  virtual ostring InitializeFromStream(std::ifstream &fin);
  void setBoundary(DielectricParams *_boundary) {boundary = _boundary;}
};

class Secondary2Params : public SecondaryParams
{
  ScalarParameter fReflected; // fraction of reflected primaries;
  ScalarParameter fScattered; // fraction of scattered primaries;
  //  ScalarParameter energy_w; // max value of energy emission function;
  ScalarParameter energy_max0; // energy at which sec. coeff = secondary;
  ScalarParameter ks; // smoothness, 0<=ks<=2, 2 is smoother;
 public:
  Secondary2Params();
  virtual ostring InitializeFromStream(std::ifstream &fin);
};

//=================== DielectricParams Class

class DielectricParams : public BoundaryParams
{
protected:
  ScalarParameter er;
  IntParameter QuseFlag;
  ScalarParameter reflection; // reflected particle fraction
  ScalarParameter refMaxE; // reflect when E < refMaxE
  ScalarParameter transparency;
  SecondaryParams secondaryParams;
  Secondary2Params secondary2Params;
  oopicList<SecondaryGroup> secondaryGroupList;	
public:
  DielectricParams(GridParams *GP, SpatialRegionGroup* srg);
  virtual ~DielectricParams();
  void set_commonParams(Boundary* B);
  virtual void addSecondaries(Boundary* B);
  virtual void addSecondaryGroup(SecondaryGroup* sGroup);
  virtual void checkRules();
  virtual void checkSecondaryRules();
  virtual Boundary* CreateCounterPart();
};

//=================== DielectricRegionParams Class
//	Creates a region filled with a dielectric material
// (j1,k1), (j2,k2) specifies a rectangular region, not a line segment.

class DielectricRegionParams : public DielectricParams
{
public:
	DielectricRegionParams(GridParams *GP, SpatialRegionGroup *srg);
	virtual void checkRules();
	virtual Boundary* CreateCounterPart();
	virtual void makeSegmentFitGrid(LineSegment *S, Grid *G);
};

//=================== DielectricTriangle Class
//	Creates a triangular region filled with a dielectric material
// (j1,k1), (j2,k2) specifies a rectangular region, not a line segment.
// if the normal > 0, the region above the defined boundary is filled
// with dielectric, the opposite is true if normal < 0.

class DielectricTriangleParams : public DielectricParams
{
public:
	DielectricTriangleParams(GridParams *GP, SpatialRegionGroup *srg) 
: DielectricParams(GP,srg) {};
	virtual Boundary* CreateCounterPart();
};

#endif  //  __DIELECG_H
