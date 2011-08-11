/*
 *  secondary.cpp
 *
 * These classes represent a number of secondary models:
 * 
 * Secondary: simple step function secondary emission, with turn-on threshold.
 * 
 * Secondary2: a Vaughan-based model, includes energy and angular dependence, 
 * full emission spectrum including reflected and scattered primaries.
 * 
 * 1.0 (JohnV, 09-02-98) Created by cutting Secondary from dielectric.h, added
 *     Secondary2.
 * 2.0 (JohnV, 25-Oct-2002) Removed unused variables, cleaned up code, modified
 *     Secondary to enable isotropic emission of neutrals due to ion impact.
 * 2.01 (JohnV, 29-Oct-2002) Corrected coefficient in Emax0 to conform to updated
 *     Vaughan model (Vaughan, IEEE Trans. ED 40 (1993)). Also updated k to
 *     use the empirical values in the same source, and added the high energy
 *     relation therein as well.
 */

#include "secondary.h"
#include "boundary.h"
#include "vmaxwelln.h"

Secondary::Secondary(Scalar _secondary, Scalar _threshold, Scalar Eemit, 
		     Species* _secSpecies, Species* _iSpecies)
{
  secondary = _secondary; 
  threshold = _threshold; 
  secSpecies = _secSpecies; 
  iSpecies = _iSpecies;
  v_emit = sqrt(2*PROTON_CHARGE*Eemit/secSpecies->get_m()); 
  //printf("Secondary\n");
}

Secondary::~Secondary()
{

}

void Secondary::setBoundaryPtr(Boundary* _bPtr)
{
  bPtr = _bPtr;
  normal = bPtr->unitNormal();
  t = Vector3(0,0,1).cross(normal); // tangent in x1-x2 plane
  // third component is in x3
}

int Secondary::generateSecondaries(Particle& p, ParticleList& pList)
{
  if (p.get_speciesPtr() != iSpecies) return 0;
  if (p.energy_eV_MKS() < threshold) return 0;
  Scalar yield = number(p);
  int n = (int)yield;
  yield -= n;
  if (frand() <= yield) n++;
  for (int i=0; i<n; i++) pList.add(createSecondary(p));
  return n;
}

// number of secondaries emitted per incident particle

Scalar Secondary::number(Particle& p)
{
  return secondary;
}

Particle* Secondary::createSecondary(Particle& p)
{
  // compute isotropic angles of emission
  // general case should work for oblique boundaries
  Scalar sintheta = frand();
  Scalar costheta = 1 - sintheta*sintheta;
  Scalar phi = 2*M_PI*frand();
  Vector3 v = costheta*normal + sintheta*cos(phi)*t;
  v.set_e3(sin(phi)*sintheta);
  v *= frand()*v_emit; // linearly scale magnitude
  return new Particle(p.get_x(), v, secSpecies, p.get_np2c(), 
		      p.isVariableWeight());
}

Secondary2::Secondary2(Scalar secondary, Scalar threshold, Scalar Eemit, 
		       Species* secSpecies, Species* iSpecies, Scalar f_ref,
		       Scalar f_scat, /*Scalar _energy_w,*/ Scalar _energy_max0,
		       Scalar _ks)
  : Secondary(secondary, threshold, Eemit, secSpecies, iSpecies)
{
  fractionReflected = f_ref;
  fractionScattered = f_scat;
  // energy_w = Eemit; unused?
  energy_max0 = _energy_max0;
  ks = _ks;
  dist = new MaxwellianFlux(secSpecies);
  dist->setThermal(Eemit, EV);
  //printf("Secondary2\n");
}

Secondary2::~Secondary2()
{
  delete dist;
  //printf("~Secondary2\n");
}

Scalar Secondary2::number(Particle& p)
{
  if (secondary <= 0) return 0;
  Scalar theta, delta_ratio;
  Vector3 v = p.get_u()/p.gamma();
  theta = acos(-(v*bPtr->unitNormal())/v.magnitude());
  Scalar Emax = energy_max0*(1 + 0.5*ks*theta*theta/M_PI);
  Scalar w = (p.energy_eV_MKS() - threshold)/(Emax - threshold);
  Scalar k = (w > 1) ? 0.25 : 0.56;
  if (w < 3.6)
     delta_ratio = pow(w*exp(1-w), k);
  else 
     delta_ratio = 1.125/pow(w, Scalar(0.35));
  return secondary*(1 + 0.5*ks*theta*theta/M_PI)*delta_ratio;
}

Particle* Secondary2::createSecondary(Particle& p)
{
  Scalar R = frand();
  Vector3 u;
  if (R < fractionScattered+fractionReflected){
    u = p.get_u();
    if (bPtr->alongx1()) u.set_e2(-u.e2()); // reflected primary;
    else u.set_e1(-u.e1());
    if (R > fractionReflected) { // scattered primary;
       // compute isotropic angles of emission
       // general case should work for oblique boundaries
       Scalar sintheta = frand();
       Scalar costheta = 1 - sintheta*sintheta;
       Scalar phi = 2*M_PI*frand();
       Vector3 v = costheta*normal + sintheta*cos(phi)*t;
       v.set_e3(sin(phi)*sintheta);
       v *= frand()*u.magnitude(); // linearly scale magnitude
       u = v;
    }
  }
  else u = dist->get_U(bPtr->unitNormal()); // true secondary;
  return new Particle(p.get_x(), u, secSpecies, p.get_np2c(), 
		      p.isVariableWeight());
}
