#include "fft.h"

#ifndef _DIAGN_H
#define _DIAGN_H

#ifndef UNIX
#include <precision.h>
#include "ovector.h"
#endif

#ifdef UNIX
#include <config.h>
#endif

#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#endif

/*
====================================================================

DIAGN.H

Histories.

Revision/Author/Date
2.01  (Bruhwiler, 10-08-99) added integer flag synchRadiationFlag.
2.02  (Bruhwiler, 10-04-2000) added history plots for RMS beam quantities
====================================================================
*/

#include "history.h"
#include "ostring.h"
#include "ngd.h"
#include "misc.h"

class Scalar_Local_History;
class Scalar_History;

class PDistCol;
class Species;

typedef struct {
	const char *name;
	long unsigned int memory_size;
	int nparticles;
	int ngroups;  //number of particle groups
	int nparticles_plot;  // The number of particles actually plotted.
	int ID;   //  position in the particlegrouplist.  
	Scalar *x1positions;
	Scalar *x2positions;
	Scalar *x1velocities;
	Scalar *x2velocities;
	Scalar *x3velocities;
	Species *spec;
} SpeciesDiag;

class Ihistdiag {
 public:
  char name[80];
  PDistCol *p_dist;
  Scalar_Ave_History *Ihist; // total 
  Scalar_Ave_History **Ihist_sp; // plus one for each species
  int nSpecies; // number of species
  Ihistdiag(int _nSpecies) {
     nSpecies = _nSpecies;
     Ihist_sp = new Scalar_Ave_History*[nSpecies];
  }
  ~Ihistdiag() { 
    if(Ihist) 
      delete Ihist;
    if (Ihist_sp) {
       for (int i=0; i<nSpecies; i++) delete Ihist_sp[i];
       delete[] Ihist_sp;
    }
  }
};

class PFhistdiag {
 public:
  char name[80];
  Scalar *p_flux;
  Scalar_History *PFhist;
  Scalar_Local_History *PFlocal;  
  ~PFhistdiag() {
    if (PFhist)
      delete PFhist;
    if (PFlocal)
      delete PFlocal;
  }
};
	
class Diagnostics {
public:

  //  for the initwin function
  ostring X1;
  ostring X2;
  ostring X3;

  // 
  // the next two are used for the FFT diagnostics 
  // 
  ostring K1;
  ostring K2;

  ostring E1;
  ostring E2;
  ostring E3;
  ostring B1;
  ostring B2;
  ostring B3;
  ostring I1;
  ostring I2;
  ostring I3;
  ostring U1vsX1;
  ostring U2vsX1;
  ostring U3vsX1;
  ostring U1vsX2;
  ostring U2vsX2;
  ostring U3vsX2;

  int jm,km,km2;
  SpeciesDiag* theSpecies;
  Scalar Uetot, Ubtot, Uktot, divderrormag;
  Scalar *x1_array, *x2_array, **E1_array, **E2_array,**E3_array;
  Scalar **B1_array, **B2_array, **B3_array, **I1_array, **I2_array,**I3_array;
  Vector3 **B,**E,**S,**I,**BDynamic;
#ifdef DEBUG_FIELDS
  Vector3 **intBdS,**intEdl;
#endif
  Vector3 **S_array;
  Scalar **rho,**phi, **totalRho;
#ifdef DEBUG_PHI
  Scalar **phi_err;
#endif

  Scalar ***rho_species, ***loaded_density;
  Scalar **CellVolumes;
  int Show_loaded_densityFlag;
  Scalar x1max,x2max,x1min,x2min;
  SpatialRegion *theSpace;
  Scalar **Ue, **Ub;
  Scalar **divderror;
  Scalar *I1hist;
  Scalar **E1_PHI,**E2_PHI,**E1_ERR,**E2_ERR;
  Scalar **ave_den_at_Z,**ave_den_at_Z_show;

  ostring RegionName;

  Scalar_History energy_e, energy_b, energy_k, energy_all;
  Scalar_History **number, **ke_species, **ngroups,
                 **total_density, **Ave_KE;
  Scalar_History divderrorhis;
  Scalar *t_array;
  int HISTMAX; // specifies the array size for most builtin time histories

