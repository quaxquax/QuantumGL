#ifndef _H_COMMON_EVAL
#define _H_COMMON_EVAL

#include <exception>

#include "ReferenceCounted.h"

const int maxDimensions = 10;

class TypeMismatch : public std::exception
{
public:
			TypeMismatch();
    virtual const char* what () const throw();
};

class CommonEvaluator : public ReferenceCounted
{
	static int		cacheIDSeed;
	int				cacheID;
public:
	static	void	ClearCachedEvaluations();
			void	SetEvaluationCached();
			bool	IsEvaluationCached();

					CommonEvaluator();

	virtual bool	IsComplex() = 0;
	virtual int		Dimensions() = 0;
	virtual bool	IsField() = 0;
};

#endif
