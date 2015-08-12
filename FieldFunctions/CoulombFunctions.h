#include "FieldFunction.h"

namespace field
{
	Complex	CoulombFunction(vec<Real, 3> pos, Real n, Real ell, Real m, Real g);
	Complex	CoulombFunctionT(vec<Real, 3> pos, Real n, Real ell, Real m, Real g, Real t);
	Complex CoulombParabolicEF(vec<Real, 3> pos, Real n1, Real n2, Real m);

	vec<Complex,2> CoulombSpinorUp(vec<Real, 3> pos, Real n, Real j, Real mj, Real g);
	vec<Complex,2> CoulombSpinorDown(vec<Real, 3> pos, Real n, Real j, Real mj, Real g);
	
	vec<Complex,2> CoulombSpinor(vec<Real, 3> pos, Real n, Real kappa, Real mj, Real g);
}
