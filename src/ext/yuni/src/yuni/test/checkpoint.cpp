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
#include "../yuni.h"
#include "checkpoint.h"
#include <iostream>
#include <string.h>
#include <time.h>
#ifndef YUNI_OS_WINDOWS
#	include <sys/time.h>
#endif
#include "../core/system/gettimeofday.h"


#define RESULT_TO_STR(X) ((X) ? "ok" : "failed")


namespace Yuni
{
namespace Test
{

	namespace
	{

		uint GetCurrentTime()
		{
			timeval tv;
			gettimeofday(&tv, 0);
			return (uint)tv.tv_sec * 1000  + (uint)tv.tv_usec / 1000;
		}

	} // anonymous namespace



	Checkpoint::Checkpoint(const char* id)
		:pResult(true)
	{
		// ID
		# ifdef YUNI_OS_MSVC
		strncpy_s(pID, sizeof(pID), id, YUNI_TEST_ID_MAXLENGTH - 1);
		# else
		::strncpy(pID, id, YUNI_TEST_ID_MAXLENGTH - 1);
		# endif
		// Ensure it is zero-terminated (should not be necessary however)
		pID[YUNI_TEST_ID_MAXLENGTH - 1] = '\0';
		// Tag the outputs
		std::cout << "[test:cout] <begin> " << pID << "\n";
		std::cerr << "[test:cerr] <begin> " << pID << "\n";
	}


	Checkpoint::~Checkpoint()
	{
		// \internal At the end of each checkpoint we force the
		// flush for stdout and stderr
		std::cout << "[test:cout] <end," << RESULT_TO_STR(pResult) << "> " << pID << std::endl;
		std::cerr << "[test:cerr] <end," << RESULT_TO_STR(pResult) << "> " << pID << std::endl;
	}


	bool Checkpoint::operator () (bool b)
	{
		if (!b)
			pResult = false;
		std::cout << "[test:cout] <check," << RESULT_TO_STR(b) << ">\n";
		std::cerr << "[test:cerr] <check," << RESULT_TO_STR(b) << ">\n";
		return b;
	}


	bool Checkpoint::operator () (bool b, const char* msg)
	{
		if (!b)
			pResult = false;
		std::cout << "[test:cout] <check," << RESULT_TO_STR(b) << ">" << msg << "\n";
		std::cerr << "[test:cerr] <check," << RESULT_TO_STR(b) << ">" << msg << "\n";
		return b;
	}



	TimedCheckpoint::TimedCheckpoint(const char* id)
		:pResult(true)
	{
		// ID
		# ifdef YUNI_OS_MSVC
		strncpy_s(pID, sizeof(pID), id, YUNI_TEST_ID_MAXLENGTH - 1);
		# else
		::strncpy(pID, id, YUNI_TEST_ID_MAXLENGTH - 1);
		# endif
		// Ensure it is zero-terminated (should not be necessary however)
		pID[YUNI_TEST_ID_MAXLENGTH - 1] = '\0';
		// Tag the outputs
		std::cout << "[test:cout] <begin> " << pID << "\n";
		std::cerr << "[test:cerr] <begin> " << pID << "\n";

		// Start time
		pStartTime = GetCurrentTime();
	}


	TimedCheckpoint::~TimedCheckpoint()
	{
		// Duration
		uint duration = GetCurrentTime() - pStartTime;
		// \internal At the end of each checkpoint we force the
		// flush for stdout and stderr
		std::cout << "[test:cout] <end," << RESULT_TO_STR(pResult) << "," << duration << "ms> " << pID << std::endl;
		std::cerr << "[test:cerr] <end," << RESULT_TO_STR(pResult) << "," << duration << "ms> " << pID << std::endl;
	}


	bool TimedCheckpoint::operator () (bool b)
	{
		if (!b)
			pResult = false;
		std::cout << "[test:cout] <check," << RESULT_TO_STR(b) << ">\n";
		std::cerr << "[test:cerr] <check," << RESULT_TO_STR(b) << ">\n";
		return b;
	}


	bool TimedCheckpoint::operator () (bool b, const char* msg)
	{
		if (!b)
			pResult = false;
		std::cout << "[test:cout] <check," << RESULT_TO_STR(b) << ">" << msg << "\n";
		std::cerr << "[test:cerr] <check," << RESULT_TO_STR(b) << ">" << msg << "\n";
		return b;
	}




} // namespace Test
} // namespace Yuni
