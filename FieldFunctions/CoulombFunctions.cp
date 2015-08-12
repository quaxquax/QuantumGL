#include "CoulombFunctions.h"

static Real hypergeometric1F1(int a, int b, Real x);
static Real hypergeometric2F1(int a, int c, int b, Real x);
static Real CoulombEnergy(int n, Real g);
static Real radialCoulomb(int n, int l, Real g, Real r);
static Complex sphericalHarmonicY(int ell, int m, Real theta, Real phi);

using namespace std;

#define gamma DONT-USE-THIS-EVIL-THING!!!

inline Real Gamma(int n)
{
	static Real buf[100];
	static bool inited = false;

	if(n <= 0)
		return 1./0;

	if(!inited)
	{
		buf[0] = 1;
		for(int i=1;i<100;i++)
			buf[i] = i*buf[i-1];
		inited = true;
	}
	if(n-1 < 100)
		return buf[n-1];
	else
	{
		Real x = buf[99];
		for(int i=100;i<n;i++)
			x *= i;
		return x;
	}
}

inline void	ConvertToSpherical(Real &r, Real &theta, Real &phi, vec<Real, 3> pos)
{
	r = abs(pos);
	if(r != 0)
	{
		theta = acos(pos[3]/r);
		if(pos[1] != 0 || pos[2] != 0)
			phi = atan2(pos[2],pos[1]);
		else
			phi = 0;
	}
	else
	{
		theta = 0;
		phi = 0;
	}
}

static Real hypergeometric1F1(int a, int b, Real x)
{
	int k, n = -a;
	Real result = 1;
	for(k=0; k<n; k++)
	{
		result = result*x*(a+n-1-k)/((Real)(b+n-1-k)*(n-k)) + 1;
	}
	return result;
}

static Real hypergeometric2F1(int a, int c, int b, Real x)
{
	int k, n = -a;
	Real result = 1;
	for(k=0; k<n; k++)
	{
		result = result*x*(a+n-1-k)*(c+n-1-k)/((Real)(b+n-1-k)*(n-k)) + 1;
	}
	return result;
}

static Real CoulombEnergy(int n, Real g)
{
	return 0.5*g*g/(n*n);
}

static Real radialCoulomb(int n, int l, Real g, Real r)
{
	Real k1 = 2*g/n;    //k1 = 2*sqrt(2*CoulombEnergy)
	Real x = k1*r;
	Real k2 = sqrt(k1);
	Real k3 = k2*k2*k2;
	Real norm = sqrt(Gamma(n + l + 1)/(2*n*Gamma(n-l)))/Gamma(2*l+2);
	return norm*k3*pow(x,l)*exp(-.5*x)*hypergeometric1F1(l+1-n,2*l+2,x);
}

static Complex sphericalHarmonicY(int ell, int m, Real theta, Real phi)
{
	int mabs = abs(m);
	
	Real result =
		sqrt(
			(2*ell + 1) * Gamma(ell+mabs+1) / (4 * M_PI * Gamma(ell-mabs+1))
		)
		*
	    pow( sin(theta) , mabs ) / ( pow((Real)-2,mabs) * Gamma(mabs+1) )
	    *
	    hypergeometric2F1(mabs-ell, mabs+ell+1, mabs+1, (1-cos(theta))/2);
	
	if(m>=0)
		return Complex( result*cos(mabs*phi) , result*sin(mabs*phi) );
	else
		return ((Real)(mabs%2?-1:1))*Complex(result*cos(mabs*phi),-result*sin(mabs*phi));
}

static vec<Complex,2> sphericalSpinorUp(Real j, Real mj, Real theta, Real phi)
{
	int ell = int(j-1./2);
	int mUpper = int(mj-1./2);
	int mLower = int(mj+1./2);
	
	Complex upper, lower;
	
	if(mj == -j)
		upper = Complex(0,0);
	else
		upper = sqrt((j+mj)/(2*j)) * sphericalHarmonicY(ell,mUpper,theta,phi);
	
	if(mj == j)
		lower = Complex(0,0);
	else
		lower = sqrt((j-mj)/(2*j)) * sphericalHarmonicY(ell,mLower,theta,phi);
	
	return makeVecC(upper, lower);
}

static vec<Complex,2> sphericalSpinorDown(Real j, Real mj, Real theta, Real phi)
{
	int ell = int(j+1./2);
	int mUpper = int(mj-1./2);
	int mLower = int(mj+1./2);
		
	Complex upper, lower;
	
	if(mj == j+1)
		upper = Complex(0,0);
	else
		upper = -sqrt((j+1-mj)/(2*j+2)) * sphericalHarmonicY(ell,mUpper,theta,phi);
		
	if(mj == -j-1)
		lower = Complex(0,0);
	else
		lower = sqrt((j+1+mj)/(2*j+2)) * sphericalHarmonicY(ell,mLower,theta,phi);
	
	return makeVecC(upper, lower);
}

static vec<Complex,2> SpinorHarmonic(Real kappa, Real mj, Real theta, Real phi)
{
	int ellUp = int(kappa - 1.);
	int ellDown = int(-kappa);
	int mUpper = int(mj-1./2);
	int mLower = int(mj+1./2);
	
	Complex upper, lower;
	
	if( abs(mj) > abs(kappa)-1./2 )
		return makeVecC(Complex(0,0), Complex(0,0));
	
	if(kappa < 0)
	{
		upper = -sqrt((Real(1./2)-kappa-mj)/(1-2*kappa)) * sphericalHarmonicY(ellDown,mUpper,theta,phi);
		lower =  sqrt((Real(1./2)-kappa+mj)/(1-2*kappa)) * sphericalHarmonicY(ellDown,mLower,theta,phi);
	}
	else if(kappa > 0)
	{
		upper =  sqrt((kappa-Real(1./2)+mj)/(2*kappa-1)) * sphericalHarmonicY(ellUp , mUpper,theta,phi);
		lower =  sqrt((kappa-Real(1./2)-mj)/(2*kappa-1)) * sphericalHarmonicY(ellUp , mLower,theta,phi);
	}
	else
	{
		lower = Complex(0,0);
		upper = Complex(0,0);
	}
	return makeVecC(upper, lower);
}

