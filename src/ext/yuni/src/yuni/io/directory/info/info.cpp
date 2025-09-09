/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include "info.h"
#include "../../file.h"

namespace Yuni::IO::Directory
{
static void NormalizeTinyDirectoryPath(String& path)
{
    ShortString512 tmp(path);
    IO::Normalize(path, tmp);
}

void Info::normalize()
{
    // We will use a tiny optimization here
    // When possible, we will use a static buffer to avoid as much as possible
    // malloc and free.
    if (pDirectory.size() < 512)
    {
        NormalizeTinyDirectoryPath(pDirectory);
    }
    else
    {
        String tmp = pDirectory;
        IO::Normalize(pDirectory, tmp);
    }
}

bool Info::clean() const
{
    bool result = true;
    iterator i(pDirectory);
    for (; i.valid(); ++i)
    {
        if (i.isFile())
        {
            // removing the file
            result = (IO::errNone == IO::File::Delete(i.filename())) and result;
        }
        else
        {
            if (i.isFolder())
            {
                // Removing the folder
                result = IO::Directory::Remove(i.filename()) and result;
            }
        }
    }
    return result;
}

} // namespace Yuni::IO::Directory
