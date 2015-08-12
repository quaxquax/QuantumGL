#include "FieldFunction.h"
#include "Hydrogen553.h"

#include <iostream>
using namespace std;

Complex field::hydrogen553(vec<Real,3> pos)
{
	const number theMax = 270;
	number x = pos[1]*theMax;
	number y = pos[2]*theMax;
	number z = pos[3]*theMax;

	number rad = sqrt(x*x+y*y+z*z);
	number zfac = sqrt(rad*rad - z*z);
	number func = 0.0004083144*exp(-rad/11.)
				*(1. - 5*rad/66. + 10*rad*rad/4719.
				 - 10*rad*rad*rad/363363. + 2*rad*rad*rad*rad/11990979. 
				 - rad*rad*rad*rad*rad/2638015380.)*
	               (rad*rad - 9*z*z)*zfac*zfac*zfac;
	
	// Psi(nradial = 2, el = 3, m = 2) + Psi(nradial = 3, el = 2, m = -1)
	
	return
		Complex(
	         func*cos(3*atan2(pos[1],pos[2]))
    		,
    		 func*sin(3*atan2(pos[1],pos[2]))
    	);
}
