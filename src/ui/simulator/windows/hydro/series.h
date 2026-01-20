// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__
#define __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__

#include "../../toolbox/input/area.h"
#include "../../toolbox/components/notebook/notebook.h"
#include <ui/common/component/panel.h>

namespace Antares::Window::Hydro
{
class Series: public Component::Panel, public Yuni::IEventObserver<Series>
{
public:
    //! name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Series(wxWindow* parent, Toolbox::InputSelector::Area* notifier = NULL);
    //! Destructor
    virtual ~Series();
    //@}

private:
    void onStudyLoaded();
    void onStudyClosed();

private:
    //! Event: An area has been selected
    void onAreaChanged(Data::Area* area);
    //! Area notifier
    Toolbox::InputSelector::Area* pNotifier;
    //! The last selected area
    Data::Area* pLastArea;
    //! Page
    Component::Notebook::Page* pPageFatal;

}; // class Series

} // namespace Antares::Window::Hydro

#endif // __ANTARES_APPLICATION_WINDOWS_HYDRO_PREPRO_H__
