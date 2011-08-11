//dielecg.cpp

#include "dielecg.h"
#include "secondary.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}
using namespace std;
//=================== SecondaryGroup

SecondaryGroup::SecondaryGroup(Scalar _secondary, Scalar _threshold, Scalar 
  _Eemit, ostring _secSpeciesName, ostring _iSpeciesName)
{
  secondary = _secondary; 
  threshold = _threshold; 
  Eemit = _Eemit; 
  secSpeciesName = _secSpeciesName; 
  iSpeciesName = _iSpeciesName;
}

void SecondaryGroup::checkRules(oopicList<ostring>& ruleMessages, 
				SpatialRegionGroup *spatialRegionGroup)
{
  if (secondary > 0) {
    if (secSpeciesName == (ostring)"NULL")
      ruleMessages.add(new ostring("secSpeciesName is required"));
    else if (!spatialRegionGroup->KnownSpecies(secSpeciesName)){
      char buffer[512];
      sprintf(buffer, "Unrecognized secSpeciesName: %s", secSpeciesName.c_str());
      ruleMessages.add(new ostring(buffer));
    }
    if (iSpeciesName == (ostring)"NULL")
      ruleMessages.add(new ostring("iSpeciesName is required when secondary > 0"));
    else if (!spatialRegionGroup->KnownSpecies(iSpeciesName)){
      char buffer[512];
      sprintf(buffer, "Unrecognized iSpeciesName: %s", iSpeciesName.c_str());
      ruleMessages.add(new ostring(buffer));
    }
  }
}

Secondary* SecondaryGroup::CreateCounterPart(SpatialRegionGroup* srgPtr)
{
  Species* secSpeciesPtr = srgPtr->get_speciesPtr(secSpeciesName);
  Species* iSpeciesPtr = srgPtr->get_speciesPtr(iSpeciesName);
  return new Secondary(secondary, threshold, Eemit, secSpeciesPtr, iSpeciesPtr);
}

Secondary2Group::Secondary2Group(Scalar secondary, Scalar threshold, Scalar 
  Eemit, ostring secSpeciesName, ostring iSpeciesName, Scalar _fReflected,
				 Scalar _fScattered, /*Scalar _energy_w,*/ 
				 Scalar _energy_max0, Scalar _ks)
:  SecondaryGroup(secondary, threshold, Eemit, secSpeciesName, 
		  iSpeciesName)
{
  fReflected = _fReflected; 
  fScattered = _fScattered;
  // energy_w = _energy_w; 
  energy_max0 = _energy_max0;
  ks = _ks;
}

Secondary* Secondary2Group::CreateCounterPart(SpatialRegionGroup* srgPtr)
{
  Species* secSpeciesPtr = srgPtr->get_speciesPtr(secSpeciesName);
  Species* iSpeciesPtr = srgPtr->get_speciesPtr(iSpeciesName);
  return new Secondary2(secondary, threshold, Eemit, secSpeciesPtr, iSpeciesPtr,
			fReflected, fScattered, /*energy_w,*/ energy_max0, ks);
}

//=================== SecondaryParams

SecondaryParams::SecondaryParams()
{
  name = "Secondary";

  secondary.setNameAndDescription("secondary", "");
  secondary.setValue("0");
  parameterList.add(&secondary);

  threshold.setNameAndDescription("threshold","");
  threshold.setValue("0.5");
  parameterList.add(&threshold);

  Eemit.setNameAndDescription("Eemit","");
  Eemit.setValue("2");
  parameterList.add(&Eemit);
  
  secSpeciesName.setNameAndDescription("secSpecies","");
  secSpeciesName.setValue("NULL");
  parameterList.add(&secSpeciesName);

  iSpeciesName.setNameAndDescription("iSpecies","");
  iSpeciesName.setValue("NULL");
  parameterList.add(&iSpeciesName);
}

ostring SecondaryParams::InitializeFromStream(std::ifstream &fin)
{
  ostring msg = ParameterGroup::InitializeFromStream(fin);
  boundary->addSecondaryGroup(new SecondaryGroup(secondary.getValue(), 
    threshold.getValue(), Eemit.getValue(), secSpeciesName.getValue(), 
    iSpeciesName.getValue()));
  return msg;
}

Secondary2Params::Secondary2Params() : SecondaryParams()
{
  name = "Secondary2";

  fReflected.setNameAndDescription("fReflected","");
  fReflected.setValue("0");
  parameterList.add(&fReflected);

  fScattered.setNameAndDescription("fScattered","");
  fScattered.setValue("0");
  parameterList.add(&fScattered);

  //  energy_w.setNameAndDescription("energy_w","");
  //  energy_w.setValue("0");
  //  parameterList.add(&energy_w);

  energy_max0.setNameAndDescription("energy_max0","");
  energy_max0.setValue("0");
  parameterList.add(&energy_max0);

  ks.setNameAndDescription("ks","");
  ks.setValue("1");
  parameterList.add(&ks);
}

