/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "main.h"
#include <wx/dnd.h>
#include "antares/study/study.h"
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
