#ifndef __ANTARES_TEST_LIBS_JIT_H__
# define __ANTARES_TEST_LIBS_JIT_H__

#include "../jit/jit.h"


class global_JIT_manager
{
public:
	global_JIT_manager(bool turn_to) : JIT_initial(JIT::enabled)
	{
		JIT::enabled = turn_to;
	}
	~global_JIT_manager() { JIT::enabled = JIT_initial; }
private:
	bool JIT_initial;
};


#endif