ostring Secondary2Params::InitializeFromStream(ifstream &fin)
{
  ostring msg = ParameterGroup::InitializeFromStream(fin);
  boundary->addSecondaryGroup(new Secondary2Group(secondary.getValue(), 
    threshold.getValue(), Eemit.getValue(), secSpeciesName.getValue(), 
    iSpeciesName.getValue(), fReflected.getValue(), fScattered.getValue(), 
						  /*energy_w.getValue(),*/
    energy_max0.getValue(), ks.getValue()));
  return msg;
}

//=================== DielectricParams Class
DielectricParams::DielectricParams(GridParams* _GP,SpatialRegionGroup *srg) 
  : BoundaryParams(_GP,srg)
{
  name = "Dielectric";

  parameterGroupList.add(&secondaryParams); // make this an eligible subgroup
  secondaryParams.setBoundary(this);
  parameterGroupList.add(&secondary2Params);
  secondary2Params.setBoundary(this);
  
  er.setNameAndDescription("er","Relative dielectric constant");
  er.setValue(ostring("1.0"));
  parameterList.add(&er);

  QuseFlag.setNameAndDescription("QuseFlag","Relative dielectric constant");
  QuseFlag.setValue("1");
  parameterList.add(&QuseFlag);

  reflection.setNameAndDescription("reflection", "reflected particle fraction");
  reflection.setValue("0");
  parameterList.add(&reflection);
  addLimitRule("reflection", "<", 0.0, "Fatal  -- 0 <= reflection <= 1", 1);
  addLimitRule("reflection", ">", 1.0, "Fatal  -- 0 <= reflection <= 1", 1);

  refMaxE.setNameAndDescription("refMaxE", "reflect for energy < refMaxE");
  refMaxE.setValue("1e10");
  parameterList.add(&refMaxE);

  transparency.setNameAndDescription("transparency","");
  transparency.setValue("0");
  parameterList.add(&transparency);
  addLimitRule("transparency", "<", 0.0, "Fatal  -- 0 <= transparency <= 1", 1);
  addLimitRule("transparency", ">", 1.0, "Fatal  -- 0 <= transparency <= 1", 1);
}

DielectricParams::~DielectricParams()
{
  secondaryGroupList.deleteAll();
}

void DielectricParams::addSecondaryGroup(SecondaryGroup* sGroup)
{
  secondaryGroupList.add(sGroup);
}

Boundary* DielectricParams::CreateCounterPart()
{
   Boundary* B = new Dielectric(SP.GetLineSegments(), er.getValue(),
                                QuseFlag.getValue());
   set_commonParams(B);
   return B;
}

void DielectricParams::set_commonParams(Boundary *B)
{
   Dielectric* D = (Dielectric*)B;
   D->set_reflection(reflection.getValue());
   D->set_refMaxE(refMaxE.getValue());
   D->set_transparency(transparency.getValue());
   addSecondaries(D);
}

// addSecondaries is provided as a function for descendants to call

void DielectricParams::addSecondaries(Boundary* B)
{
	oopicListIter<SecondaryGroup> sIter(secondaryGroupList);
	for (sIter.restart(); !sIter.Done(); sIter++)
	  B->addSecondary(sIter()->CreateCounterPart(spatialRegionGroup));
}

void DielectricParams::checkRules()
{
  BoundaryParams::checkRules();
  checkSecondaryRules();
}

void DielectricParams::checkSecondaryRules()
{
  oopicListIter<SecondaryGroup> sIter(secondaryGroupList);
  for (sIter.restart(); !sIter.Done(); sIter++)
	 sIter()->checkRules(ruleMessages, spatialRegionGroup);
}

//------------------------------------------------------------
//	DielectricRegionParams

DielectricRegionParams::DielectricRegionParams(GridParams *GP,
	SpatialRegionGroup *srg) : DielectricParams(GP, srg)
{
	name = "DielectricRegion";
}

Boundary* DielectricRegionParams::CreateCounterPart()
{
  Boundary* B = new DielectricRegion(SP.GetLineSegments(),
    er.getValue(), QuseFlag.getValue(), reflection.getValue());
  set_commonParams(B);
  return B;
}

void DielectricRegionParams::checkRules()
{
	// Rules from BoundaryParams
	ostring result = OnGridConstraint();
	if (strlen(result.c_str()) > 0) ruleMessages.add(new ostring(result));
	// Loop thru rules
	oopicListIter<RuleBase> nR(RuleList);
	oopicListIter<BaseParameter> nP(parameterList);
	for (nR.restart(); !nR.Done(); nR++)
	// Loop thru parameters and set parameter values required by rule
	{
		for (nP.restart(); !nP.Done(); nP++)
			nR.current()->setRuleVariable(nP.current()->getName(),
				nP.current()->getValueAsDouble());
		// Check the rule
		ostring result = nR.current()->checkRule();
		if (strlen(result.c_str()) > 0) ruleMessages.add(new ostring(result));
	}
	checkSecondaryRules();
}


Boundary* DielectricTriangleParams::CreateCounterPart()
{
	Boundary* B = new DielectricTriangle(SP.GetLineSegments(),
													 er.getValue(),QuseFlag.getValue());
	set_commonParams(B);
	return B;
}


