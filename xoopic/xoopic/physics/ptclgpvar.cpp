/*
====================================================================

PARTICLEGROUPVAR.CPP

Variable charge version of ParticleGroup.

Revision History

====================================================================
*/

#include "ptclgpvar.h"
#include "fields.h"
#include	"particle.h"
#include "speciesvar.h"
#include "particlevar.h"

void ParticleGroupVAR::advance(Fields& fields, Scalar dt)
{
	Scalar	f;// = 0.5*dt*q_over_m; varies
//	Scalar*	q_dt = q/dt;
	Vector3	uPrime;
	Vector3 uOld;
	Vector3	a;
	Vector3	t;
	Vector3	s;
    int m =0;
	Vector2* x = ParticleGroup::x;
    Vector2  y;
	Vector3* u = ParticleGroup::u;
	Vector3	dxMKS;
	Boundary*	bPtr;
	Grid&	grid = *fields.get_grid();
	Vector3	B;
//	register Scalar temp;
	double localEnergy = 0; // overflows if float
	for (int i=0; i<n; i++, x++, u++,q++)
	{
//        cout<<get_qMKS(i)<<" ";
		uOld = *u;
 //       cout <<x[i].e1()<<" "<<x[i].e2()<<ostring("moving");
        y=x[i];
		bPtr = 0;
  //      cout<<q[i]<<" ";
        f=0.5*dt*q[i]/get_m();
		a = f*fields.E(*x);
        a +=(0,0,-10);//gravity (in mks)
		*u += a;									//	half acceleration
		B = fields.B(*x);   // speed-ups make an array for f*E and f*B
		                    // have a and t returned with one call
		                    // if test B.isNonZero outside this loop
		                    // then f*B doesn't have to be returned and
								  // an if test and += are saved per particle
		if (B.isNonZero())
		{
			t = f*B;								// reuse B!
			uPrime = *u + u->cross(t);
			s = 2*t/(1 + t*t);
			*u += uPrime.cross(s);			//	rotation
		}
		*u += a;									//	half acceleration
        if (!(y.e1()==(x[i].e1()))||!(y.e2()==(x[i].e2()))) {
            cout <<y.e1()<<" "<<y.e2()<<ostring("moving");
            cout <<x[i].e1()<<" "<<x[i].e2()<<endl;
        } 
		dxMKS = grid.differentialMove(fields.getMKS(*x), *u, *u, dt);
		while (fabs(dxMKS.e1()) + fabs(dxMKS.e2()) > 1E-25 && !bPtr)
			bPtr = grid.translate(*x, dxMKS);
		if (bPtr)
		{
		  //	send this particle to boundary
		  bPtr->collect(*(new Particle(*x, *u, species, get_np2c(i),
					       (BOOL)(qArray!=0))), dxMKS);
		  n--; // decrement number of particles
		  //	Move last particle into this slot and advance it.
		  if (i == n) break; //	last particle in array?
		  *x = ParticleGroup::x[n]; //	move last particle into open slot
		  *u = ParticleGroup::u[n];
		  if (qArray) qArray[i] = qArray[n];
		  i--; // Set index to do i again.
		  x--; // Setup to redo the array element
		  u--; // it gets inc in for loop
          q--;
          m++;
		}
		else{
		  if (qArray)
		    localEnergy += qArray[i]**u*uOld;
		  else
		    localEnergy += *u*uOld;
		}
#ifdef DEBUG
		if(strncmp(HOSTTYPE, "alpha", 5) != 0)
		  {
		    if(((*x).e1()>=grid.getJ() || (*x).e1()<=0 || (*x).e2()>=grid.getK()
			|| (*x).e2()<=0)&&!bPtr) {
		      printf("Particle escaped.\n");
		      int *crash = 0;	
		      *crash = 10;
		    }
		  }
#endif //DEBUG
		
	}
//    cout <<" "<<m<<" ";

	if (qArray)
	  species->addKineticEnergy(0.5*localEnergy/q_over_m);
	else
	  species->addKineticEnergy(0.5*localEnergy*get_m());
}  
//    ParticleGroupVAR::~ParticleGroupVAR() {delete[] q;}
    
    BOOL ParticleGroupVAR::addMKS(Vector2 _x, Vector3 _v,Scalar _q)
    {
        if (n<maxN) 
        {
            q[n]=_q;
            if ( ParticleGroup::addMKS( _x,  _v)) {
                return TRUE;
            } ;
        }
        return FALSE;
    }
BOOL ParticleGroupVAR::add(ParticleGroup& p)
{
	if ( p.get_np2c0() == np2c0)
	{
        while (p.get_n() >= 0)
		{
            if (q[p.get_n()]){
                                if(!(add(x[p.get_n()], u[p.get_n()],q[p.get_n()])))	//	did add fail?
                    break;			
            }
            p.set_n(p.get_n()-1);
		}
	p.set_n(p.get_n()+1);										//	restore proper value
	}
    if (p.get_n()) return FALSE;
	else return TRUE;     					//	caller responsible for destructor?
}
//--------------------------------------------------------------------
//	Add a new particle to this group, possibly of variable weight.
//	Note that uNew is gamma*v, and xNew is in grid coords.

BOOL ParticleGroupVAR::add(const Vector2& xNew, const Vector3& uNew,const Scalar& qNew, Scalar np2cNew)
{
	if (n < maxN)
	{
		if (np2cNew > 0)
//			if (qArray)	qArray[n] = q*np2cNew/np2c0;
//			else return FALSE;
	
            if (ParticleGroup::add(xNew,uNew,np2cNew)){
                q[n] = qNew;
                cout<< qNew;
                return TRUE;
            }
	}
	return FALSE;
}
//--------------------------------------------------------------------
//	Add a Particle to this group.

BOOL ParticleGroupVAR::add(ParticleVar* p)
{
    //	if (p->get_q_over_m() == q_over_m)
	if (p->get_speciesID() == get_speciesID() && n < maxN)
//		if (qArray && p->isVariableWeight())
			//		if (qArray)
			return add(p->get_x(), p->get_u(), p->get_np2c(),p->get_q());
		else if (p->get_np2c() == np2c0)
			return add(p->get_x(), p->get_u(),p->get_q());
	return FALSE;
}
  


