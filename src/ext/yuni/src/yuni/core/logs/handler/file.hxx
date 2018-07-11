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



namespace Yuni
{
namespace Logs
{

	template<class NextHandler>
	bool File<NextHandler>::logfile(const AnyString& filename)
	{
		// Assigning the new filename
		pOutputFilename = filename;
		// Opening the log file
		if (pOutputFilename.empty())
		{
			pFile.close();
			return true;
		}
		return pFile.open(pOutputFilename, IO::OpenMode::write | IO::OpenMode::append);
	}


	template<class NextHandler>
	inline void File<NextHandler>::closeLogfile()
	{
		pFile.close();
	}


	template<class NextHandler>
	inline bool File<NextHandler>::reopenLogfile()
	{
		pFile.close();
		return (pOutputFilename.empty())
			? true
			: pFile.open(pOutputFilename, IO::OpenMode::write | IO::OpenMode::append);
	}


	template<class NextHandler>
	inline String File<NextHandler>::logfile() const
	{
		return pOutputFilename;
	}


	template<class NextHandler>
	inline bool File<NextHandler>::logfileIsOpened() const
	{
		return (pFile.opened());
	}



	template<class NextHandler>
	template<class LoggerT, class VerbosityType>
	void File<NextHandler>::internalDecoratorWriteWL(LoggerT& logger, const AnyString& s) const
	{
		if (pFile.opened())
		{
			typedef typename LoggerT::DecoratorsType DecoratorsType;
			// Append the message to the file
			logger.DecoratorsType::template internalDecoratorAddPrefix<File, VerbosityType>(pFile, s);

			// Flushing the result
			# ifdef YUNI_OS_WINDOWS
			pFile << "\r\n";
			# else
			pFile << '\n';
			# endif
			pFile.flush();
		}

		// Transmit the message to the next handler
		NextHandler::template internalDecoratorWriteWL<LoggerT, VerbosityType>(logger, s);
	}





} // namespace Logs
} // namespace Yuni

