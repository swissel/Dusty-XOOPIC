//
//  speciesvarg.h
//  
//
//  Created by joshua blumenkopf on 7/28/11.
//  Copyright 2011 Yeshiva University. All rights reserved.
//

#ifndef _speciesvarg_h
#define _speciesvarg_h
#include "speciesg.h"
#include "speciesvar.h"

class SpeciesVarParams:public SpeciesParams
{
    ScalarParameter r;
    public:
    SpeciesVarParams();
    virtual ~SpeciesVarParams() {};
    Scalar get_r() {return r.getValue();};
    SpeciesVar* createCounterPart();
 

};


#endif
