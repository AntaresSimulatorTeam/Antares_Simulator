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
#include "program.h"
#include <yuni/io/directory.h>
#include <iostream>

#define YUNI_LIBYUNI_CONFIG_SEPARATORS ", ;:\t\n"



namespace Yuni
{


	LibConfigProgram::LibConfigProgram() :
		pExitStatus(0),
		pOptVersion(false),
		pOptList(false),
		pOptListOnlyVersions(false),
		pOptNoDefaultPath(false),
		pOptModuleList(false),
		pOptDefaultPathList(false),
		pOptCxxFlags(false),
		pOptLibFlags(false),
		pOptPrintCompilerByDefault(false),
		pOptPrintErrors(false),
		pOptPrintModulesDeps(false),
		pOptDebug(false),
		pOptCompiler(YUNI_LIBCONFIG_DEFAULT_COMPILER)
	{
		// Default module
		pOptModules.push_back("core");
	}


	int LibConfigProgram::execute(int argc, char** argv)
	{
		// Parse the command line
		if (not parseCommandLine(argc, argv))
			return pExitStatus;

		// Find the root path
		findRootPath(argv[0]);

		// Display information if asked
		if (not displayInformations())
			return pExitStatus;

		// Expand name for each modules
		expandModuleNames();

		// Display information
		displayInformationAboutYuniVersion();
		return pExitStatus;
	}


	bool LibConfigProgram::parseCommandLine(int argc, char** argv)
	{
		// The Command line parser
		GetOpt::Parser opts;

		// Compiler
		opts.addParagraph("\nTypical usage example :\n  $ yuni-config -c gcc -m script --cxxflags\n\nCompiler settings:");
		opts.addFlag(pOptCxxFlags, ' ', "cxxflags", "Print the CXX flags (*)");
		opts.addFlag(pOptLibFlags, ' ', "libs", "Print the Libs flags (*)");
		opts.add(pOptCompiler, 'c', "compiler", "Set the target compiler (gcc,msvc,mingw,icc,...)");
		opts.addFlag(pOptPrintCompilerByDefault, ' ', "compiler-default", "Print the compiler used by default and exit");

		// All required modules
		opts.addParagraph("\nModules:");
		opts.add(pOptModules, 'm', "modules", "Add one or several requested modules (*)");
		opts.addFlag(pOptModuleList, ' ', "module-list", "Print all available modules of the selected version and exit (*)");
		opts.addFlag(pOptPrintModulesDeps, ' ', "module-deps", "Print all modules and the dependencies required and exit (*)");

		// Prefix
		opts.addParagraph("\nSearching paths:");
		opts.add(pOptPrefix, 'p', "prefix", "Add a prefix folder for searching available versions of libyuni");
		opts.addFlag(pOptNoDefaultPath, ' ', "no-default-path", "Do not use the default paths to find available versions of libyuni");
		opts.addFlag(pOptDefaultPathList, ' ', "default-path-list", "Print the default paths that would be used and exit (empty if 'no-default-path' is enabled)");

		opts.addParagraph("\nVersions:");
		opts.addFlag(pOptList, 'l', "list", "Print the list of all versions of libyuni which have been found (with complete informations) and exit");
		opts.addFlag(pOptListOnlyVersions, ' ', "list-only-versions", "Print the list of all versions of libyuni which have been found and exit");

		// Help
		opts.addParagraph("\nHelp\n  * : Option related to the selected version of libyuni");
		opts.addFlag(pOptPrintErrors, ' ', "verbose", "Print any error message");
		opts.addFlag(pOptDebug, ' ', "debug", "Print debug messages");
		opts.addFlag(pOptVersion, 'v', "version", "Print the version and exit");

		if (!opts(argc, argv))
		{
			pExitStatus = opts.errors() ? 1 /*error*/ : 0;
			if (pOptPrintErrors || pOptDebug)
				std::cout << "Error when parsing the command line\n";
			return false;
		}

		if (pOptPrintCompilerByDefault)
		{
			std::cout << YUNI_LIBCONFIG_DEFAULT_COMPILER << "\n";
			return false;
		}

		// Clean the prefix paths (make them absolute)
		pVersionList.debug(pOptDebug);
		cleanPrefixPaths();
		normalizeCompiler();

		return true;
	}


	void LibConfigProgram::findRootPath(const char* a0)
	{
		# ifdef YUNI_YUNICONFIG_IN_TREE
		(void) a0;
		pRootPath = YUNI_YUNICONFIG_IN_TREE;
		# else

		const String argv0 = a0;
		if (IO::IsAbsolute(argv0))
		{
			IO::ExtractFilePath(pRootPath, argv0);
		}
		else
		{
			IO::Directory::Current::Get(pRootPath);
			pRootPath << IO::Separator;
			String t;
			IO::ExtractFilePath(t, argv0);
			pRootPath += t;
			pRootPath.removeTrailingSlash();
		}
		# endif

		if (pOptDebug)
			std::cout << "[yuni-config][debug] Root path : `" << pRootPath << '`' << std::endl;
	}


