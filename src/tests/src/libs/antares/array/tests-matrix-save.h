#include "fill-matrix.h"
#include "matrix-bypass-load.h"
#include <yuni/core/system/stdint.h>
#include <global_JIT_manager.h>

#include<string>
#include<vector>
using namespace std;
using namespace Yuni;
using namespace Antares;


struct TSNumbersPredicate
{
	uint32 operator () (uint32 value) const
	{
		return value + 1;
	}
};

struct PredicateIdentity
{
    template<class U>
    inline U operator()(const U& value) const
    {
        return value;
    }
};

