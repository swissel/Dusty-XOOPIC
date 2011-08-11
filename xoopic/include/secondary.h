#ifndef __SECONDARY_H
#define __SECONDARY_H

/*
=======================================================================

secondary.h

These classes represent a number of secondary models:

Secondary: simple step function secondary emission, with turn-on threshold.

Secondary2: a Vaughan-based model, includes energy and angular dependence, 
full emission spectrum including reflected and scattered primaries.

1.0 (JohnV, 09-02-98) Created by cutting Secondary from dielectric.h, added
    Secondary2.
2.0 (JohnV, 25-Oct-2002) Removed unused variables, cleaned up code, modified
    Secondary to enable isotropic emission of neutrals due to ion impact.

=======================================================================
*/

#include "particle.h"
class Boundary;
class MaxwellianFlux;

class Secondary
{
 protected:
  Boundary* bPtr; // access to Boundary geometry
  Vector3 normal; // normal of the boundary surface
  Vector3 t; // z cross normal, tangent to the surface
  Scalar secondary; // secondary electron emission coeff.
  Scalar threshold; // emission threshold;
  Scalar v_emit;  // max emission velocity
  Species* secSpecies; // secondary emission species
  Species* iSpecies; // ions which generate secondaries
 public:
  Secondary(Scalar _secondary, Scalar _threshold, Scalar Eemit, Species* 
	    _secSpecies, Species* _iSpecies);
  virtual ~Secondary();
  BOOL checkSpecies() {return secSpecies && iSpecies;};
  virtual Particle* createSecondary(Particle& p);
  int generateSecondaries(Particle& p, ParticleList& pList);
  Scalar get_q() {return secSpecies->get_q();}
  virtual Scalar number(Particle& p);
  void setBoundaryPtr(Boundary* _bPtr);
};

class Secondary2 : public Secondary
{
  Scalar fractionReflected; // fraction reflected primaries;
  Scalar fractionScattered; // fraction scattered primaries;
  // Scalar energy_w; // max value of emission energy function; unused?
  Scalar energy_max0; // energy at which sec. coeff = secondary for normal inc.;
  MaxwellianFlux *dist; // distribution for true secondaries
  Scalar ks; // surface smoothness (0 = rough, 2 = smooth)
 public:
  Secondary2(Scalar secondary, Scalar threshold, Scalar Eemit, Species* 
	     secSpecies, Species* iSpecies, Scalar f_ref, Scalar f_scat, 
	     /*Scalar energy_w,*/ Scalar energy_max0, Scalar ks);
  virtual ~Secondary2();
  virtual Scalar number(Particle& p);
  virtual Particle* createSecondary(Particle& p);
};

#endif