static vec<Complex,2> CoulombSpinorUpPolar(int n, Real j, Real mj, Real g, Real r, Real theta, Real phi)
{
	return Complex(radialCoulomb(n, int(j-1./2), g, r)) * sphericalSpinorUp(j,mj,theta,phi);
}

static vec<Complex,2> CoulombSpinorDownPolar(int n, Real j, Real mj, Real g, Real r, Real theta, Real phi)
{
	return Complex(radialCoulomb(n, int(j+1./2), g, r)) * sphericalSpinorDown(j,mj,theta,phi);
}

vec<Complex,2> field::CoulombSpinorUp(vec<Real, 3> pos, Real n, Real j, Real mj, Real g)
{
	Real r, theta, phi;
	ConvertToSpherical(r,theta,phi,pos);
	
	return Complex(radialCoulomb(int(n), int(j-1./2), g, r)) * sphericalSpinorUp(j,mj,theta,phi);
}

vec<Complex,2> field::CoulombSpinorDown(vec<Real, 3> pos, Real n, Real j, Real mj, Real g)
{
	Real r, theta, phi;
	ConvertToSpherical(r,theta,phi,pos);
	
	return Complex(radialCoulomb(int(n), int(j+1./2), g, r)) * sphericalSpinorDown(j,mj,theta,phi);
}

vec<Complex,2> field::CoulombSpinor(vec<Real, 3> pos, Real n, Real kappa, Real mj, Real g)
{
	Real r, theta, phi;
	ConvertToSpherical(r,theta,phi,pos);
	
	if(kappa < 0.)
		return Complex(radialCoulomb(int(n), -int(kappa), g, r)) * SpinorHarmonic(kappa,mj,theta,phi);
	else if(kappa > 0.)
		return Complex(radialCoulomb(int(n), int(kappa)-1, g, r)) * SpinorHarmonic(kappa,mj,theta,phi);
	else
		return makeVecC(Complex(0,0),Complex(0,0));
}


Complex field::CoulombFunction(vec<Real, 3> pos, Real n, Real ell, Real m, Real g)
{
	Real r, theta, phi;
	ConvertToSpherical(r,theta,phi,pos);
	
	return radialCoulomb(int(n), int(ell), g, r) * sphericalHarmonicY(int(ell),int(m),theta,phi);
}

/*
static Real parabolicCoulombF(int n1, int n2, int m, Real zeta)
{
	Real mabs = abs(m);
	int N = n1+n2+mabs+1;
	Real norm = 1.0;
	return norm*exp(-zeta/(2*N))*pow(zeta,mabs/2)*hypergeometric1F1(-n1,1+mabs,zeta/N);
}

static Real parabolicCoulombG(int n1, int n2, int m, Real eta)
{
	Real mabs = abs(m);
	int N = n1+n2+mabs+1;
	Real norm = 1.0;
	return norm*exp(-eta/(2*N))*pow(eta,mabs/2)*hypergeometric1F1(-n2,1+mabs,eta/N);
}
*/

static Real parabolicCoulomb(int n, int N, int m, Real zeta)
{
	return exp(-zeta/(2*N))*pow(zeta,(Real)(m/2.))*hypergeometric1F1(-n,1+m,zeta/N);
}


Complex field::CoulombParabolicEF(vec<Real, 3> pos, Real n1, Real n2, Real m)
{
	int mabs = int(abs(m));
	int N = int(n1)+int(n2)+mabs+1;
	Real nrm = sqrt(Gamma(n1+mabs+1)*Gamma(n2+mabs+1)/(M_PI*Gamma(n1+1)*Gamma(n2+1)))/(pow((Real)N,mabs+2)*Gamma(mabs+1)*Gamma(mabs+1));
	Real r = abs(pos);
	Real zeta = r+pos[3];
	Real eta = r-pos[3];
	Real phi;
	if(pos[1] != 0 || pos[2] != 0)
		phi = atan2(pos[2],pos[1]);
	else
		phi = 0;
	
	Real pc1 = parabolicCoulomb(int(n1), N, mabs, zeta);
	Real pc2 = parabolicCoulomb(int(n2), N, mabs, eta);
	
	Complex prod = (int(m)%2 ? -1 : 1)*nrm*pc1 *
			pc2 *
			Complex(cos(m*phi),sin(m*phi));
	return prod;
	
	/*return (int(m)%2 ? -1 : 1)*nrm*parabolicCoulomb(int(n1), N, mabs, zeta) *
			parabolicCoulomb(int(n2), N, mabs, eta) *
			Complex(cos(m*phi),sin(m*phi));*/
}

Complex field::CoulombFunctionT(vec<Real, 3> pos, Real n, Real ell, Real m, Real g, Real t)
{
	Real r, theta, phi;
	ConvertToSpherical(r,theta,phi,pos);
	
	Real argument = CoulombEnergy(int(n),g) * t;
	return radialCoulomb(int(n), int(ell), g, r) * sphericalHarmonicY(int(ell),int(m),theta,phi) *
		Complex( cos(argument) , sin(argument) );
}

