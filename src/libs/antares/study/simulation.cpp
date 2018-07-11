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
#include <yuni/io/file.h>
#include "study.h"
#include "../constants.h"
#include <stdio.h>
#include <string.h>
#include "simulation.h"
#include "../logs.h"


using namespace Yuni;


#define SEP IO::Separator


namespace Antares
{
namespace Data
{


	Simulation::Simulation(Study& study) :
		pStudy(study)
	{}


	bool Simulation::saveToFolder(const AnyString& folder) const
	{
		String b;
		b.reserve(folder.size() + 20);
		b = folder;

		// Ensure that the folder has been created
		if (!IO::Directory::Create(b))
		{
			logs.error() << "I/O: impossible to create the directory " << b;
			return false;
		}

		// Save the comments
		b = folder;
		b << SEP << "comments.txt";
		if (IO::File::SetContent(b, comments))
			return true;
		logs.error() << "I/O: impossible to write " << b;
		return false;
	}



	bool Simulation::loadFromFolder(const StudyLoadOptions& options)
	{
		if (!options.loadOnlyNeeded)
		{
			pStudy.buffer.clear() << pStudy.folderSettings << SEP << "comments.txt";
			if (IO::errNone != IO::File::LoadFromFile(comments, pStudy.buffer))
				logs.warning() << pStudy.buffer << ": Impossible to read the file";
		}
		return true;
	}


	Yuni::uint64 Simulation::memoryUsage() const
	{
		return name.capacity() + comments.capacity();
	}





} // namespace Data
} // namespace Antares

