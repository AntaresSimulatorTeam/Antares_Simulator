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

#include "job.h"
#include <antares/logs.h>
#include <antares/study.h>
#include <yuni/io/directory/info.h>

using namespace Yuni;

# define SEP Yuni::IO::Separator



namespace Antares
{
namespace Private
{
namespace OutputViewerData
{



	template<class MapT>
	static inline void Browse(const AnyString& path, MapT& content)
	{
		typedef IO::Directory::Info  DirInfo;

		DirInfo dirinfo(path);
		typename MapT::key_type name;

		auto end = dirinfo.folder_end();
		for (auto i = dirinfo.folder_begin(); i != end; ++i)
		{
			name = *i;
			name.toLower();
			content.insert(name);
		}
	}


	template<class MapT>
	static inline bool TryAreaListFileExtraction(const AnyString& path, MapT& content)
	{
		String filename;
		filename << path << SEP << "about-the-study" << SEP << "areas.txt";

		typename MapT::key_type name; // temporary name

		return (IO::File::ReadLineByLine(filename, [&] (const String& line)
		{
			if (line.first() != '@')
			{
				// This is an area
				name.clear();
				TransformNameIntoID(line, name);
				content.insert(name);
			}
			else
			{
				// This is a group
				name = line;
				name.trim();
				name.toLower();
				content.insert(name);
			}
		}));
	}






	Job::Job(Antares::Window::OutputViewer::Component& component, const AnyString& path) :
		Yuni::Job::IJob(),
		pComponent(component),
		pPath(path),
		pContent(nullptr)
	{}


	Job::~Job()
	{
		delete pContent;
	}


	void Job::onExecute()
	{
		// It is important to check from time to time if the job is still valid to stop
		// as soon as possible
		if (canceling())
			return;

		// Temporary variables
		logs.info() << "[output-viewer] running analyzis on " << pPath;
		pContent = new Content();

		typedef IO::Directory::Info  DirInfo;
		DirInfo  dirinfo(pPath);

		// Looking for economy / adequacy
		String pathTmp;

		auto end = dirinfo.folder_end();
		for (auto i = dirinfo.folder_begin(); i != end; ++i)
		{
			if ((*i).equalsInsensitive("economy"))
			{
				// Trying to use the file about-the-study/areas.txt first,
				// otherwise we will analyze the directory structure
				if (not TryAreaListFileExtraction(pPath, pContent->economy.areas))
				{
					pathTmp.clear() << i.filename() << SEP << "mc-all" << SEP << "areas";
					Browse(pathTmp, pContent->economy.areas);
				}

				if (canceling())
					return;

				pathTmp.clear() << i.filename() << SEP << "mc-all" << SEP << "links";
				Browse(pathTmp, pContent->economy.links);

				// Year-by-year
				gatherInfosAboutYearByYearData(i.filename());

				// Gather informations about the thermal clusters
				gatherInfosAboutThermalClusters(i.filename());
				break;
			}
			if ((*i).equalsInsensitive("adequacy"))
			{
				// Trying to use the file about-the-study/areas.txt first,
				// otherwise we will analyze the directory structure
				if (not TryAreaListFileExtraction(pPath, pContent->adequacy.areas))
				{
					pathTmp.clear() << i.filename() << SEP << "mc-all" << SEP << "areas";
					Browse(pathTmp, pContent->adequacy.areas);
				}

				if (canceling())
					return;

				pathTmp.clear() << i.filename() << SEP << "mc-all" << SEP << "links";
				Browse(pathTmp, pContent->adequacy.links);

				// Year-by-year
				gatherInfosAboutYearByYearData(i.filename());

				// Gather informations about the thermal clusters
				gatherInfosAboutThermalClusters(i.filename());
				break;
			}
			if (canceling())
				return;
		}

		if (canceling() or shouldAbort())
			return;

		// Write the results into the cache
		pComponent.pMutex.lock();
		if (not canceling())
		{
			Content* c = pContent;
			pContent = nullptr;
			pComponent.pAlreadyPreparedContents[pPath] = c;

			// Cool we can perform the full aggregation !
			typedef Antares::Window::OutputViewer::Component Component;
			if (0 == --pComponent.pJobsRemaining)
			{
				if (not canceling())
					Antares::Dispatcher::GUI::Post(&pComponent, &Component::treeDataUpdate);
			}
		}
		pComponent.pMutex.unlock();
	}