// This overrides the boundg version, which doesn't do the proper windowing
// needed for a dielectric region split into SRB's.
void DielectricRegionParams::makeSegmentFitGrid(LineSegment *S, Grid *G) {
#ifndef MPI_VERSION
  // later on we expect coordinates in MKS units.  Convert them
  // here
  if(S->points==(int *)1) {
	 S->points=0;
	 S->A.set_e1(G->getX()[(int)S->A.e1()][(int)S->A.e2()].e1());
	 S->A.set_e2(G->getX()[(int)S->A.e1()][(int)S->A.e2()].e2());
	 S->B.set_e1(G->getX()[(int)S->B.e1()][(int)S->B.e2()].e1());
	 S->B.set_e2(G->getX()[(int)S->B.e1()][(int)S->B.e2()].e2());
  }


#endif // ndef MPI_VERSION  
#ifdef MPI_VERSION
//  Here's where we fix up the j's and k's or A's and B's to fit
//  into the grid window we're trying to create.  An annoying thing
//  but necessary.
//  I basically have to clip the "lines" defined by the coordinates
//  given.

  if(S->points==(int *)1) { // we're working in real-world coordinates 
	 // easier to work in MKS units here
	 // assume a UNIFORM mesh for autopartitioning.
	 // transform j1, j2, k1, k2 into MKS units.
	 S->A = Vector2((G->MaxMPIx1-G->MinMPIx1)*S->A.e1()/(Scalar)G->MaxMPIJ,
						 (G->MaxMPIx2-G->MinMPIx2)*S->A.e2()/(Scalar)G->MaxMPIK);
	 S->B = Vector2((G->MaxMPIx1-G->MinMPIx1)*S->B.e1()/(Scalar)G->MaxMPIJ,
						 (G->MaxMPIx2-G->MinMPIx2)*S->B.e2()/(Scalar)G->MaxMPIK);

/*		A1.setValue((G->MaxMPIx1-G->MinMPIx1)*j1.getValue()/(Scalar)G->MaxMPIJ);
	 B1.setValue((G->MaxMPIx1-G->MinMPIx1)*j2.getValue()/(Scalar)G->MaxMPIJ);
	 A2.setValue((G->MaxMPIx2-G->MinMPIx2)*k1.getValue()/(Scalar)G->MaxMPIK);
	 B2.setValue((G->MaxMPIx2-G->MinMPIx2)*k2.getValue()/(Scalar)G->MaxMPIK);
*/
  }
  Scalar y,m,x,b;  // linear parameters
  Scalar dx2,dy2;  // need these because floating point isn't exact.
  Scalar x1min = G->getX()[0][0].e1();
  Scalar x2min = G->getX()[0][0].e2();
  Scalar x1max = G->getX()[G->getJ()][0].e1();
  Scalar x2max = G->getX()[0][G->getK()].e2();
  dx2 = G->getX()[1][0].e1()-G->getX()[0][0].e1();
  dx2 /=2.01; //  so that rounding will occur properly later.
  dy2 = G->getX()[0][1].e2()-G->getX()[0][0].e2();
  dy2 /=2.01; //  so that rounding will occur properly later.
  Scalar a1,a2,b1,b2,t1,t2;
  a1 = S->A.e1(); a2 = S->A.e2(); b1 = S->B.e1();b2 = S->B.e2();
  if(a1 > b1 ) { // swap the points
	 t1 = a1; t2 = a2;
	 a1 = b1; a2 = b2;
	 b1 = t1; b2 = t2;
  }
  //  discard any boundaries completely out of bounds.
  if( (a1 < x1min - dx2 && b1 < x1min - dx2 ) || ( b1 > x1max + dx2 && a1 > x1max + dx2)) 
	 {
		S->A.set_e1(-1);
		return;
	 }
  
  if(a1 < x1min )  // out of bounds, need to squish it....
	 {
		a1 = x1min;
	 }
  if(b1 > x1max ) // out of bounds...
	 {
		b1 = x1max;
	 }
  //  discard any boundaries completely out of bounds.
  if( (a2 < x2min - dy2&& b2 < x2min - dy2) || ( b2 > x2max + dx2 && a2 > x2max + dx2)) 
	 {
		S->A.set_e1(-1);
		return;
	 }

  if(a2 > b2 ) { // swap the points
	 t1 = a1; t2 = a2;
	 a1 = b1; a2 = b2;
	 b1 = t1; b2 = t2;
  }
  
  if(a2 < x2min) // out of bounds, need to squish it.....
	 {
		a2 = x2min;
	 }

  if(b2 > x2max) // out of bounds, need to squish it.....
	 {
		b2 = x2max;
	 }
  if( (a2 < x2min - dy2&& b2 < x2min - dy2) || ( b2 > x2max + dx2 && a2 > x2max + dx2)) 
	 {
		S->A.set_e1(-1);
		return;
	 }

  // Hopefully, we now have a normalized, windowed line, write it back.
  S->A.set_e1(a1); S->A.set_e2(a2); S->B.set_e1(b1); S->B.set_e2(b2);   

#endif  /* MPI_VERSION */



}