	/** Need to collect scalar history of AVE & RMS beam quantities
	 *  for beam physics applications:  velocities, beam sizes,
	 *  energy spread, emittance, etc.
	 *  (Bruhwiler, revisions started on 10/04/2000)
	 *  (Bruhwiler/Dimitrov,  started on 10/09/2000)
	 */
  Scalar_History **aveBeamSize;
  Scalar_History **rmsBeamSize;
  Scalar_History **aveVelocity;
  Scalar_History **rmsVelocity; /* array of pointers to the three components of
				   the velocity */
  Scalar_History **rmsEmittance;/* array of pointers to the X1 and X2 
				   components of the emittance */
  ///Scalar_History *aveEnergy_eV;/* for the energy average and rms */
  ///Scalar_History *rmsEnergy_eV; 
  
  /** For the purpose of plotting the NeutralGasDensity, which is a 
   *  data member of the Grid class, we need a local member pointer
   *  to access the data. I'll use the same name. This local pointer 
   *  is used only for access and does not carry any responsibility
   *  to create/destroy storage for the data. This is done elsewhere.
   *  DAD: Wed Nov 29 11:56:57 MST 2000
   *
   * the NGD objects now live in an oopic list in the grid. 
   * The plotting of their data should happen through 
   * an oopicList. dad: 01/24/2001
   */
  oopicList<NGD>* ptrNGDList;
  /**
   * the neutral gas densities will be plotted at the centers of
   * the grid cells. For this reason we need the x1_arrayNGD
   * and x2_arrayNGD to give the positions of the cell centers.
   * There are JxK cells indexed via [0...J-1][0...K-1] with 
   * the centers at (j+0.5, k+0.5). The jmNGD and kmNGD
   * give the dimensions of the x1_arrayNGD and x2_arrayNGD
   * arrays. 
   */
  int jmNGD, kmNGD;
  Scalar* x1_arrayNGD;
  Scalar* x2_arrayNGD;
  
  Scalar **error;
  int number_of_species;
  int electrostaticFlag;
  int synchRadiationFlag;
  int boltzmannFlag;
  Scalar Bz0;
  BoundaryList *blist;
  oopicList<Ihistdiag> *BoundaryIhist;
  oopicList<PFhistdiag> *BoundaryPFhist;

  Diagnostics(SpatialRegion *thisSpace);
  ~Diagnostics();

  void UpdatePreDiagnostics();
  void UpdatePostDiagnostics();
  void DumpDiagnostics();
#ifdef HAVE_HDF5
  void dumpH5(dumpHDF5 &dumpObj);
  void dumpDiagH5(dumpHDF5 *H5DumpObj);
  void dumpAllDiagsH5(oopicList<dumpHDF5> *H5DiagDumpObjs);
  void dumpGridH5(dumpHDF5 *H5DumpObj);
#endif
  void RestoreDiagnostics(); 
  void Update() {
	 UpdatePreDiagnostics();
	 UpdatePostDiagnostics();
  }
  void history();
  void set_HISTMAX(int histmax) {HISTMAX = histmax;};
  
  void InitWin();
  void set_analytic_rho(Scalar**, int, int, Scalar, Scalar, int);
  void compute_div_derror_magnitude(Scalar **divderror, int jm, int km);
  
private:
  int Jm; /* DAD: array length for some of the data members of the class
	     which are pointers to arrays. It is needed when the memory
	     is to be deallocated */
  // a helper function to calculate the average and variance
  // for an array of "random" variables distributed according to
  // the array of weight elements _Pq.It uses a two-pass algorithm.
  void Get_Statistical_Moments(int N, Scalar* W, Scalar* Data,
			       Scalar& Ave, Scalar& Var);
  // a helper function needed for the work in the 
  // "void Diagnostics::history()" member function
  // related to the ave and RMS calculations
  // DAD: Fri Oct 13 14:49:15 MDT 2000
  void Set_Diagnostics_Moments(int Geometry_Flag, 
			       oopicListIter<ParticleGroup>& PGIterator, 
			       Grid* RMSGrid, Scalar* _Pq, int NumWeightElements, 
			       Scalar& AveX1, Scalar& AveX2, 
			       Scalar& RMSX1, Scalar& RMSX2, 
			       Scalar* _aveU, Scalar* _rmsU, Scalar* _rmsEmit,
			       Scalar& _aveE_eV, Scalar& _rmsE_eV);  

};

#endif //DIAGN_H
