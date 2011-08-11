//
//  dd.h
//  
// for collisions, based on ngd, this models dust density
//  Created by joshua blumenkopf on 8/7/11.
//  Copyright 2011 Yeshiva University. All rights reserved.
//

#ifndef _dd_h
#define _dd_h
#include "ngd.h"
class DD:public NGD{
public:
    DD(Grid* pgrid, const ostring& gType, 
       const ostring& _analyticF, const Scalar& _gasDensity,  
       const Vector2& _p1Grid, const Vector2& _p2Grid,
       const int& _discardDumpFileNGDDataFlag):NGD( pgrid,gType, _analyticF,  _gasDensity,   _p1Grid,  _p2Grid, _discardDumpFileNGDDataFlag){}
private:
    Scalar** DustAverageCharge;
};

#endif
