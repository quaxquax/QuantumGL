#include "ReferenceCounted.h"
#include "AutoreleasePool.h"
#include <cassert>
#include <iostream>
using namespace std;

#define REF_STATISTICS 1

#if REF_STATISTICS
static int nLiveObjects = 0;
static int nAllocatedObjects = 0;
static int nPeakObjects = 0;
static int nLiveReferences = 0;
static int nAllocatedReferences = 0;
static int nPeakReferences = 0;
#endif

ReferenceCounted::ReferenceCounted()
	: refCount(0)
{
#if REF_STATISTICS
	nAllocatedObjects++;
	nLiveObjects++;
	if(nLiveObjects > nPeakObjects)
		nPeakObjects = nLiveObjects;
#endif
	IncRefCount();
}

void ReferenceCounted::IncRefCount()
{
	refCount++;
#if REF_STATISTICS
	nAllocatedReferences++;
	nLiveReferences++;
	if(nLiveReferences > nPeakReferences)
		nPeakReferences = nLiveReferences;
#endif
}

void ReferenceCounted::DecRefCount()
{
#if REF_STATISTICS
	nLiveReferences--;
#endif
	assert(refCount > 0);
	if(!--refCount)
		delete this;
}

void ReferenceCounted::AutoRelease(AutoreleasePool *pool)
{
	if(!pool)
		pool = AutoreleasePool::CurrentPool();
	assert(pool);
	pool->autorelease(this);
}
		
ReferenceCounted::~ReferenceCounted()
{
#if REF_STATISTICS
	nLiveObjects--;
#endif
	assert(refCount <= 1);
}

void PrintReferenceCountingStatistics()
{
#if REF_STATISTICS
	cout << "---- heap stats: ----\n";
	cout << "Objects:    " << nAllocatedObjects << " allocated, "
		<< nLiveObjects << " live, " << nPeakObjects << " peak.\n";
	cout << "References: " << nAllocatedReferences << " allocated, "
		<< nLiveReferences << " live, " << nPeakReferences << " peak.\n";
#endif
}
