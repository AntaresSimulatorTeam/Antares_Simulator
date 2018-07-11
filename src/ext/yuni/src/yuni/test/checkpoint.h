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
#ifndef __YUNI_TEST_CHECKPOINT_H__
# define __YUNI_TEST_CHECKPOINT_H__


# define YUNI_TEST_ID_MAXLENGTH 255


namespace Yuni
{
namespace Test
{


	class Checkpoint
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		explicit Checkpoint(const char* id);
		//! Destructor
		~Checkpoint();
		//@}

		bool operator () (bool b);
		bool operator () (bool b, const char* msg);

	private:
		Checkpoint() {}

	private:
		//! ID
		char pID[YUNI_TEST_ID_MAXLENGTH];
		//! Result
		bool pResult;

	}; // class Checkpoint





	class TimedCheckpoint
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		explicit TimedCheckpoint(const char* id);
		//! Destructor
		~TimedCheckpoint();
		//@}

		bool operator () (bool b);
		bool operator () (bool b, const char* msg);

	private:
		TimedCheckpoint() {}

	private:
		//! ID
		char pID[YUNI_TEST_ID_MAXLENGTH];
		//! Start time
		uint pStartTime;

		//! Result
		bool pResult;

	}; // class Checkpoint





} // namespace Test
} // namespace Yuni

#endif // __YUNI_TEST_CHECKPOINT_H__
