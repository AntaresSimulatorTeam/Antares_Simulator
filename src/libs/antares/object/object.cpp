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

#include "object.h"

using namespace Yuni;

namespace Antares
{
IObject::IObject() : pOID(Ref::fGenerate) // will generate a new uuid
{
    assert(!pOID.null()
           and "An object ID must not be NULL ({000000000-0000-0000-0000-0000000000000})");
}

IObject::IObject(const Ref& uid) : pOID(uid)
{
    assert(!pOID.null()
           and "An object ID must not be NULL ({000000000-0000-0000-0000-0000000000000})");
}

IObject::~IObject()
{
    // It would be safer to lock/unlock the inner mutex to prevent
    // some rare (and nearly impossible) case where the object is destroyed
    // whereas another thread is calling one of our methods.
    // The smart pointer should get rid of this issue.
    // And anyway, it is pointless here the vtable has been already wiped out
    //
    // ThreadingPolicy::MutexLocker locker(*this);
}

void IObject::caption(const AnyString& text)
{
    ThreadingPolicy::MutexLocker locker(*this);
    pCaption = text;
}

} // namespace Antares