	void LibConfigProgram::cleanPrefixPaths()
	{
		if (!pOptPrefix.empty())
		{
			// Current Directory
			String pwd;
			IO::Directory::Current::Get(pwd);

			String tmp;
			const String::List::iterator end = pOptPrefix.end();
			for (String::List::iterator i = pOptPrefix.begin(); i != end; ++i)
			{
				IO::MakeAbsolute(tmp, *i, pwd);
				*i = tmp;
			}
		}
	}


	void LibConfigProgram::initializeSystemPathList()
	{
		if (not pOptNoDefaultPath && pDefaultPathList.empty())
		{
			# ifdef YUNI_OS_WINDOWS
			pDefaultPathList.push_back("${PROGRAMFILES}\\libyuni");
			pDefaultPathList.push_back("${PROGRAMFILES}\\Dev\\libyuni");
			pDefaultPathList.push_back("C:\\Dev\\libyuni");
			# else
			pDefaultPathList.push_back(String() << YUNI_INSTALL_PREFIX << "/include/" << YUNI_VERSIONED_INST_PATH);
			pDefaultPathList.push_back(String() << YUNI_INSTALL_PREFIX << "/include");
			pDefaultPathList.push_back("/usr/include/yuni");
			pDefaultPathList.push_back("/usr/local/include/yuni");
			pDefaultPathList.push_back("/opt/yuni/include");
			# endif
			# ifdef YUNI_OS_MAC
			pDefaultPathList.push_back("/opt/local/include/yuni");
			# endif
		}
	}


	void LibConfigProgram::normalizeCompiler()
	{
		if (pOptCompiler == "g++")
			pOptCompiler = "gcc";
		if (pOptCompiler == "sun++")
			pOptCompiler = "suncc";
		if (pOptCompiler == "i++")
			pOptCompiler = "icc";

		if (pOptDebug)
			std::cout << "[yuni-config][debug] compiler : " << pOptCompiler << std::endl;
	}


	bool LibConfigProgram::displayInformations()
	{
		if (pOptVersion)
		{
			std::cout << YUNI_VERSION_LITE_STRING << "\n";
			return true;
		}
		if (pOptDefaultPathList)
		{
			initializeSystemPathList();
			for (String::List::const_iterator i = pDefaultPathList.begin(); i != pDefaultPathList.end(); ++i)
				std::cout << *i << std::endl;
			return true;
		}

		pVersionList.compiler(pOptCompiler);
		pVersionList.checkRootFolder(pRootPath);
		pVersionList.findFromPrefixes(pOptPrefix);
		if (not pOptNoDefaultPath)
		{
			initializeSystemPathList();
			pVersionList.findFromPrefixes(pDefaultPathList);
		}

		// List
		if (pOptListOnlyVersions)
		{
			pVersionList.print();
			return false;
		}
		if (pOptList)
		{
			pVersionList.printWithInfos();
			return false;
		}
		return true;
	}



	void LibConfigProgram::expandModuleNames()
	{
		String item;

		// Lowercase
		const String::List::iterator end = pOptModules.end();
		for (String::List::iterator i = pOptModules.begin(); i != end; ++i)
		{
			item = *i;
			*i = item.toLower();
		}

		bool mustContinue;
		do
		{
			mustContinue = false;
			const String::List::iterator end = pOptModules.end();
			for (String::List::iterator i = pOptModules.begin(); i != end; ++i)
			{
				if (i->empty())
				{
					// We have no interest in empty strings
					pOptModules.erase(i);
					mustContinue = true;
					break;
				}
				if (String::npos != i->find_first_of(YUNI_LIBYUNI_CONFIG_SEPARATORS))
				{
					// This item seems to be actually a list of several requested modules
					// We will split the string to get the real list
					item = *i;
					pOptModules.erase(i);
					item.split(pOptModules, YUNI_LIBYUNI_CONFIG_SEPARATORS, false, false, true);
					mustContinue = true;
					break;
				}
				if (i->first() == '+')
				{
					// The symbol `+` enables a module, but this is already a default behavior
					item.assign(*i, 1, i->size() - 1);
					*i = item;
					continue;
				}
				if (i->first() == '-')
				{
					// The symbol `-` disables a module, so we have to remove all existing entries
					mustContinue = true;
					item.assign(*i, 1, i->size() - 1);
					pOptModules.erase(i);
					pOptModules.remove(item);
					break;
				}
			}
		} while (mustContinue);
	}


	void LibConfigProgram::displayInformationAboutYuniVersion()
	{
		LibConfig::VersionInfo::Settings* version = pVersionList.selected();
		if (version)
		{
			if (!displayInformationAboutYuniVersion(*version))
				return;
			createArguments(*version);
		}
		else
		{
			pExitStatus = 1;
			if (pOptPrintErrors)
				std::cout << "Error: There is no available version of the yuni library\n";
		}
	}


