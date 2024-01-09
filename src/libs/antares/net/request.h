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
#ifndef __ANTARES_LIBS_NET_REQUEST_H__
#define __ANTARES_LIBS_NET_REQUEST_H__

namespace Antares
{
namespace Net
{
enum Method
{
    rmGET,
    rmPOST
};

//! Event
using EventOnSuccess = Yuni::Bind<void()>;

void Request(Method method,
             const AnyString& url,
             const EventOnSuccess& onSuccess,
             const EventOnError& onError);

} // namespace Net
} // namespace Antares

#endif // __ANTARES_LIBS_NET_REQUEST_H__
