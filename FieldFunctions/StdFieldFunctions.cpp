#include "FieldFunction.h"
#include "StdFieldFunctions.h"

Real	field::arg(vec<Real,3> pos, c1field f)
{
	return arg(f(pos));
}

static vec<Real,3> hsbColor(Real h, Real s, Real b)
{
	Real mh = fmod(h / (2*pi),(Real) 1.);
	if(mh < 0) mh += 1;
	if(mh >= 1) mh -= 1;
	mh *= 6;
	
	Real i = floor(mh);
	Real f = mh - i;
	Real p = b*(1 - s);
	Real q = b*(1 - s*f);
	Real t = b*(1 - s * (1-f) );
	switch(int(i))
	{
		case 0:	return makeVecR(b,t,p);
		case 1: return makeVecR(q,b,p);
		case 2: return makeVecR(p,b,t);
		case 3: return makeVecR(p,q,b);
		case 4: return makeVecR(t,p,b);
		case 5: return makeVecR(b,p,q);
	}
	return makeVecR(0,0,0);
}

vec<Real,3>	field::hue(vec<Real,3> pos, r1field f)
{
	return hsbColor(f(pos),1,1);
}

vec<Real,3>	field::argcolor(vec<Real,3> pos, c1field f)
{
	return hsbColor(arg(f(pos)),1,1);
}

vec<Real,3>	field::rgbcolor(vec<Real,3> /*pos*/, Real r, Real g, Real b)
{
	return makeVecR(r,g,b);
}

vec<Real,3>	field::stdcolor(vec<Real,3> pos, c1field f,
							Real blackVal, Real fullVal, 
							Real minimumBrightness)
{
	Complex z = f(pos);
	Real b = ((abs(z)-blackVal) / (fullVal-blackVal))
			 * (1-minimumBrightness) + minimumBrightness;
	
	return hsbColor(arg(z),1,b < minimumBrightness ? minimumBrightness
								 : (b > 1 ? 1 : b));
}

Real field::multiplyR1_R(vec<Real,3> pos, r1field f, Real factor)
{
	return f(pos)*factor;
}

Complex field::multiplyC1_C(vec<Real,3> pos, c1field f, Real factor_re, Real factor_im)
{
	return f(pos)*Complex(factor_re, factor_im);
}

Complex field::multiplyC1_C1(vec<Real,3> pos, c1field a, c1field b)
{
	return a(pos)*b(pos);
}

Complex field::addC1_C(vec<Real,3> pos, c1field f, Real factor_re, Real factor_im)
{
	return f(pos) + Complex(factor_re, factor_im);
}

Complex field::addC1_C1(vec<Real,3> pos, c1field a, c1field b)
{
	return a(pos) + b(pos);
}

Real field::realpartC1(vec<Real,3> pos, c1field a)
{
	return a(pos).real();
}

Real field::imagpartC1(vec<Real,3> pos, c1field a)
{
	return a(pos).imag();
}

/*
void field::test2part_1(int& data, r1field f, Real param)
{
	data = (int)param;
}
Real field::test2part(vec<Real,3> pos, int& data, r1field f, Real param)
{
	return f(pos)+data;
}*/

Real field::TestFunction1(vec<Real, 3> pos, Real centerVal, Real r0)
{
	Real r2 = vdot(pos,pos);
	if(r2 > r0*r0)
		return 0;
	else
	{
		Real r = sqrt(r2);
		return (r0-r)/r0 * centerVal;
	}
}

vec<Real,3> field::spinor_to_vector(vec<Real, 3> pos,cvecfield<2> c2f)
{
	vec<Complex,2> c = c2f(pos);
	return makeVecR(
			real(vdot(c,makeVecC(c[2],c[1]))),
			real(vdot(c,makeVecC(Complex(0,-1)*c[2],Complex(0,1)*c[1]))),
			real(vdot(c,makeVecC(c[1],-c[2])))
		);
}

vec<Real,3> field::complex_to_RGB(vec<Real,3> pos, c1field f, Real R)
{
	return complex_to_RGBp(pos, f, R, 0., 1., 0., 1.);
}

