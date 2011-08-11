/*
	 ====================================================================

	 GRIDPROB.CPP

	 1.01	(KC, 9-21-95) After release. First draft.
	 1.02	(JohnV, 10-27-95) def'ed out xgrafix include for PC version

	 ====================================================================
	 */

#include "newdiag.h"
#include "diagn.h"
#include "gridprob.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"
#define True 1

extern "C++" void printf(char *);
extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif //NOX
}

//--------------------------------------------------------------------

GridProbes::GridProbes(SpatialRegion* region, int j1, int k1, int j2, int k2,
											 int nfft, int HistMax, 
											 int Ave, int Comb, ostring _VarName, 
											 ostring diagXLabel, 
											 ostring diagYLabel, ostring diagZLabel, ostring diagLabel,
											 int save, 
											 ostring _integral)
:Diag(region, j1, k1, j2, k2, nfft, HistMax, 
			Ave, Comb, _VarName, diagXLabel, 
			diagYLabel, diagZLabel, diagLabel, save)
{
  integral = _integral;
	VarName = _VarName;
	if (alongx2()) // vertical
		size = k2-k1+1;
	else // horizontal
		size = j2-j1+1;
	
	Chose_Later = 0;
	Integrate = 0;
	Space_fft = 0;

	grid = region->get_grid();
	VarPtr = new Scalar*[size];
	if ((integral==(ostring)"flux")||(integral==(ostring)"line")||(integral==(ostring)"sum"))
		{
			dVarPtr = new Scalar[size];
			Integrate = 1;
			Stuff = 0;
		}
	x_array = new Scalar[size];
	if (Space_fft)
		FTVarPtr = new Scalar[size];

	if (alongx2()) // vertical
		for (int k=0; k<size; k++)
			x_array[k] = region->getMKS(j1,k1+k).e2();
	else // horizontal
		for (int j=0; j<size; j++)
			x_array[j] = region->getMKS(j1+j,k1).e1();

	if (VarName == (ostring)"E1")
		{
			printf("You Chose Varname = E1 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getENode()[j1][k1+i])))+0);
					else
						VarPtr[i] = (((Scalar*)(&(region->getENode()[j1+i][k1])))+0);
				}
		}
	else if(VarName == (ostring)"E2")
		{
			printf("You Chose Varname = E2 ");
			for (int i=0; i<size; i++)
				{
					if(alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getENode()[j1][k1+i])))+1);
					else
						VarPtr[i] = (((Scalar*)(&(region->getENode()[j1+i][k1])))+1);
				}
		}
	else if(VarName == (ostring)"E3")
		{
			printf("You Chose Varname = E3 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getENode()[j1][k1+i])))+2);
					else
						VarPtr[i] = (((Scalar*)(&(region->getENode()[j1+i][k1])))+2);
				}
		}
	else if (VarName == (ostring)"intEdl1")
		{
			printf("You Chose Varname = intEdl1 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getIntEdl()[j1][k1+i])))+0);
					else
						VarPtr[i] = (((Scalar*)(&(region->getIntEdl()[j1+i][k1])))+0);
				}
		}
	else if(VarName == (ostring)"intEdl2")
		{
			printf("You Chose Varname = intEdl2 ");
			for (int i=0; i<size; i++)
				{
					if(alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getIntEdl()[j1][k1+i])))+1);
					else
						VarPtr[i] = (((Scalar*)(&(region->getIntEdl()[j1+i][k1])))+1);
				}
		}
	else if(VarName ==(ostring) "intEdl3")
		{
			printf("You Chose Varname = intEdl3 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getIntEdl()[j1][k1+i])))+2);
					else
						VarPtr[i] = (((Scalar*)(&(region->getIntEdl()[j1+i][k1])))+2);
				}
		}
	else if(VarName ==(ostring) "B1")
		{
			printf("You Chose Varname = B1 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getBNode()[j1][k1+i])))+0);
					else
						VarPtr[i] = (((Scalar*)(&(region->getBNode()[j1+i][k1])))+0);
				}
		}
	else if(VarName ==(ostring) "B2")
		{
			printf("You Chose Varname = B2 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getBNode()[j1][k1+i])))+1);
					else
						VarPtr[i] = (((Scalar*)(&(region->getBNode()[j1+i][k1])))+1);
				}
		}
	else if(VarName ==(ostring) "B3")
		{
			printf("You Chose Varname = B3 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getBNode()[j1][k1+i])))+2);
					else
						VarPtr[i] = (((Scalar*)(&(region->getBNode()[j1+i][k1])))+2);
				}
		}
	else if(VarName ==(ostring) "I1")
		{
			printf("You Chose Varname = I1 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getI()[j1][k1+i])))+0);
					else
						VarPtr[i] = (((Scalar*)(&(region->getI()[j1+i][k1])))+0);
				}
		}
	else if(VarName ==(ostring) "I2")
		{
			printf("You Chose Varname = I2 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getI()[j1][k1+i])))+1);
					else
						VarPtr[i] = (((Scalar*)(&(region->getI()[j1+i][k1])))+1);
				}
		}
	else if(VarName == (ostring)"I3")
		{
			printf("You Chose Varname = I3 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getI()[j1][k1+i])))+2);
					else
						VarPtr[i] = (((Scalar*)(&(region->getI()[j1+i][k1])))+2);
				}
		}
	else if(VarName ==(ostring) "rho")
		{
			printf("You Chose Varname = rho ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getRho()[j1][k1+i]))));
					else
						VarPtr[i] = (((Scalar*)(&(region->getRho()[j1+i][k1]))));
				}
		}
	else if(VarName == (ostring)"Q")
		{
			printf("You Chose Varname = Q ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getQ()[j1][k1+i]))));
					else
						VarPtr[i] = (((Scalar*)(&(region->getQ()[j1+i][k1]))));
				}
		}
	else if(VarName == (ostring)"phi")
		{
			printf("You Chose Varname = phi ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(region->getPhi()[j1][k1+i]))));
					else
						VarPtr[i] = (((Scalar*)(&(region->getPhi()[j1+i][k1]))));
				}
		}
	else 
		Chose_Later = 1;

	if(Integrate)
		Chose_Later = 1;
	
	if (HistMax)
		{
			Space_fft = 0; // Space_fft is not used yet
			if (Space_fft)
				{
					//				if(Ave)
					//					history = new Vector_Ave_History(size/2,HistMax,Ave);
					if(Comb)
						history = new Vector_History(size/2,HistMax,Comb);
					else
						history = new Vector_Local_History(size/2,HistMax,1);
				}
			else
				{
					if (size>1)
						{ 
							if(Integrate)
								{
									if((Ave>0)&&(Comb>0))
					 					history = new Scalar_Ave_History(HistMax,Ave,Comb,1);
									else if(Ave)
										history = new Scalar_Ave_Local_History(HistMax,Ave,1);
									else if(Comb)
										history = new Scalar_History(HistMax,Comb);
									else
										history = new Scalar_Local_History(HistMax,1);
								}
							else
								{
									if ((Ave>0)&&(Comb>0))
										history = new Vec_Pointers_History_Ave(size,HistMax, Ave, Comb);
									else if(Comb>0)
										history = new Vec_Pointers_History(size,HistMax,Comb);
									else if (Ave>0)
										history = new Vec_Pointers_History_Local_Ave(size,HistMax,Ave,HistMax/4);
									else
										history = new Vec_Pointers_Local_History(size,HistMax,HistMax/4);
								}
						}
					else if (size==1)
						{
							if((Ave>0)&&(Comb>0))
								history = new Scalar_Ave_History(HistMax,Ave,Comb,HistMax/4);
							else if(Ave)
								history = new Scalar_Ave_Local_History(HistMax,Ave,1);
							else if(Comb)
								history = new Scalar_History(HistMax,Comb);
							else
								history = new Scalar_Local_History(HistMax,1);
						}
				}
		}
	else // for HistMax = 0 
		{
		}
		
}
GridProbes::~GridProbes()
{
	int size = MAX(j2-j1,k2-k1);
	for (int i = 0; i<size;i++) delete[] VarPtr[i];
	delete[] VarPtr;
}
void GridProbes::MaintainDiag(Scalar t)
{
	int i;
	if (Chose_Later==1) Chose_VarPtr();
	if (size == 1) history->add(**VarPtr,t);
	else {
	  if (Space_fft) {
		 FFT();
		 //					history->add(FTVarPtr,t);
	  }
	  if (Integrate) {
		 Stuff =0;
		 for (i=0; i<size; i++)	Stuff += *VarPtr[i]*dVarPtr[i];
		 history->add(Stuff,t);
	  }
	  else history->add(VarPtr,t);
	}
}

