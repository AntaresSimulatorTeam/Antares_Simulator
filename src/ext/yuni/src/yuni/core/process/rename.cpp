/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include "rename.h"
#include "../../thread/mutex.h"

#ifdef YUNI_OS_LINUX
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <limits.h>
# include <sys/user.h>
# include <sys/prctl.h>
#endif




namespace Yuni
{
namespace Process
{


	# ifdef YUNI_OS_LINUX

	namespace // anonymous
	{

		static char **Argv    = NULL;   // pointer to argument vector
		static char *LastArgv = NULL;   // end of argv
		//static char Argv0[128];       // program name


		static inline void LinuxInitializeRenameProcess(int argc, char **argv, char **envp)
		{
			uint i = 0;
			for (; envp[i] != NULL; ++i)
				continue;
			__environ = (char **) malloc(sizeof (char *) * (i + 1));

			for (i = 0; envp[i] != NULL; ++i)
				__environ[i] = strdup(envp[i]);
			__environ[i] = nullptr;

			Argv = argv;
			LastArgv = (i > 0)
				? envp[i - 1] + strlen(envp[i - 1])
				: argv[argc - 1] + strlen(argv[argc - 1]);

			char* tmp = strrchr(argv[0], '/');
			if (!tmp)
				tmp = argv[0];
			else
				++tmp;

			//strncpy(Argv0, tmp, sizeof(Argv0));
			//Argv0[sizeof(Argv0) - 1] = 0;
		}


		static inline void LinuxRenameProcess(const char* text, uint size)
		{
			// TODO Remove this static variable into a dynamicaly allocated one
			enum
			{
				initialBufferSize = 2048,
			};
			static char buffer[initialBufferSize];

			memset(buffer, '\0', sizeof(buffer));
			char* p = buffer;

			uint buffersize = (uint) (sizeof(buffer) - (size_t)(p - buffer) - 1);
			if (size > buffersize)
				size = buffersize;
			YUNI_MEMCPY(p, initialBufferSize, text, size);
			p[size] = '\0';

			for (uint i = 0; Argv[i]; ++i)
				memset(Argv[i], '\0', strlen(Argv[i]));

			(void) strcpy(Argv[0], buffer);
			Argv[1] = nullptr;
		}

	} // anonymous namespace








	char** Rename(int argc, char** argv, const AnyString& newprocessname)
	{
		// Global locking for multithreading
		static Yuni::Mutex globalMutex;
		Yuni::MutexLocker locker(globalMutex);

		static bool initialized = false;
		if (not initialized)
		{
			LinuxInitializeRenameProcess(argc, argv, environ);
			initialized = true;
		}
		LinuxRenameProcess(newprocessname.c_str(), newprocessname.size());
		return argv;
	}



	# else




	char** Rename(int /*argc*/, char** argv, const AnyString& /*newprocessname*/)
	{
		// does nothing
		return argv;
	}

	# endif






} // namespace Process
} // namespace Yuni

