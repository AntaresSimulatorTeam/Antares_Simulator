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
#include "../cleaner.h"
#include "../../sys/mem-wrapper.h"
#include "versions.h"
#include "../../logs.h"


using namespace Yuni;


# define STUDY_CLEANER_LOG "[study cleaner] "



namespace Antares
{
namespace Data
{


	StudyCleaningInfos::StudyCleaningInfos()
	{
		version = versionUnknown;
	}


	StudyCleaningInfos::StudyCleaningInfos(const AnyString& path) :
		folder(path)
	{
		version = versionUnknown;
	}


	StudyCleaningInfos::~StudyCleaningInfos()
	{
	}



	bool StudyCleaningInfos::analyze()
	{
		logs.info() << "cleaning study: " << folder;

		// clear
		exclude.clear();
		intruders.clear();
		postExclude.clear();
		// Getting the version
		version = StudyTryToFindTheVersion(folder);

		switch (version)
		{
			case version1xx:
				{
					logs.error() << "Study version: 1.x. Too old version. Nothing will be done: " << folder;
					break;
				}
			case versionFutur:
				{
					logs.error() << "A more recent version of Antares is required for " << folder;
					break;
				}
			case versionUnknown:
				{
					logs.error() << "Unknown study version: " << folder;
					break;
				}
			default:
				{
					if ((int)version >= (int)version200 && (int)version <= (int)versionLatest)
					{
						if (not PreflightVersion20(this))
						{
							logs.error() << "Aborting: an error has been encountered: " << folder;
							return false;
						}
					}
					else
						logs.error() << "Invalid study version: " << folder;
					break;
				}
		}

		// Grab all intruders at once
		intruders.onProgress = onProgress;
		intruders.addFromFolder(folder, exclude);
		// Post cleanup in the list
		intruders.remove(postExclude);

		// Result
		if (not intruders.empty())
		{
			if (intruders.size() == 1)
				logs.info() << "  :: 1 file/folder will be removed";
			else
				logs.info() << "  :: " << intruders.size() << " files/folders will be removed";
			return true;
		}
		else
		{
			logs.info() << "  :: no file will be removed";
			return false;
		}
		return (intruders.size()) ? true : false;
	}


	void StudyCleaningInfos::performCleanup()
	{
		if (version != versionUnknown && not intruders.empty())
		{
			// Remove all files first
			intruders.deleteAllFiles(folder);
			// Remove all folders next
			intruders.deleteAllEmptyFolders(folder);
		}
	}


	Yuni::uint64 StudyCleaningInfos::totalSize() const
	{
		return intruders.totalSizeInBytes();
	}




} // namespace Data
} // namespace Antares

