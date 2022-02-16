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

// Map
#include "../../toolbox/clipboard/clipboard.h"
#include "../study.h"
#include "../../toolbox/ext-source/handler.h"

using namespace Yuni;

#define ANTARES_MAGIC_CLIPBOARD "antares.study.clipboard;"

namespace Antares
{
namespace Forms
{
namespace
{
/*!
** \brief Job for performing the paste operation
*/
class JobPasteFromClipboard final : public Yuni::Job::IJob
{
public:
    JobPasteFromClipboard(String::Ptr text, bool forceDialog = false) :
     pText(text), pForceDialog(forceDialog)
    {
    }
    virtual ~JobPasteFromClipboard()
    {
    }

protected:
    virtual void onExecute() override
    {
        ApplWnd::Instance()->pasteFromClipboard(*pText, pForceDialog);
    }

private:
    const String::Ptr pText;
    bool pForceDialog;

}; // class JobPasteFromClipboard

void PreparePasteFromClipboard(const String& text, bool forceDialog)
{
    // The job of this routine is to extract informations about the
    // handler to use for performing the paste.
    // Each handler is designed for a single data source.
    //
    // The name of the handler is obvously required, such as "com.rte-france.antares.study"
    // (used for Antares Study).
    // Some additional parameters may be provided
    //
    // All those data are located on the first line, separated by semicolons.
    // Here is an example :
    // antares.study.clipboard;handler=com.rte-france.antares.study;path=/local/partage/private/CS_2008_v3.5
    // (The first value is a magic dust)

    // Map of properties
    ExtSource::Handler::PropertyMap map;
    // The offset the first end-of-line
    String::Size line = text.find('\n');

    // Extracting all parameters
    {
        // skipping the first semicolon, because the first value is only some magic dust
        auto begin = text.find(';');
        if (begin == String::npos or line == String::npos or line < begin)
            return;
        ++begin;

        // iterating through all parameters
        bool stop = false;
        Antares::ExtSource::Handler::Key key;
        Antares::ExtSource::Handler::Value value;

        do
        {
            String::Size end = text.find(';', begin);
            if (end == String::npos)
            {
                end = line;
                stop = true;
            }
            if (end < begin)
                break;
            const String::Size equal = text.find('=', begin);
            // note: end will always be > 0 here
            if (equal < end - 1 && equal > begin)
            {
                key.assign(text, equal - begin, begin);
                key.trim(" \r\n");
                value.assign(text, end - equal - 1, equal + 1);
                value.trim(" \r\n");
                if (not key.empty())
                    map[key] = value;
            }
            begin = end + 1;
        } while (!stop);

        if (map.empty())
            return;
    }

    // Retrieving the handler
    const Antares::ExtSource::Handler::Value& handler = map["handler"];
    if (handler.empty())
        return;

    // Performing the paste operation according to the handler name
    // The following routines are located in 'toolbox/ext-source/handler'
    //
    // Note: Those routines will only prepare the paste in a first time
    //   in order to check conflicts.
    if (handler == "com.rte-france.antares.study")
    {
        auto study = Data::Study::Current::Get();
        if (!(!study))
            Antares::ExtSource::Handler::AntaresStudy(study, text, line + 1, map, forceDialog);
        return;
    }
}

} // anonymous namespace

void ApplWnd::pasteFromClipboard(bool showDialog)
{
    // Reset the status bar, since the next operation may take some
    // time... (if the dialog is invoked for example)
    resetDefaultStatusBarText();

    // Extracting the text from clipboard
    String::Ptr s = new String();
    Toolbox::Clipboard::GetFromClipboard(*s);

    // Performing the paste (delayed operation)
    if (!s->empty())
        Antares::Dispatcher::GUI::Post(
          (const Yuni::Job::IJob::Ptr&)new JobPasteFromClipboard(s, showDialog), 50 /*ms*/);
}

void ApplWnd::evtOnEditPaste(wxCommandEvent&)
{
    pasteFromClipboard(false);
}

void ApplWnd::evtOnEditPasteSpecial(wxCommandEvent&)
{
    pasteFromClipboard(true);
}

void ApplWnd::pasteFromClipboard(const String& text, bool showDialog)
{
    // The text is valid for Antares paste operations only if its
    // starts with a magic value
    if (text.startsWith(ANTARES_MAGIC_CLIPBOARD))
    {
        // The paste may have to be performed from various handler,
        // because the source may not be an Antares study
        PreparePasteFromClipboard(text, showDialog);
    }
}

} // namespace Forms
} // namespace Antares