	void LibConfigProgram::computeDependencies(LibConfig::VersionInfo::Settings& version)
	{
		String::List deps;
		do
		{
			deps.clear();
			{
				const String::List::const_iterator end = pOptModules.end();
				for (String::List::const_iterator i = pOptModules.begin(); i != end; ++i)
				{
					const String::List& modDeps = version.moduleSettings[*i].dependencies;
					const String::List::const_iterator endJ = modDeps.end();
					for (String::List::const_iterator j = modDeps.begin(); j != endJ; ++j)
					{
						if (pOptModules.end() == std::find(pOptModules.begin(), pOptModules.end(), *j))
							deps.push_back(*j);
					}
				}
			}
			if (not deps.empty())
			{
				// Merge results
				const String::List::const_iterator end = deps.end();
				for (String::List::const_iterator i = deps.begin(); i != end; ++i)
				{
					if (pOptModules.end() == std::find(pOptModules.begin(), pOptModules.end(), *i))
						pOptModules.push_back(*i);
				}
			}
		}
		while (not deps.empty());
	}


	void LibConfigProgram::printModulesDependencies() const
	{
		std::map<String, bool> deps;
		{
			const String::List::const_iterator end = pOptModules.end();
			for (String::List::const_iterator i = pOptModules.begin(); i != end; ++i)
				deps[*i] = true;
		}

		bool first = true;
		const std::map<String, bool>::const_iterator end = deps.end();
		for (std::map<String, bool>::const_iterator i = deps.begin(); i != end; ++i)
		{
			if (!first)
				std::cout << ' ';
			std::cout << i->first;
			first = false;
		}
		std::cout << "\n";
	}



	bool LibConfigProgram::checkForDependencies(LibConfig::VersionInfo::Settings& version)
	{
		const String::List::const_iterator end = pOptModules.end();
		for (String::List::const_iterator i = pOptModules.begin(); i != end; ++i)
		{
			if (not isCoreModule(*i) && version.modules.end() == std::find(version.modules.begin(), version.modules.end(), *i))
			{
				pExitStatus = 3;
				//if (pOptPrintErrors)
				std::cerr << "Error: The module '" << *i << "' is required but not available\n";
			}
		}
		return (0 == pExitStatus);
	}


	bool LibConfigProgram::displayInformationAboutYuniVersion(LibConfig::VersionInfo::Settings& version)
	{
		// Getting the config file
		String::List options;
		if (not version.configFile(options, pOptPrintErrors) or not version.parserModulesOptions(options, pOptPrintErrors))
		{
			pExitStatus = 1;
			return false;
		}
		// Dependencies
		computeDependencies(version);
		if (pOptPrintModulesDeps)
		{
			printModulesDependencies();
			checkForDependencies(version);
			return false;
		}
		if (!checkForDependencies(version))
			return false;
		return true;
	}





	static void PrintArgs(const LibConfig::VersionInfo::Settings::SettingsPerModule::OptionMap& args)
	{
		typedef LibConfig::VersionInfo::Settings::SettingsPerModule::OptionMap::const_iterator const_iterator;

		bool first = true;
		const const_iterator end = args.end();
		for (const_iterator i = args.begin(); i != end; ++i)
		{
			if (!first)
				std::cout << ' ';
			first = false;
			std::cout << i->first;
		}
	}


	void LibConfigProgram::createArguments(LibConfig::VersionInfo::Settings& version) const
	{
		LibConfig::VersionInfo::Settings::SettingsPerModule::OptionMap args;
		# if !defined(YUNI_OS_MSVC) && !defined(YUNI_OS_MAC)
		bool hasCxxFlags = false;
		# endif

		if (pOptCxxFlags)
		{
			const String::List::const_iterator end = pOptModules.end();
			for (String::List::const_iterator i = pOptModules.begin(); i != end; ++i)
			{
				LibConfig::VersionInfo::Settings::SettingsPerModule& modSettings = version.moduleSettings[*i];
				modSettings.merge(args, modSettings.cxxFlags);
				modSettings.merge(args, modSettings.defines);
				modSettings.merge(args, modSettings.includes);
			}
			if (!args.empty())
			{
				# if !defined(YUNI_OS_MSVC) && !defined(YUNI_OS_MAC)
				hasCxxFlags = true;
				# endif
				PrintArgs(args);
			}
		}

		if (pOptLibFlags)
		{
			args.clear();
			const String::List::const_iterator end = pOptModules.end();
			for (String::List::const_iterator i = pOptModules.begin(); i != end; ++i)
			{
				LibConfig::VersionInfo::Settings::SettingsPerModule& modSettings = version.moduleSettings[*i];
				modSettings.merge(args, modSettings.frameworks);
				modSettings.merge(args, modSettings.libs);
				modSettings.merge(args, modSettings.libIncludes);
			}
			if (!args.empty())
			{
				# if !defined(YUNI_OS_MSVC) && !defined(YUNI_OS_MAC)
				if (hasCxxFlags)
					std::cout << ' ';
				std::cout << "-Wl,--start-group ";
				# endif
				PrintArgs(args);
				# if !defined(YUNI_OS_MSVC) && !defined(YUNI_OS_MAC)
				std::cout << " -Wl,--end-group";
				# endif
			}
		}
		std::cout << "\n";
	}




} // namespace Yuni
