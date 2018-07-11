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

#include "output.h"
#include <yuni/io/directory/iterator.h>

using namespace Yuni;

#define SEP IO::Separator




namespace Antares
{
namespace Data
{


	namespace // anonymous
	{

		class OutputFolderIterator : public IO::Directory::IIterator<true>
		{
		public:
			typedef IO::Directory::IIterator<true> IteratorType;
			typedef IO::Flow Flow;

		public:
			OutputFolderIterator(Data::Output::List& list) :
				pList(list)
			{}
			virtual ~OutputFolderIterator()
			{
				// For code robustness and to avoid corrupt vtable
				stop();
			}

		protected:
			virtual bool onStart(const String&)
			{
				pList.clear();
				return true;
			}

			virtual Flow onFile(const String& /*filename*/, const String& parent, const String& name, uint64)
			{
				pExtension.clear();
				IO::ExtractExtension(pExtension, name);
				pExtension.toLower();

				if (pExtension == ".antares-output")
				{
					auto* info = new Data::Output(parent);
					if (info->valid())
						pList.push_back(info);
					return IO::flowSkip;
				}
				return IO::flowContinue;
			}

		private:
			CString<20,false> pExtension;
			Output::List& pList;

		}; // class OutputFolderIterator


	} // anonymous namespace








	Output::Output(const AnyString& folder) :
		version(0),
		timestamp(0),
		mode(Data::stdmEconomy),
		menuID(-1),
		viewMenuID(-1),
		outputViewerID(-1)
	{
		loadFromFolder(folder);
	}


	bool Output::valid() const
	{
		// The outputs as we know them was first introduced in Antares 3.0
		return ((uint) version >= 300)
			&& ((uint) version <= (uint) Data::versionLatest);
	}


	bool Output::loadFromFolder(const AnyString& folder)
	{
		// reset
		menuID = -1;
		viewMenuID = -1;
		outputViewerID = -1;
		title.clear();
		name.clear();
		path.clear();
		version = 0;
		mode = Data::stdmUnknown;

		// Load the INI file in memory
		IniFile ini;
		// The internal variable path will be use for temporary operations
		path.reserve(folder.size() + 32);
		path << folder << SEP << "info.antares-output";

		if (not ini.open(path))
		{
			// Restoring the good value for the variable `path`
			path = folder;
			return false;
		}

		// Restoring the good value for the variable `path`
		path = folder;

		// The section
		auto* section = ini.find("general");
		const IniFile::Property* p = section->firstProperty;
		while (p)
		{
			if (p->key == "version")
			{
				version = p->value.to<uint>();

				// Early checks about the version
				if (version < 300 || version > (uint)Data::versionLatest)
				{
					version = 0;
					return false;
				}
			}
			else
			{
				if (p->key == "title")
					title = p->value;
				else
				{
					if (p->key == "mode")
					{
						StringToStudyMode(mode, p->value);
					}
					else
					{
						if (p->key == "timestamp")
							timestamp = p->value.to<uint>();
						else
						{
							if (p->key == "name")
								name = p->value;
						}
					}
				}
			}
			// Next
			p = p->next;
		}

		// Post-processing about the title
		if (not name.empty())
			title << " - " << name;

		return true;
	}



	void Output::RetrieveListFromStudy(List& out, const Study& study)
	{
		out.clear();

		if (not study.folder.empty())
		{
			String folder;
			folder << study.folder << SEP << "output";

			if (IO::Directory::Exists(folder))
			{
				OutputFolderIterator iterator(out);
				iterator.add(folder);
				iterator.start();
				iterator.wait(15000); // 15s - arbitrary
			}
			else
			{
				// No output at all, it is quite useless to start a new thread
				// for iterating into a non-existing folder
			}
		}
		else
		{
			// The variable folder is empty, meaning that the study is still
			// in memory and does not come from the disk
		}
	}





} // namespace Data
} // namespace Antares

