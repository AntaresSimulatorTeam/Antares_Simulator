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

#ifndef __TESTS_ANTARES_LIBS_LOGS_FAKE_H__
#define __TESTS_ANTARES_LIBS_LOGS_FAKE_H__

#include <string>

#include <yuni/core/string/string.h>

namespace Antares
{
namespace UnitTests
{

class fakeLogger;

class Buffer
{
public:
    Buffer() = default;

    template<typename U>
    Buffer& operator<<(const U& u)
    {
        // Appending the piece of message to the buffer
        buffer_.append(u);
        return *this;
    }

    void clear()
    {
        buffer_.clear();
    }

    std::string content() const
    {
        return buffer_.to<std::string>();
    }

    bool contains(const std::string sub_string) const
    {
        return buffer_.contains(sub_string);
    }

    bool empty()
    {
        return buffer_.empty();
    }

private:
    Yuni::CString<1024> buffer_;
};

class fakeLogger
{
public:
    fakeLogger() = default;

    Buffer& error()
    {
        return error_buffer_;
    }

    Buffer& info()
    {
        return info_buffer_;
    }

    Buffer& debug()
    {
        return debug_buffer_;
    }

    Buffer& warning()
    {
        return warning_buffer_;
    }

private:
    Buffer error_buffer_;
    Buffer info_buffer_;
    Buffer debug_buffer_;
    Buffer warning_buffer_;
};

} // namespace UnitTests

extern UnitTests::fakeLogger logs;

} // namespace Antares

#endif // __TESTS_ANTARES_LIBS_LOGS_FAKE_H__
