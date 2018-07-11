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
#pragma once
#include "../null.h"
#include "../../../io/file.h"
#include <cassert>



namespace Yuni
{
namespace Logs
{

	/*!
	** \brief Log Handler: Single Log file
	*/
	template<class NextHandler = NullHandler>
	class YUNI_DECL File : public NextHandler
	{
	public:
		enum Settings
		{
			// Colors are not allowed in a file
			colorsAllowed = 0,
		};

	public:
		/*!
		** \brief Try to (re)open a target log file
		**
		** You should use an absolute filename to be able to safely reopen it.
		** If a log file was already opened, it will be closed before anything else.
		** If the given filename is empty, true will be returned.
		**
		** \param filename A relative or absolute filename
		** \return True if the operation succeeded, false otherwise
		*/
		bool logfile(const AnyString& filename);

		/*!
		** \brief Get the last opened log file
		** \see outputFilename(filename)
		*/
		String logfile() const;

		/*!
		** \brief Reopen the log file
		**
		** It is safe to call several times this routine.
		** True will be returned if the log filename is empty.
		*/
		bool reopenLogfile();

		/*!
		** \brief Close the log file
		**
		** It is safe to call several times this routine.
		*/
		void closeLogfile();

		/*!
		** \brief Get if a log file is opened
		*/
		bool logfileIsOpened() const;


	public:
		template<class LoggerT, class VerbosityType>
		void internalDecoratorWriteWL(LoggerT& logger, const AnyString& s) const;

	private:
		//! The originale filename
		String pOutputFilename;
		//! File
		mutable IO::File::Stream pFile;

	}; // class File






} // namespace Logs
} // namespace Yuni

#include "file.hxx"

