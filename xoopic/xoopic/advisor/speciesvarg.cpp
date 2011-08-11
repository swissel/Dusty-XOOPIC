//
//  speciesvarg.cpp
//  
//
//  Created by joshua blumenkopf on 7/28/11.
//  Copyright 2011 Yeshiva University. All rights reserved.
//

#include <iostream>
#include "speciesvarg.h"
SpeciesVarParams::SpeciesVarParams():SpeciesParams()//unclear : ParameterGroup()
{   r.setNameAndDescription("r", "radius (meters)");
	r.setValue(".000001");
    parameterList.add(&r);

}
SpeciesVar* SpeciesVarParams::createCounterPart()
{
    if (get_subcycleIndex() > 1) set_supercycleIndex("1");
    
    SpeciesVar* species;
    
    species =  new SpeciesVar(get_speciesName(), get_r(), get_m(), get_subcycleIndex(),
                           get_supercycleIndex(), get_collisionModel(), get_threshold(), 
                           get_particleLimit(), get_nanbu(), get_coulomb(), get_TAModel(), get_IntraTAModel(), get_rmsDiagnosticsFlag() );
    
    species->zeroKineticEnergy();
    
    return species;

}

