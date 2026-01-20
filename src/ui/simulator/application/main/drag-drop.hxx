// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "main.h"
#include <wx/dnd.h>
#include "antares/study/study.h"
#include "../../toolbox/dispatcher/study.h"
#include <ui/common/lock.h>
#include "antares/utils/utils.h"

using namespace Yuni;

namespace Antares::Forms
{

class StudyDrop final: public wxFileDropTarget
{
public:
    StudyDrop():
        wxFileDropTarget()
    {
    }

    virtual ~StudyDrop()
    {
    }

    virtual bool OnDropFiles(wxCoord /*x*/, wxCoord /*y*/, const wxArrayString& filenames) override
    {
        if (IsGUIAboutToQuit() or GUIIsLock())
        {
            return false;
        }

        String filename;
        String folder;
        String title;

        for (uint i = 0; i != (uint)filenames.size(); ++i)
        {
            wxStringToString(filenames[i], filename);

            if (!Utils::isPathValid(filename.to<std::string>()))
            {
                logs.error() << "Drag & drop : study path contains a non ASCII char";
                return false;
            }

            if (not Data::Study::IsInsideStudyFolder(filename, folder, title))
            {
                folder.clear();
            }
        }

        if (not folder.empty())
        {
            // opening the study
            Dispatcher::StudyOpen(folder);
        }
        else
        {
            logs.warning() << "The folder provided by drag & drop is not a valid study: "
                           << filename;
        }

        return true;
    }

}; // class StudyDrop

} // namespace Antares::Forms
