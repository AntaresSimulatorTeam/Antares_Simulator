/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include "application/application.h"
#include "application/main.h"
#include <yuni/core/getopt.h>
#include <yuni/io/io.h>
#include "../../config.h"
#include "../common/winmain.hxx"
#include <antares/resources/resources.h>
#include <antares/sys/policy.h>
#include <antares/logs.h>
#include <antares/locale.h>

#ifdef YUNI_OS_MSVC
// WxWidgets Stuff
IMPLEMENT_APP(Antares::Application)
# else

# include "../../internet/license.h"
# include <antares/memory/memory.h>


// WxWidgets Stuff
IMPLEMENT_APP_NO_MAIN(Antares::Application)


using namespace Yuni;
using namespace Antares;




int main(int argc, char* argv[])
{

	// Antares SWAP
	if (not memory.initialize())
		return EXIT_FAILURE;

	// We have one or several arguments
	argv = AntaresGetUTF8Arguments(argc, argv);

	// locale
	InitializeDefaultLocale();

	if (argc > 1)
	{
		GetOpt::Parser options;
		//
		options.addParagraph(Yuni::String()
			<< "Antares Simulator v" << ANTARES_VERSION_PUB_STR << "\n");

		// Study
		options.add(Antares::Forms::StudyToLoadAtStartup, 'p', "path", "Path to the study to load");
		options.remainingArguments(Antares::Forms::StudyToLoadAtStartup);
		// Version
		bool optVersion = false;
		options.addFlag(optVersion, 'v', "version", "Print the version and exit");

		// An error has occured
		if (!options(argc, argv))
		{
			FreeUTF8Arguments(argc, argv);
			return options.errors() ? 1 : 0;
		}

		// Version
		if (optVersion)
		{
			std::cout << ANTARES_VERSION_STR << "\n";
			FreeUTF8Arguments(argc, argv);
			return 0;
		}
	}

	// UNIX - Application name
	// For Windows, see application/application.cpp
	logs.applicationName("antares");

	// UNIX - Load the local policy settings
	// For Windows, see application/application.cpp
	LocalPolicy::Open();
	LocalPolicy::CheckRootPrefix(argv[0]);

	// Initialize resources
	Resources::Initialize(argc, argv, true);

	// Running the GUI
	const int ret = wxEntry(argc, argv);

	LocalPolicy::Close();
	FreeUTF8Arguments(argc, argv);
	return ret;
}

#endif // not YUNI_OS_MSVC
