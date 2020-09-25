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
#ifndef __ANTARES_LIBS_STUDY_SCENARIO_BUILDER_UPDATER_HXX__
# define __ANTARES_LIBS_STUDY_SCENARIO_BUILDER_UPDATER_HXX__

# include <yuni/yuni.h>
# include "../study.h"
# include "sets.h"

# define SEP IO::Separator


namespace Antares
{

	namespace // anonymous
	{

		class ScenarioBuilderUpdater
		{
		public:
			ScenarioBuilderUpdater(Data::Study& study) :
				pStudy(study)
			{
				using namespace Yuni;
				// We can store the INI files in disk because it may not fit in memory

				if (study.scenarioRules)
				{
					logs.debug() << "[scenario-builder] updater mode ON";

					logs.debug() << "[scenario-builder] writing data to a temporary file before structure changes";
					pTempFile << memory.cacheFolder()
						<< SEP << "antares-scenbld-save-" << memory.processID()
						<< '-' << (size_t)(this) << "-scenariobuilder.tmp";
					// Dump the memory
					study.scenarioRules->saveToINIFile(pTempFile);
					study.scenarioRules->clear();
				}
			}

			~ScenarioBuilderUpdater()
			{
				using namespace Yuni;

				if (not pTempFile.empty())
				{
					if (pStudy.scenarioRules)
					{
						logs.debug() << "[scenario-builder] reloading data from a temporary file";
						pStudy.scenarioRules->loadFromINIFile(pTempFile);

						logs.debug() << "[scenario-builder] updater mode OFF";
					}
					// Removing the temporary file
					IO::File::Delete(pTempFile);
				}
			}

		private:
			Data::Study& pStudy;
			Yuni::String pTempFile;

		}; // class ScenarioBuilderUpdater


	} // anonymous namespace




} // namespace Antares

#endif //  __ANTARES_LIBS_STUDY_SCENARIO_BUILDER_UPDATER_HXX__
