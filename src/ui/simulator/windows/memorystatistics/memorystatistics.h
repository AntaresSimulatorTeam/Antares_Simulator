/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_APPLICATION_MEMORYSTATISTICS_MEMORYSTATISTICS_H__
#define __ANTARES_APPLICATION_MEMORYSTATISTICS_MEMORYSTATISTICS_H__

#include <wx/dialog.h>
#include "private.h"

namespace Antares
{
namespace Window
{
class MemoryStatistics final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    MemoryStatistics(wxWindow* parent);

    //! Destructor
    virtual ~MemoryStatistics();
    //@}

    /*!
    ** \brief Refresh information
    */
    void refreshInformation();

private:
    //! User: close the window (via button "close")
    void onClose(void*);

private:
    //! Internal data
    Antares::Private::Window::MemoryStatisticsData* pData;
    bool pDisplayLogsOnce;

}; // class MemoryStatistics

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_MEMORYSTATISTICS_MEMORYSTATISTICS_H__
