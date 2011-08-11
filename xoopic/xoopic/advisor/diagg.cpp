//diagg.cpp

#include "spatialg.h"
#include "diagg.h"
#include "gridprob.h"
#include "probergn.h"
#include "laserspotdiag.h"
#include "wavesplitdiag.h"

#ifdef HAVE_FFT
#include "psdFieldDiag1d.h"
#include "psdFieldDiag2d.h"
#endif // HAVE_FFT 

#include "spbound.h"
#include "diags.h"
     
//=================== DiagParams Class

int Between2(int i, int lo, int hi)
{return ((lo <= i) && (i <= hi));}

DiagParams::DiagParams(GridParams *_GP,SpatialRegionGroup* srg)
	  : BoundaryParams(_GP,srg)
{name = "Diagnostics";
// GP = _GP;
 spatialRegionGroup = srg;
 j1.setNameAndDescription("j1","x1 Index for Diagnostic");
 j1.setValue("-1");
 k1.setNameAndDescription("k1","x2 Index for Diagnostic");
 k1.setValue("0");
 j2.setNameAndDescription("j2","x1 Index for Diagnostic");
 j2.setValue("0"); 
 k2.setNameAndDescription("k2","x2 Index for Diagnostic");
 k2.setValue("0");
 normal.setNameAndDescription("normal", "normal for Diagnostic");
 normal.setValue("1");
 parameterList.add(&j1);
 parameterList.add(&k1);
 parameterList.add(&j2);
 parameterList.add(&k2);
 A1.setNameAndDescription("A1","x1 Location (MKS) of boundary endpoint");
 A1.setValue("0");
 parameterList.add(&A1);
 A2.setNameAndDescription("A2","x2 Location (MKS) of boundary endpoint");
 A2.setValue("0");
 parameterList.add(&A2);
 B1.setNameAndDescription("B1","x1 Location (MKS) of boundary endpoint");
 B1.setValue("0");
 parameterList.add(&B1);
 B2.setNameAndDescription("B2","x2 Location (MKS) of boundary endpoint");
 B2.setValue("0");
 parameterList.add(&B2);

 VarName.setNameAndDescription("VarName",  "Name of Variable to be plotted");
 VarName.setValue("NULL");
 parameterList.add(&VarName);

 polarizationEB.setNameAndDescription("polarizationEB",  "E_component B_component");
 polarizationEB.setValue("EyBz");
 parameterList.add(&polarizationEB);

 psd1dFlag.setNameAndDescription("psd1dFlag",  "Flag for the 1D PSD");
 psd1dFlag.setValue("0"); // default is to turn off this calculation
 parameterList.add(&psd1dFlag);

 windowName.setNameAndDescription("windowName",  
                                  "Name of the window for use in the 1D PSD");
 windowName.setValue("Blackman"); // default is the Blackman window
 parameterList.add(&windowName);

 fieldName.setNameAndDescription("fieldName",  "E or B");
 fieldName.setValue("None"); // no field name is set by default
 parameterList.add(&fieldName); 

 fieldComponentLabel.setNameAndDescription("fieldComponentLabel",  
                                           "1, 2, or 3");
 fieldComponentLabel.setValue("0"); // the default is not a valid component label
 parameterList.add(&fieldComponentLabel); 

 integral.setNameAndDescription("integral",  "type of integral performed: line, flux, and sum");
 integral.setValue("NULL");
 parameterList.add(&integral);

 x1Label.setNameAndDescription("x1_Label","x1 Label the plot");
 x1Label.setValue("x1");
 parameterList.add(&x1Label);

 x2Label.setNameAndDescription("x2_Label","x2 Label the plot");
 x2Label.setValue("x2");
 parameterList.add(&x2Label);

 x3Label.setNameAndDescription("x3_Label","x3 Label the plot");
 x3Label.setValue("x3");
 parameterList.add(&x3Label);

 title.setNameAndDescription("title","Title of the plot");
 title.setValue("not_named");
 parameterList.add(&title);

 nfft.setNameAndDescription("nfft",  "number of fft must be power of 2");
 nfft.setValue("0");
 parameterList.add(&nfft);

 save.setNameAndDescription("save",  "Flag: 1 saves the diagnostic data in the dumpfile");
 save.setValue("0");
 parameterList.add(&save);

 HistMax.setNameAndDescription("HistMax",  "Maximum Length of History Buffer");
 HistMax.setValue("64");
 parameterList.add(&HistMax);

 Comb.setNameAndDescription("Comb",  "Every \'Comb\' value is left when HistMax is reached");
 Comb.setValue("0");
 parameterList.add(&Comb);

 Ave.setNameAndDescription("Ave",  "Averaged over \'Ave\' when HistMax is reached");
 Ave.setValue("0");
 parameterList.add(&Ave);
};

