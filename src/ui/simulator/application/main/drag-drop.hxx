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

#include "main.h"
#include <wx/dnd.h>
#include "../study.h"
#include "../../toolbox/dispatcher/study.h"
#include <ui/common/lock.h>

using namespace Yuni;

namespace Antares
{
namespace Forms
{
class StudyDrop final : public wxFileDropTarget
{
public:
    StudyDrop() : wxFileDropTarget()
    {
    }

    virtual ~StudyDrop()
    {
    }

    virtual bool OnDropFiles(wxCoord /*x*/, wxCoord /*y*/, const wxArrayString& filenames) override
    {
        if (IsGUIAboutToQuit() or GUIIsLock())
            return false;

        String filename;
        String folder;
        String title;

        for (uint i = 0; i != (uint)filenames.size(); ++i)
        {
            wxStringToString(filenames[i], filename);
            if (not Data::Study::IsInsideStudyFolder(filename, folder, title))
                folder.clear();
        }

        if (not folder.empty())
        {
            // opening the study
            Dispatcher::StudyOpen(folder);
        }
        else
            logs.warning() << "The folder provided by drag & drop is not a valid study: "
                           << filename;

        return true;
    }

}; // class StudyDrop

} // namespace Forms
} // namespace Antares
