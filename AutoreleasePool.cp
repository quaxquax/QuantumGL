#include "AutoreleasePool.h"
#include "ReferenceCounted.h"

#include <functional>
#include <algorithm>
#include <cassert>
using namespace std;

stack<AutoreleasePool*> AutoreleasePool::pools;
	
AutoreleasePool* AutoreleasePool::CurrentPool()
{
	assert(!pools.empty());
	return pools.top();
}

void AutoreleasePool::autorelease(ReferenceCounted* obj)
{
	released.push_front(obj);
}

void AutoreleasePool::Flush()
{
	for_each(released.begin(), released.end(),
			mem_fun(&ReferenceCounted::DecRefCount));
	released.clear();
}
	
void AutoreleasePool::PushNewPool()
{
	pools.push(new AutoreleasePool());
}

void AutoreleasePool::FlushAndPopPool()
{
	assert(!pools.empty());
	pools.top()->Flush();
	pools.pop();
}

void AutoreleasePool::FlushCurrentPool()
{
	assert(!pools.empty());
	pools.top()->Flush();
}
