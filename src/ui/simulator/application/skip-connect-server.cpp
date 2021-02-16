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

#include <wx/config.h>
#include "../toolbox/wx-wrapper.h"
#include <yuni/core/smartptr/smartptr.h>
#include "skip-connect-server.h"

using namespace Yuni;

namespace Antares
{
namespace SkipConnectionToServer
{
typedef SmartPtr<wxConfig, Policy::Ownership::ReferenceCounted> WxConfigPtr;

bool Get()
{
    bool result = false;

    // Get the config file
    WxConfigPtr config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));

    result = config->ReadBool("/skipConnectionToServer", false);

    return result;
}

void Do()
{
    // Get the config file
    WxConfigPtr config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));

    config->Write("/skipConnectionToServer", true);
}

} // namespace SkipConnectionToServer
} // namespace Antares
