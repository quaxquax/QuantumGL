#ifndef _H_ANIMATOR
#define _H_ANIMATOR

#include "QuantumMath.h"

#include <vector>
using std::vector;

class RealVariable;

class AnimatorVariableSpec
{
public:
	RealVariable *var;
	Range		range;
	bool		inclusive;
	
	void		SetVariable(int frame, int n);
};

class AnimationSection
{
public:
	vector<AnimatorVariableSpec>	vars;
	int								frames;

	bool	GenerateAnimation(int startingFrame);
	void	SetFrame(int frame);
};

class Animator
{
public:
	vector<AnimationSection>		sections;
	int								frames;
	
	void	GenerateAnimation();
	int		GetFrameCount();
	void	SetFrame(int f);
};

#endif