void GridProbes::initwin()
{
#ifndef NOX
	if ((size == 1)||(Integrate==1))
		{
			WinSet2D("linlin",x1Label.c_str()," ",title.c_str(),"closed",800,200,
							1.0,1.0,True,True,0,0,0.0,0.0);
			XGCurve(history->get_time_array(),history->get_data(), history->get_hi(),1);
		}
	else if (size > 1)
		{
			XGSet3D( "linlinlin",x1Label.c_str(),x2Label.c_str(),title.c_str(),
							45.0,225.0,"closed",1,1,
							1.0,1.0,1.0,1,1,1,0,1,0,1,0,1);
			XGSurf(x_array,history->get_time_array(),history->get_vpdata(), &size, history->get_hi(), 1 );
		}
#endif
}
/*
int GridProbes::Dump(FILE* DMPFile)
{
#ifndef NOX
	XGWrite(&j1, 4, 1, DMPFile, "int");
	XGWrite(&k1, 4, 1, DMPFile, "int");
	XGWrite(&j2, 4, 1, DMPFile, "int");
	XGWrite(&k2, 4, 1, DMPFile, "int");
	//	ostring name = title;
	int titleLength = title.length();
	// Write the length of the title
	XGWrite(&titleLength,4,1,DMPFile,"int");
	// Write the title
	XGWrite(title.c_str(),1,titleLength+1,DMPFile,"char");

	if (save)
		history->dump(DMPFile);
#endif
	return(TRUE);
}

int GridProbes::Restore(FILE* DMPFile, int j1test, int k1test, int j2test, int k2test, ostring titleTest)
{
	if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test)&&(title == titleTest))
		{
			//Diags is the same get your information from fill
			if (save)
				history->restore(DMPFile);
			return(TRUE);
		}
	else
		return(FALSE);
}
*/
void GridProbes::FFT()
{
	Scalar average;
	int i;

	average = 0.0;
	for (i=0; i<size; i++)
		average += *VarPtr[i];
	for (i=0; i<size; i++)
		{
			if (i<=nfft/2)
				FTVarPtr[i] = (*VarPtr[i]-average)*i/nfft;
			else
				FTVarPtr[i] = (*VarPtr[i]-average)*(nfft-i)/nfft;
		}
	realft(FTVarPtr, nfft, 1);
}

