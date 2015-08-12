#ifndef _H_QUANTUM_MATH
#define _H_QUANTUM_MATH

typedef float number;
#include <math.h>
#include <complex>
using std::complex;
#include <utility>
#include <cassert>
//#include <limits>
using std::pair;
#ifndef assert
using std::assert;
#endif
//using std::numeric_limits;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef __FP__
const number pi = number(M_PI);
#endif

struct vecC2
{
	complex<number> z1,z2;
	
	inline vecC2 operator+ (vecC2 v2)
	{
		vecC2 tmp;
		tmp.z1 = z1+v2.z1;
		tmp.z2 = z2+v2.z2;
		return tmp;
	}
	inline vecC2 operator* (complex<number> c)
	{
		vecC2 tmp;
		tmp.z1 = z1*c;
		tmp.z2 = z2*c;
		return tmp;
	}
};
struct vecR3
{
	number x,y,z;
};
const vecR3 nullVecR3 = {0,0,0};

struct vecN3
{
	int x,y,z;
};
const vecN3 nullVecN3 = {0,0,0};

class Range : public pair<number,number>
{
public:
	Range() {}
	Range(number a, number b) : pair<number,number>(a,b) {}
	inline bool inside(const Range& other)
	{
		assert(second >= first);
		if(first < other.first) return false;
		if(second > other.second) return false;
		return true;
	}
	inline number size()
	{
		assert(second >= first);
		return second - first;
	}
	inline number clamp(number x)
	{
		return x < first ? first : x > second ? second : x;
	}
};

inline Range intersectRanges(const Range& a, const Range& b)
{
	Range dst;
	dst.first = a.first > b.first ? a.first : b.first;
	dst.second = a.second > b.second ? b.second : a.second;
	return dst;
}

#ifndef INFINITY
//#define INFINITY infinityf()
#define INFINITY HUGE_VAL
#endif

const number	numberPlusInfinity = INFINITY;
const number	numberMinusInfinity = -INFINITY;

const Range		fullRange(numberMinusInfinity,numberPlusInfinity);

#ifndef __GNUC__

inline
complex<number>
operator*(const number& lhs, const complex<number>& rhs)
{
	return complex<number>(lhs * rhs.real(), lhs * rhs.imag());
}

inline
complex<number>
operator*(const complex<number>& lhs, const number& rhs)
{
	return complex<number>(lhs.real() * rhs, lhs.imag() * rhs);
}

#endif

#endif
