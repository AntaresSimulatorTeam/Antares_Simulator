// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_MEMORYSTATISTICS_MEMORYSTATISTICS_H__
#define __ANTARES_APPLICATION_MEMORYSTATISTICS_MEMORYSTATISTICS_H__

#include <wx/dialog.h>
#include "private.h"

namespace Antares::Window
{
class MemoryStatistics final: public wxDialog
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

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_MEMORYSTATISTICS_MEMORYSTATISTICS_H__
