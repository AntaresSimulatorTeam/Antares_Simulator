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
#ifndef __ANTARES_LIBS_TIME_ELAPSED__TIME_ELAPSED_H__
#define __ANTARES_LIBS_TIME_ELAPSED__TIME_ELAPSED_H__

#include <yuni/yuni.h>
#include <yuni/string.h>

namespace TimeElapsed
{
class ContentHandler;

class Timer final
{
public:
    //! Default constructor
    Timer() = delete;

    //! Copy constructor
    Timer(const Timer& rhs) = delete;
    Timer& operator=(const Timer& rhs) = delete;

    //! Constructor with a default text
    explicit Timer(const AnyString& userText,
                   const AnyString& logText = "",
                   bool verbose = true,
                   ContentHandler* aggregator = nullptr);

    void stop();

private:
    //! The text to display at the end
    Yuni::ShortString256 fileText;
    // Aggregation
    Yuni::ShortString256 logText;
    //! timestamp when the timer started
    yint64 pStartTime;
    //! Display text on destruction
    bool verbose;
    //! Aggregation into a single file for automation
    ContentHandler* pContentHandler;
}; // class Timer
} // namespace TimeElapsed

#endif // __ANTARES_LIBS_TIME_ELAPSED__TIME_ELAPSED_H__