Diag* DiagParams::CreateCounterPart(SpatialRegion* SP)
  throw(Oops){
	region = SP;
	Diag* diagnostic;
	if(VarName.getValue() == ostring("LaserSpotSize")) {
	  return new LaserSpotDiagnostic(region,
					       j1.getValue(),
					       k1.getValue(),
					       j2.getValue(),
					       k2.getValue(),
					       nfft.getValue(),
					       HistMax.getValue(),
					       Ave.getValue(),
					       Comb.getValue(),
					       VarName.getValue(),
					       x1Label.getValue(),
					       x2Label.getValue(),
					       x3Label.getValue(),
					       title.getValue(),
					       save.getValue());
	}
	if(VarName.getValue() == ostring("WaveDirDiagnostic")) {
          return new WaveDirDiag(region,
					       j1.getValue(),
					       k1.getValue(),
					       j2.getValue(),
					       k2.getValue(),
					       nfft.getValue(),
					       HistMax.getValue(),
					       Ave.getValue(),
					       Comb.getValue(),
					       VarName.getValue(),
					       x1Label.getValue(),
					       x2Label.getValue(),
					       x3Label.getValue(),
					       title.getValue(),
					       save.getValue(), 
                                 polarizationEB.getValue(), 
                                 psd1dFlag.getValue(),
                                 windowName.getValue());
	}   
#ifdef HAVE_FFT    
        if(VarName.getValue() == ostring("PSDFieldDiag1d")) {
          try {
            return new PSDFieldDiag1d(region,
                                    j1.getValue(),
                                    k1.getValue(),
                                    j2.getValue(),
                                    k2.getValue(),
                                    nfft.getValue(),
                                    HistMax.getValue(),
                                    Ave.getValue(),
                                    Comb.getValue(),
                                    VarName.getValue(),
                                    x1Label.getValue(),
                                    x2Label.getValue(),
                                    x3Label.getValue(),
                                    title.getValue(),
                                    save.getValue(), 
                                    windowName.getValue(), 
                                    fieldName.getValue(),
                                    fieldComponentLabel.getValue());
          }
          catch(Oops& oops){
            oops.prepend("DiagParams::CreateCounterPart: Error:\n"); //SpatialRegionGroup::CreateCounterPart
            throw oops;
          }

	}       
        if(VarName.getValue() == ostring("PSDFieldDiag2d")) {
          try{
            return new PSDFieldDiag2d(region,
                                    j1.getValue(),
                                    k1.getValue(),
                                    j2.getValue(),
                                    k2.getValue(),
                                    nfft.getValue(),
                                    HistMax.getValue(),
                                    Ave.getValue(),
                                    Comb.getValue(),
                                    VarName.getValue(),
                                    x1Label.getValue(),
                                    x2Label.getValue(),
                                    x3Label.getValue(),
                                    title.getValue(),
                                    save.getValue(),  
                                    fieldName.getValue(),
                                    fieldComponentLabel.getValue());
          }
          catch(Oops& oops){
            oops.prepend("DiagParams::CreateCounterPart: Error:\n"); //SpatialRegionGroup::CreateCounterPart
            throw oops;
          }

	}       
#endif // HAVE_FFT

	if ((j1.getValue() == j2.getValue()) || (k1.getValue()==k2.getValue()))
		diagnostic = new GridProbes(region, j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue(),
										 nfft.getValue(), HistMax.getValue(),Ave.getValue(),
										 Comb.getValue(), 
										 VarName.getValue(), x1Label.getValue(),
										 x2Label.getValue(),x3Label.getValue(),
										 title.getValue(), save.getValue(), integral.getValue());
	else
		diagnostic = new ProbeRegion(region, j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue(),
										 nfft.getValue(), HistMax.getValue(),Ave.getValue(),
										 Comb.getValue(), 
										 VarName.getValue(), x1Label.getValue(),
										 x2Label.getValue(),x3Label.getValue(),
										 title.getValue(),save.getValue());
	return diagnostic;
}
/*void DiagParams::toGrid(Grid *G) {

  // j1 == -1 is the flag telling us to use the MKS

  if(j1.getValue()==-1) {
	 j1.setValue((int) (0.5 +G->getGridCoords(Vector2(A1.getValue(),A2.getValue())).e1()));
	 k1.setValue((int) (0.5 +G->getGridCoords(Vector2(A1.getValue(),A2.getValue())).e2()));
	 j2.setValue((int) (0.5 +G->getGridCoords(Vector2(B1.getValue(),B2.getValue())).e1()));
	 k2.setValue((int) (0.5 +G->getGridCoords(Vector2(B1.getValue(),B2.getValue())).e2()));
  } 


}
*/

