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

#include <antares/logs/logs.h>
#include "recentfiles.h"
#include <wx/config.h>
#include <wx/dir.h>
#include <antares/study/study.h>

using namespace Yuni;

namespace Antares
{
namespace RecentFiles
{
using WxConfigPtr = SmartPtr<wxConfig, Policy::Ownership::ReferenceCounted>;

static bool TryToGetTheTitleFromStudyFolder(String& cstr,
                                            String& pathConvert,
                                            const wxString& path,
                                            wxString& title)
{
    wxStringToString(path, pathConvert);
    if (Data::Study::TitleFromStudyFolder(pathConvert, cstr, false))
    {
        if (!cstr.empty())
        {
            title = wxStringFromUTF8(cstr);
            return true;
        }
    }
    return false;
}

ListPtr Get()
{
    ListPtr result = new List();

    // Get the config file
    WxConfigPtr config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));

    long count;
    if (config->Read(wxT("/files/recents/count"), &count, 0))
    {
        wxString title;
        wxString path;
        wxString entry;

        // A temporary c-string used by the study routines
        String cstr;
        // Another temporary string, for converting path
        String pathConvert;

        for (long i = 0; i < count; ++i)
        {
            entry.Clear();
            entry << wxT("/files/recents/") << i << wxT("/");

            config->Read(entry + wxT("title"), &title, wxEmptyString);
            config->Read(entry + wxT("path"), &path, wxEmptyString);

            // Getting the title from the real folder
            // The operation may fail so 'title' will remain untouched
            if (!TryToGetTheTitleFromStudyFolder(cstr, pathConvert, path, title))
                continue;

            // Adding
            if (!path.IsEmpty() && wxT(".") != path && wxT("..") != path && wxDir::Exists(path))
            {
                for (uint c = 0; c != path.size(); ++c)
                {
                    if (System::windows)
                    {
                        if (path[c] == wxT('/'))
                            path[c] = wxT('\\');
                    }
                    else
                    {
                        if (path[c] == wxT('\\'))
                            path[c] = wxT('/');
                    }
                }
                result->push_back(std::pair<wxString, wxString>(path, title));
            }
        }
    }
    return result;
}

ListPtr AddAndGet(wxString path, const wxString& title)
{
    // Gettinh the list
    ListPtr lst = Get();

    if (path.Last() == '/' || path.Last() == '\\')
        path.RemoveLast();
    if (!path.IsEmpty())
    {
        // Check if the path does not already exist
        for (List::iterator i = lst->begin(); i != lst->end(); ++i)
        {
            if (i->first == path)
            {
                lst->erase(i);
                break;
            }
        }
        // Check for limits
        if (lst->size() == Max)
            lst->pop_back();

        // Insert the item at the begining
        lst->push_front(std::pair<wxString, wxString>(path, title));
        Write(lst);
    }
    return lst;
}

void Add(const wxString& path, const wxString& title)
{
    AddAndGet(path, title);
}

void Write(const ListPtr lst)
{
    // Get the config file
    WxConfigPtr config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));
    if (!(!config))
    {
        config->SetPath(wxT("/files"));
        config->DeleteGroup(wxT("recents"));

        if (lst && !lst->empty())
        {
            config->Write(wxT("recents/count"), (int)lst->size());

            int indx = 0;
            for (List::iterator i = lst->begin(); i != lst->end(); ++i)
            {
                wxString p;
                p << wxT("recents/") << indx << wxT("/");
                config->Write(p + wxT("path"), i->first);
                if (!i->second.IsEmpty())
                    config->Write(p + wxT("title"), i->second);
                ++indx;
            }
        }
        else
        {
            config->Write(wxT("recents/count"), 0);
        }
    }
}

void ShowPathInMenu(bool value)
{
    WxConfigPtr config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));
    if (!(!config))
    {
        config->SetPath(wxT("/files"));
        config->Write(wxT("showPathInRecentFilesMenu"), value);
    }
}

bool ShowPathInMenu()
{
    WxConfigPtr config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));
    if (!(!config))
    {
        config->SetPath(wxT("/files"));
        bool value = true;
        config->Read(wxT("showPathInRecentFilesMenu"), &value, true);
        return value;
    }
    return true;
}

} // namespace RecentFiles
} // namespace Antares
