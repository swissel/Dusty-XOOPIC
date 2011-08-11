/*
 *  particlevar.h
 *  
 *for dust
 *  Created by joshua blumenkopf on 7/21/11.
 *  Copyright 2011 Yeshiva University. All rights reserved.
 *
 */
#ifndef	__PARTICLEVAR_H
#define	__PARTICLEVAR_H

#include "particle.h"
#include "speciesvar.h"


class ParticleVar : public Particle {
	Scalar q;
	Scalar q_over_m;
    SpeciesVar* speciesvar;
public:
	ParticleVar(const Vector2& _x, const Vector3& _u, SpeciesVar* _species, Scalar _np2c,Scalar _q,
				BOOL _vary_np2c = FALSE):Particle(_x,_u,_species,_np2c,_vary_np2c)
	{q=_q,q_over_m=q/species->get_m(),speciesvar =_species;}
	void set_q(Scalar _q){q=_q;}
	Scalar	get_q() {return np2c*q;}// q*np2c
	Scalar	get_qMKS() {return q;}
	Scalar	get_q_over_m() {return q_over_m;}
	Scalar get_r(){return speciesvar->get_r();}
    SpeciesVar*	get_speciesPtr() {return speciesvar;}

			};
typedef oopicList<ParticleVar> ParticleVarList;





#endif