/*ostring DiagParams::AlignmentConstraint()
{char buffer[200];
 if (((j1.getValue() == j2.getValue()) &&
		(k1.getValue() != k2.getValue())) ||
	  ((j1.getValue() != j2.getValue()) &&
		(k1.getValue() == k2.getValue())))
  sprintf(buffer, "%s","");
 else
  sprintf(buffer, "boundary (%d, %d)- (%d, %d) is not aligned with axis",
		j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue());
 return(ostring(buffer));}

ostring DiagParams::OnGridConstraint()
{char buffer[200];
 if  (Between2(j1.getValue(), 0, GP->getJ()) && Between2(k1.getValue(), 0, GP->getK()) &&
		Between2(j2.getValue(), 0, GP->getJ()) && Between2(k2.getValue(), 0, GP->getK()))
  sprintf(buffer, "%s","");
 else
  sprintf(buffer, "boundary (%d, %d)- (%d, %d) is not contained on grid",
		j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue());
 return(ostring(buffer));}

BOOL DiagParams::HasLength()
{
	if ((j1.getValue() == j2.getValue()) && (k1.getValue() == k2.getValue()))
		return 0;
	else
		return 1;
}

*/
void DiagParams::checkRules()
{
//ruleMessages.removeAll();
 ostring result = AlignmentConstraint();
 if (strlen(result.c_str()) > 0)
	 ruleMessages.add(new ostring(result));
 result = OnGridConstraint();
 if (strlen(result.c_str()) > 0)
	 ruleMessages.add(new ostring(result));
 // Loop thru rules
 oopicListIter<RuleBase> nR(RuleList);
 oopicListIter<BaseParameter> nP(parameterList);
 for (nR.restart(); !nR.Done(); nR++)
 // Loop thru parameters and set parameter values required by rule
  {for (nP.restart(); !nP.Done(); nP++)
	 nR.current()->setRuleVariable(nP.current()->getName(),
	 nP.current()->getValueAsDouble());
 // Check the rule
	ostring result = nR.current()->checkRule();
	if (strlen(result.c_str()) > 0)
	 ruleMessages.add(new ostring(result));}}

 // query function for GUI
// #ifndef UNIX
/**
Segment DiagParams::getSegment()
{
	Segment result(Point(j1.getValue(), k1.getValue()), 
		Point(j2.getValue(), k2.getValue()));
	result.setNormal(normal.getValue());
	return result; 
}
void DiagParams::setSegment(Segment& s)
{
	char buff[200];
	itoa(s.getj1(), buff, 10);
	j1.setValue(buff);
	itoa(s.getk1(), buff, 10);
	k1.setValue(buff);
	itoa(s.getj2(), buff, 10);
	j2.setValue(buff);
	itoa(s.getk2(), buff, 10);
	k2.setValue(buff);

//heuristics for setting the normal -- ntg 2-23-95
	normal.setValue("1");
	if (((j1.getValue() == j2.getValue()) &&
		 (j1.getValue() > GP->getJ()/2)) ||
		(((k1.getValue() == k2.getValue()) &&
		 (k1.getValue() > GP->getK()/2))))
		normal.setValue("-1");
	
}
*/
// #endif // ifndef UNIX