void GridProbes::Chose_VarPtr()
{
	Chose_Later = 0;
	if(integral == (ostring)"line")
		for (int i=0; i<size; i++)
			{
				if (alongx2())
					dVarPtr[i] = grid->dlPrime(j1,k1+i).e2();
				else
					dVarPtr[i] = grid->dlPrime(j1+i,k1).e1();
			}

	if(integral== (ostring)"flux")
		for (int i=0; i<size; i++)
			{
				if (alongx2())
					dVarPtr[i] = grid->dSPrime(j1,k1+i).e2();
				else
					dVarPtr[i] = grid->dSPrime(j1+i,k1).e1();
			}

	if(integral==(ostring) "sum")
		for (int i=0; i<size; i++)
			dVarPtr[i] = 1;
	if(VarName == (ostring)"poynting1")
		{
			printf("You Chose Varname = poynting 1 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(theDiagnostics->S_array[j1][k1+i])))+0);
					else
						VarPtr[i] = (((Scalar*)(&(theDiagnostics->S_array[j1+i][k1])))+0);
				}
		}
	else if(VarName == (ostring)"poynting2")
		{
			printf("You Chose Varname = poynting 2 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(theDiagnostics->S_array[j1][k1+i])))+1);
					else
						VarPtr[i] = (((Scalar*)(&(theDiagnostics->S_array[j1+i][k1])))+1);
				}
		}
	else if(VarName == (ostring)"poynting3")
		{
			printf("You Chose Varname = poynting 3 ");
			for (int i=0; i<size; i++)
				{
					if (alongx2())
						VarPtr[i] = (((Scalar*)(&(theDiagnostics->S_array[j1][k1+i])))+2);
					else
						VarPtr[i] = (((Scalar*)(&(theDiagnostics->S_array[j1+i][k1])))+2);
				}
		}
	else 
		for(int isp=0;isp<number_of_species;isp++) 
			if (VarName == (ostring)theDiagnostics->theSpecies[isp].name)
				{
					std::cout << "You Chose Varname = " << theDiagnostics->theSpecies[isp].name << endl;
					for (int i=0; i<size; i++)
						{
					  	if (alongx2())
								VarPtr[i] = (((Scalar*)(&(theDiagnostics->rho_species[isp][j1][k1+i]))));
							else
								VarPtr[i] = (((Scalar*)(&(theDiagnostics->rho_species[isp][j1+i][k1]))));
						}
				 }
	if (VarPtr[0]==NULL)
		std::cout << "Could not display " << VarName << endl;
}



