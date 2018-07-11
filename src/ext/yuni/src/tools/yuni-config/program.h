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
#ifndef YUNI_NO_THREAD_SAFE
#	define YUNI_NO_THREAD_SAFE // disabling thread-safety
#endif
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/getopt.h>
#include "versions.h"



namespace Yuni
{

	class LibConfigProgram
	{
	public:
		LibConfigProgram();

		int execute(int argc, char** argv);

		bool debug() const {return pOptDebug;}

	private:
		void findRootPath(const char* a0);

		bool parseCommandLine(int argc, char** argv);

		void cleanPrefixPaths();

		void initializeSystemPathList();

		void normalizeCompiler();

		void expandModuleNames();

		bool displayInformations();

		void displayInformationAboutYuniVersion();

		void computeDependencies(LibConfig::VersionInfo::Settings& version);

		void printModulesDependencies() const;

		bool isCoreModule(const String& name) const;

		bool checkForDependencies(LibConfig::VersionInfo::Settings& version);

		bool displayInformationAboutYuniVersion(LibConfig::VersionInfo::Settings& version);

		void createArguments(LibConfig::VersionInfo::Settings& version) const;


	private:
		//! Exit status
		int pExitStatus;
		//! The Root path
		String pRootPath;

		//! Flag: Print the version and exit
		bool pOptVersion;
		//! Flag: Print all available versions of libyuni (with info) and exit
		bool pOptList;
		//! Flag: Print all available versions of libyuni and exit
		bool pOptListOnlyVersions;
		//! Flag: Do not use default paths
		bool pOptNoDefaultPath;
		//! Flag: Print all available modules of the selected versions
		bool pOptModuleList;
		//! Flag: Print all default paths and exit
		bool pOptDefaultPathList;
		//! Flag: Print cxx flags
		bool pOptCxxFlags;
		//! Flag: Print lib flags
		bool pOptLibFlags;
		//! Flag: Print the default compiler
		bool pOptPrintCompilerByDefault;
		//! Flag: Verbose
		bool pOptPrintErrors;
		//! Flag: Print all modules and exit
		bool pOptPrintModulesDeps;
		//! Flag: Debug
		bool pOptDebug;

		//! List of required modules
		String::List pOptModules;
		//! List of given prefix
		String::List pOptPrefix;
		//! List of default paths
		String::List pDefaultPathList;
		//! The complete list of known libyuni versions
		LibConfig::VersionInfo::List pVersionList;
		//! The compiler to use
		String pOptCompiler;

	}; // class Options





} // namespace Yuni

#include "program.hxx"
