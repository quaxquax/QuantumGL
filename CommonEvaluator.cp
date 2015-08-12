#include "CommonEvaluator.h"

TypeMismatch::TypeMismatch()
{
}

const char* TypeMismatch::what () const throw()
{
	return "type mismatch.";
}

int CommonEvaluator::cacheIDSeed = 0;

CommonEvaluator::CommonEvaluator()
	: cacheID(-1)
{
}

/* static */
void CommonEvaluator::ClearCachedEvaluations()
{
	cacheIDSeed++;
}

void CommonEvaluator::SetEvaluationCached()
{
	cacheID = cacheIDSeed;
}

bool CommonEvaluator::IsEvaluationCached()
{
	return cacheID == cacheIDSeed;
}
