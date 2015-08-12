#include "FieldFunction.h"

namespace field
{
	Real	arg(vec<Real,3> pos, c1field f);
	vec<Real,3>	argcolor(vec<Real,3> pos, c1field f);
	vec<Real,3>	hue(vec<Real,3> pos, r1field f);
	vec<Real,3>	stdcolor(vec<Real,3> pos, c1field f,
								Real blackVal, Real fullVal, 
								Real minimumBrightness);
	vec<Real,3>	rgbcolor(vec<Real,3> pos, Real r, Real g, Real b);
	Real	multiplyR1_R(vec<Real,3> pos, r1field f, Real factor);
	Complex	multiplyC1_C(vec<Real,3> pos, c1field f, Real factor_re, Real factor_im);
	Complex	multiplyC1_C1(vec<Real,3> pos, c1field a, c1field b);
	Complex addC1_C(vec<Real,3> pos, c1field f, Real factor_re, Real factor_im);
	Complex	addC1_C1(vec<Real,3> pos, c1field a, c1field b);
	Real realpartC1(vec<Real,3> pos, c1field a);
	Real imagpartC1(vec<Real,3> pos, c1field a);

	Real TestFunction1(vec<Real, 3> pos, Real centerVal, Real r0);

	vec<Real,3> spinor_to_vector(vec<Real, 3> pos,cvecfield<2> c2f);
	
	//void	test2part_1(int& data, r1field f, Real param);
	//Real	test2part(vec<Real,3> pos, int& data, r1field f, Real param);
	
	vec<Real,3> complex_to_RGB(vec<Real,3> pos, c1field f, Real R);
	vec<Real,3> complex_to_RGBp(vec<Real,3> pos, c1field f,
		Real R, Real bmin, Real bmax, Real lmin, Real lmax);
	vec<Real,3> direction_to_RGB(vec<Real,3> pos, rvecfield<3> f);
	vec<Real,3> vector_to_RGB(vec<Real,3> pos, rvecfield<3> f, Real s);
	vec<Real,3> spinor_to_RGB(vec<Real,3> pos, cvecfield<2> f, Real s);
	
	vec<Real,3> cylinder_field(vec<Real,3> pos,Real z);
}
