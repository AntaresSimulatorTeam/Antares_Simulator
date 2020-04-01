# include<matrix.h>
#include<enhanced-matrix.h>
#include<yuni/core/system/stdint.h>


struct PredicateIdentity
{
	template<class U> inline U operator () (const U& value) const
	{
		return value;
	}
};



