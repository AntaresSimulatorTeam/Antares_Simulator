/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/

#include "main.h"
#include "../../windows/cleaner.h"
#include "../study.h"

namespace Antares
{
namespace Forms
{
void ApplWnd::evtOnCleanCurrentStudyFolder(wxCommandEvent&)
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        if (study->parameters.readonly)
        {
            logs.error() << "Impossible to perform a cleanup. The study is readonly.";
            return;
        }
        if ((int)study->header.version != (int)Data::versionLatest)
        {
            // Logs
            logs.error() << "Impossible to perform a cleanup. You must save (as) the study before, "
                            "in order to upgrade the structure of folder";
            return;
        }
        if (StudyHasBeenModified())
        {
            // Logs
            logs.error() << "You must save the changes first before cleaning the study folder.";
            return;
        }

        Forms::Disabler<ApplWnd> disabler(*this);
        auto* dialog = new Window::StudyCleaner(this);
        dialog->studyFolder(wxStringFromUTF8(study->folder));
        dialog->ShowModal();
        dialog->Destroy();
    }
}

void ApplWnd::evtOnCleanStudyFolder(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    auto* dialog = new Window::StudyCleaner(this);
    dialog->ShowModal();
    dialog->Destroy();
}

} // namespace Forms
} // namespace Antares
