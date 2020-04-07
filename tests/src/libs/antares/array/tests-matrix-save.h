#include<array/fill-matrix.h>
#include<array/matrix-bypass-load.h>
#include<yuni/core/system/stdint.h>
#include<jit/global_JIT_manager.h>

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


