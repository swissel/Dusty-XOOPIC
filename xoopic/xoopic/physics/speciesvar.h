/*
 *  speciesvar.h
 *  no charge, but radius for dust
 *
 *  Created by joshua blumenkopf on 7/21/11.
 *  Copyright 2011 Yeshiva University. All rights reserved.
 *
 */
#ifndef __SPECIESVAR_H
#define __SPECIESVAR_H

#include "species.h"
#include <iostream>


class SpeciesVar : public Species {
	Scalar r;
public:
	SpeciesVar(ostring _name, Scalar _r, Scalar _m, int subIndex, int superIndex, 
				CollisionModel cm, Scalar _threshold, int _particleLimit, int _nanbu, ostring _coulomb, ostring _TAModel, int _IntraTAModel,
			   int _rmsDiagnosticsFlag) :Species(_name, _m, subIndex, superIndex, 
												 cm,_threshold,  _particleLimit,  _nanbu, _coulomb, _TAModel,  _IntraTAModel,
												 _rmsDiagnosticsFlag,1)
			   {
				   r=_r;
               }
	Scalar get_r(){return r;}
	virtual ~SpeciesVar(){
	}
    virtual Species* clone() { return new SpeciesVar(*this); }
    
};



#endif
