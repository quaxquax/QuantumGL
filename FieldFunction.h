#ifndef _H_FIELD_FUNCTION
#define _H_FIELD_FUNCTION

#include "QuantumMath.h"
#include "ExpressionEvaluator.h"

#include <cassert>
#include <cstdlib>
#include <algorithm>
using namespace std;

typedef number				Real;
typedef complex<number>		Complex;


template <class T, int n> class vec
{
public:
	T directAccess[n];			// this must be the first member - don't change this
								// (see AdaptedEvaluator<...>::EvaluateReal in
								// FieldFunctionImpl.h if you want to know why...)

	inline T&	operator[] (int x)
	{
		assert(x>=1);		// component indices must be in the
		assert(x<=n);		// range 1..n
		return directAccess[x-1];
	}

	inline T	operator[] (int x) const
	{
		assert(x>=1);		// component indices must be in the
		assert(x<=n);		// range 1..n
		return directAccess[x-1];
	}
	
	inline vec<T,n> operator+ (const vec<T,n> & x) const
	{
		vec<T,n> tmp;
		for(int i=0;i<n;i++)
			tmp.directAccess[i] = directAccess[i] + x.directAccess[i];
		return tmp;
	}
	
	inline vec<T,n> operator- (const vec<T,n> & x) const
	{
		vec<T,n> tmp;
		for(int i=0;i<n;i++)
			tmp.directAccess[i] = directAccess[i] - x.directAccess[i];
		return tmp;
	}
	
	inline vec<T,n> operator* (T x) const
	{
		vec<T,n> tmp;
		for(int i=0;i<n;i++)
			tmp.directAccess[i] = directAccess[i] * x;
		return tmp;
	}
	
	inline vec<T,n> operator/ (T x) const
	{
		return (*this) * (1/x);
	}
	
	/*template <class T2> 
	inline vec<T2,n> map(T2 (*f)(T)) const
	{
		vec<T2,n> tmp;
		for(int i=0;i<n;i++)
			tmp.directAccess[i] = f(directAccess[i]);
		return tmp;
	}
	template <class F> 
	inline vec<F::result_type,n> map(const F& f) const
	{
		vec<F::result_type,n> tmp;
		for(int i=0;i<n;i++)
			tmp.directAccess[i] = f(directAccess[i]);
		return tmp;
	}*/
	
	inline operator vec<Complex,n>()
	{
		vec<Complex,n> tmp;
		for(int i=0;i<n;i++)
			tmp.directAccess[i] = directAccess[i];
		return tmp;
	}
};

template <class T, int n>
inline vec<T,n> operator* (T x, vec<T,n> y)
{
	return y*x;
}

template <int n> inline Real vdot(vec<Real,n> a, vec<Real,n> b)
{
	Real tmp = 0;
	for(int i=0;i<n;i++)
		tmp += a.directAccess[i] * b.directAccess[i];
	return tmp;
}

template <int n> inline Complex vdot(vec<Complex,n> a, vec<Complex,n> b)
{
	Complex tmp = 0;
	for(int i=0;i<n;i++)
		tmp += conj(a.directAccess[i]) * b.directAccess[i];
	return tmp;
}

template <class T, int n>
inline T abs(vec<T, n> a)
{
	return sqrt(vdot(a,a));
}

inline vec<Real,2> makeVecR(Real a, Real b)
{
	vec<Real,2> tmp;
	tmp.directAccess[0] = a;
	tmp.directAccess[1] = b;
	return tmp;
}

inline vec<Real,3> makeVecR(Real a, Real b, Real c)
{
	vec<Real,3> tmp;
	tmp.directAccess[0] = a;
	tmp.directAccess[1] = b;
	tmp.directAccess[2] = c;
	return tmp;
}

inline vec<Real,4> makeVecR(Real a, Real b, Real c, Real d)
{
	vec<Real,4> tmp;
	tmp.directAccess[0] = a;
	tmp.directAccess[1] = b;
	tmp.directAccess[2] = c;
	tmp.directAccess[3] = d;
	return tmp;
}

inline vec<Complex,2> makeVecC(Complex a, Complex b)
{
	vec<Complex,2> tmp;
	tmp.directAccess[0] = a;
	tmp.directAccess[1] = b;
	return tmp;
}

inline vec<Complex,3> makeVecC(Complex a, Complex b, Complex c)
{
	vec<Complex,3> tmp;
	tmp.directAccess[0] = a;
	tmp.directAccess[1] = b;
	tmp.directAccess[2] = c;
	return tmp;
}

inline vec<Complex,4> makeVecC(Complex a, Complex b, Complex c, Complex d)
{
	vec<Complex,4> tmp;
	tmp.directAccess[0] = a;
	tmp.directAccess[1] = b;
	tmp.directAccess[2] = c;
	tmp.directAccess[3] = d;
	return tmp;
}

template <int n>
class rvecfield
{
	ExpressionEvaluator	*expr;
public:
				rvecfield(CommonEvaluator *e)
					: expr(dynamic_cast<ExpressionEvaluator*>(e))
				{
					assert(expr);
					assert(e->Dimensions() == n);
					assert(!e->IsComplex());
				}

	vec<Real,n>	operator()	(vec<Real,3> coord)
	{
		vec<Real,n> tmp;
		vecR3 pos = {	coord.directAccess[0],
						coord.directAccess[1],
						coord.directAccess[2]	};
		
		expr->EvaluateReal(pos, n, tmp.directAccess);
		
		return tmp;
	}
};

template <int n>
class cvecfield
{
	ExpressionEvaluator	*expr;
public:
				cvecfield(CommonEvaluator *e)
					: expr(dynamic_cast<ExpressionEvaluator*>(e))
				{
					assert(expr);
					assert(e->Dimensions() == n);
				}

	vec<Complex,n>	operator()	(vec<Real,3> coord)
	{
		vec<Complex,n> tmp;
		vecR3 pos = {	coord.directAccess[0],
						coord.directAccess[1],
						coord.directAccess[2]	};
		
		expr->EvaluateComplex(pos, n, tmp.directAccess);
		
		return tmp;
	}
};

class r1field
{
	ExpressionEvaluator	*expr;
public:
				r1field(CommonEvaluator *e)
					: expr(dynamic_cast<ExpressionEvaluator*>(e))
				{
					assert(expr);
					assert(e->Dimensions() == 1);
					assert(!e->IsComplex());
				}

	Real	operator()	(vec<Real,3> coord)
	{
		Real tmp;
		vecR3 pos = {	coord.directAccess[0],
						coord.directAccess[1],
						coord.directAccess[2]	};
		
		expr->EvaluateReal(pos, 1, &tmp);
		
		return tmp;
	}
};

class c1field
{
	ExpressionEvaluator	*expr;
public:
				c1field(CommonEvaluator *e)
					: expr(dynamic_cast<ExpressionEvaluator*>(e))
				{
					assert(expr);
					assert(e->Dimensions() == 1);
				}

	Complex	operator()	(vec<Real,3> coord)
	{
		Complex tmp;
		vecR3 pos = {	coord.directAccess[0],
						coord.directAccess[1],
						coord.directAccess[2]	};
		
		expr->EvaluateComplex(pos, 1, &tmp);
		
		return tmp;
	}
};

#endif

