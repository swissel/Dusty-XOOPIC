/*
====================================================================

PARTICLEGROUPVAR.H

	This class defines a group of particles which are related by a common computational particle weight but can
	vary in charge. Variable weighting allows q[] to vary, while
	holding np2c fixed at the maximum weighting for the group.  This is
	the electrostatic version of particle group.

Revision History
====================================================================
*/

#ifndef	__PTCLGPVAR_H
#define	__PTCLGPVAR_H

#include "ptclgrp.h"
#include "speciesvar.h"
#include "particlevar.h"

/**
 * A subclass of ParticleGroup designed to model dust particles with radius
 *and variable charge with appropriate advance method with fixed gravity.
 */
class ParticleGroupVAR : public ParticleGroup
{
	Scalar* q;
	Scalar r;
public:
	ParticleGroupVAR(int maxN, Species* s, Scalar np2c0, BOOL vary_np2c = FALSE)
	: ParticleGroup(maxN, s, np2c0,vary_np2c, true)
    {
        q= new Scalar[maxN],r=s->get_r();
        for (int i=0; i<maxN; i++) 
        {
            q[i]=0;
        }
    }
	void advance(Fields& fields, Scalar dt);
//    virtual ~ParticleGroupVAR();
	  Scalar	get_r() {return r;};
	  Scalar	get_m() {return species->get_m();};
	  Scalar*	get_q() {return q;};
	  Scalar	get_q(int i) {return q[i];};
	  Scalar	get_q_over_m(int i) {return (q[i]/(species->get_m()));};
      Scalar	get_qMKS(int i) {return get_q(i)/np2c0;};
    Scalar get_rho_species_MAX();//for null collision technique 
// ~ParticleGroupVAR();
    BOOL addMKS(Vector2 _x, Vector3 _v,Scalar _q);
    BOOL add(ParticleVar* p);
    BOOL add(const Vector2& , const Vector3& , const Scalar&,Scalar np2c=0);
    BOOL add(ParticleGroup& p);			//	add a group of particles
    BOOL set_q(int i,Scalar _q)
    {
        if(i<=maxN)
        {
            q[i]=_q;
            return TRUE;
        }
        else return FALSE;
    
    }
    
};

#endif	// ifndef __PTCLGPVAR_H
