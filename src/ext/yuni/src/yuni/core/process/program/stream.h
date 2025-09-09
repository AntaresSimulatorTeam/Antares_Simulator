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
#pragma once
#include "../../../yuni.h"
#include "../../smartptr.h"

namespace Yuni::Process
{
/*!
** \brief Process Stream
**
** The lifetime of a process stream is guarantee to be at least
** the whole execution of the underlying process
*/
class Stream
{
public:
    typedef SmartPtr<Stream> Ptr;

public:
    Stream()
    {
    }

    virtual ~Stream()
    {
    }

    //! Some data from the standard output are ready
    virtual void onRead(const AnyString& /*buffer*/)
    {
    }

    //! Some data from the error output are ready
    virtual void onErrorRead(const AnyString& /*buffer*/)
    {
    }

    //! The execution has finished
    virtual void onStop(bool /*killed*/, int /*exitstatus*/, int64_t /*duration*/)
    {
    }
};

class CaptureOutput: public Process::Stream
{
public:
    typedef SmartPtr<CaptureOutput> Ptr;

public:
    CaptureOutput()
    {
    }

    virtual ~CaptureOutput()
    {
    }

    //! Some data from the standard output are ready
    virtual void onRead(const AnyString& buffer) override
    {
        cout += buffer;
    }

    //! Some data from the error output are ready
    virtual void onErrorRead(const AnyString& buffer) override
    {
        cerr += buffer;
    }

public:
    Clob cout;
    Clob cerr;

}; // class CaptureOutput

} // namespace Yuni::Process