vec<Real,3> field::complex_to_RGBp(vec<Real,3> pos, c1field f,
		Real R, Real bmin, Real bmax, Real lmin, Real lmax)
{
	Real h = fmod(arg(f(pos)) / (2*pi), (Real) 1.);
	if(h < 0) h += 1;
	if(h >= 1) h -= 1;
	
	Real r = abs(f(pos));
	
	Real l = lmin + (lmax -lmin)*
		min((Real)1., max((Real)0., (Real) ((2/pi)*atan(r/R)*(bmax-bmin) + bmin)));
	
	if(l <= 0.5)
	{
		switch((int)(6*h))
		{
			case 0:	return makeVecR(2*l, 12*h*l, 0);
			case 1:	return makeVecR(4*(1 - 3*h)*l, 2*l, 0);
			case 2:	return makeVecR(0, 2*l, 4*(3*h - 1)*l);
			case 3:	return makeVecR(0, 4*(2 - 3*h)*l, 2*l);
			case 4:	return makeVecR(4*(3*h - 2)*l, 0, 2*l);
			case 5:	return makeVecR(2*l, 0, 12*(1 - h)*l);
		}
	}
	else
	{
		switch((int)(6*h))
		{
			case 0:	return makeVecR(1, 2*l - 1 - 12*(l - 1)*h, 2*l - 1);
			case 1:	return makeVecR(3 + 12*(l - 1)*h - 2*l, 1, 2*l - 1);
			case 2:	return makeVecR(2*l - 1, 1, 6*l - 5 - 12*(l - 1)*h);
			case 3:	return makeVecR(2*l - 1, 7 + 12*(l - 1)*h - 6*l, 1);
			case 4:	return makeVecR(10*l - 9 - 12*(l - 1)*h, 2*l - 1, 1);
			case 5:	return makeVecR(1, 2*l - 1, 11 + 12*(l - 1)*h - 10*l);
		}
	}
	return makeVecR(0,0,0);
}

inline Real invabs(Real u)
{
	return u == 0 ? 1 : 0.5/abs(u);
}

inline Real fact(Real r, Real s)
{
	return 1 - 1/((r*r)/(s*s) + 1);
}

vec<Real,3> field::direction_to_RGB(vec<Real,3> pos, rvecfield<3> f)
{
	vec<Real,3> vf = f(pos);
	Real absv = abs(vf);
	if(absv == 0) return makeVecR(0.5, 0.5, 0.5);
	vec<Real,3> v1 = vf * (1/absv);
	vec<Real,3> vrot = makeVecR(0.81649658*v1[1] + 0.57735027*v1[3],
								0.57735027*v1[3] + 0.70710678*v1[2] - 0.40824829*v1[1],
								0.57735027*v1[3] - 0.70710678*v1[2] - 0.40824829*v1[1]);
	Real dist = min(min(invabs(vrot[1]), invabs(vrot[2])), invabs(vrot[3]));

	return makeVecR(0.5 + vrot[1]*dist, 0.5 + vrot[2]*dist, 0.5 + vrot[3]*dist);
}

vec<Real,3> field::vector_to_RGB(vec<Real,3> pos, rvecfield<3> f, Real s)
{
	vec<Real,3> vf = f(pos);
	Real absv = abs(vf);
	if(absv == 0) return makeVecR(0.5, 0.5, 0.5);
	vec<Real,3> v1 = vf * (1/absv);
	vec<Real,3> vrot = makeVecR(0.81649658*v1[1] + 0.57735027*v1[3],
								0.57735027*v1[3] + 0.70710678*v1[2] - 0.40824829*v1[1],
								0.57735027*v1[3] - 0.70710678*v1[2] - 0.40824829*v1[1]);
	Real dist = min(min(invabs(vrot[1]), invabs(vrot[2])), invabs(vrot[3]));

	return makeVecR(0.5 + vrot[1]*dist*fact(absv,s), 0.5 + vrot[2]*dist*fact(absv,s),
					0.5 + vrot[3]*dist*fact(absv,s));
}

vec<Real,3> field::spinor_to_RGB(vec<Real,3> pos, cvecfield<2> f, Real s)
{
	vec<Real,3> vf = spinor_to_vector(pos,f);
	Real absv = abs(vf);
	if(absv == 0) return makeVecR(0.5, 0.5, 0.5);
	vec<Real,3> v1 = vf * (1/absv);
	vec<Real,3> vrot = makeVecR(0.81649658*v1[1] + 0.57735027*v1[3],
								0.57735027*v1[3] + 0.70710678*v1[2] - 0.40824829*v1[1],
								0.57735027*v1[3] - 0.70710678*v1[2] - 0.40824829*v1[1]);
	Real dist = min(min(invabs(vrot[1]), invabs(vrot[2])), invabs(vrot[3]));
	return makeVecR(0.5 + vrot[1]*dist*fact(absv,s), 0.5 + vrot[2]*dist*fact(absv,s),
					0.5 + vrot[3]*dist*fact(absv,s));
}

vec<Real,3> field::cylinder_field(vec<Real,3> pos, Real z)
{
	return makeVecR(-pos[2],pos[1],z);
}
