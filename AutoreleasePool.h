#ifndef _H_AUTORELEASE_POOL
#define _H_AUTORELEASE_POOL

#include <list>
#include <stack>
using std::list;
using std::stack;

class ReferenceCounted;


class AutoreleasePool
{
	list<ReferenceCounted*> released;
	static stack<AutoreleasePool*> pools;
	
	static AutoreleasePool* CurrentPool();
	void	autorelease(ReferenceCounted* obj);

	friend class ReferenceCounted;
public:
	void	Flush();
	
	static void PushNewPool();
	static void FlushAndPopPool();
	static void FlushCurrentPool();
};

#endif