	void Job::gatherInfosAboutThermalClusters(const AnyString& path)
	{
		String filename;
		filename << path << SEP << "mc-all" << SEP << "grid" << SEP << "thermal.txt";

		enum
		{
			options = Matrix<>::optImmediate | Matrix<>::optNoWarnIfEmpty
				| Matrix<>::optQuiet | Matrix<>::optNeverFails
		};

		typedef Antares::Matrix<Yuni::CString<164,false> >  MatrixType;
		MatrixType matrix;
		if (matrix.loadFromCSVFile(filename, 1, 0, options) and matrix.width > 2)
		{
			for (uint y = 1; y < matrix.height; ++y)
				pContent->clusters[matrix[0][y]].insert(matrix[1][y]);
		}
	}


	void Job::gatherInfosAboutYearByYearData(const AnyString& path)
	{
		assert(!(!pContent) and "invalid content");

		pContent->hasYearByYear = false;
		// Making sure that the data are initialized
		pContent->ybyInterval[0] = (uint) -1;
		pContent->ybyInterval[1] = 0;

		if (canceling() or pContent->empty() or path.empty())
			return;

		// Since v3.7, the individual years can be found in the folder `mc-ind/<number>`.
		// Before 3.7, it was in the folder `Economy`, at the same level than `mc-all`.

		typedef IO::Directory::Info  DirInfo;

		// >= 3.7
		{
			String mcindpath;
			mcindpath << path << SEP << "mc-ind";
			DirInfo  dirinfo(mcindpath);
			auto end = dirinfo.folder_end();
			for (auto i = dirinfo.folder_begin(); i != end; ++i)
			{
				const String& name = *i;
				uint year;
				if (name.to(year) and year and year < 500000)
				{
					pContent->hasYearByYear = true;
					if (year < pContent->ybyInterval[0])
						pContent->ybyInterval[0] = year;
					if (year > pContent->ybyInterval[1])
						pContent->ybyInterval[1] = year;
				}
			}
		}

		// <= 3.6 compatibility, only if individual years have not been found
		if (not pContent->hasYearByYear)
		{
			DirInfo  dirinfo(path);
			auto end = dirinfo.folder_end();
			for (auto i = dirinfo.folder_begin(); i != end; ++i)
			{
				const String& name = *i;
				if (name.size() > 8 and name[2] == '-' and name.icontains("mc-i"))
				{
					AnyString stryear(name.c_str() + 4, name.size() - 4);
					uint year;
					if (stryear.to(year) and year and year < 500000)
					{
						pContent->hasYearByYear = true;
						if (year < pContent->ybyInterval[0])
							pContent->ybyInterval[0] = year;
						if (year > pContent->ybyInterval[1])
							pContent->ybyInterval[1] = year;
					}
				}
			}
		}

		// Checking for individual years concatened
		if (pContent->hasYearByYear)
		{
			pContent->hasConcatenedYbY = true;
		}
		else
		{
			String mcindpath;
			mcindpath << path << SEP << "mc-var";

			DirInfo  dirinfo(mcindpath);
			auto end = dirinfo.folder_end();
			for (auto i = dirinfo.folder_begin(); i != end; ++i)
			{
				// We have at least one folder
				pContent->hasConcatenedYbY = true;
				break;
			}
		}

		# ifndef NDEBUG
		if (pContent->hasYearByYear)
		{
			// We have to ensure that provided values are correct
			assert(pContent->ybyInterval[0] > 0 and pContent->ybyInterval[0] < 500000);
			assert(pContent->ybyInterval[1] > 0 and pContent->ybyInterval[1] < 500000);
		}
		# endif
	}






} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

