// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOW_RENEWABLE_PANEL_H__
#define __ANTARES_WINDOW_RENEWABLE_PANEL_H__

// #include "../../toolbox/wx-wrapper.h"
#include "../../toolbox/input/area.h"
#include "../../toolbox/components/notebook/notebook.h"
// #include <ui/common/component/panel.h>
// #include <antares/study/area/area.h>
// #include <antares/study/parts/thermal/cluster.h>

#include <antares/study/parts/renewable/cluster.h>

namespace Antares::Window::Renewable
{
class Panel: public Component::Panel, public Yuni::IEventObserver<Panel>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param parent The parent window
    */
    Panel(Component::Notebook* parent);
    //! Destructor
    virtual ~Panel();
    //@}

public:
    //! The page related to the cluster's timeseries
    Component::Notebook::Page* pageRenewableTimeSeries;
    //! The page related to the cluster's properties
    Component::Notebook::Page* pageRenewableCommon;
    //! The page related to the cluster list view
    Component::Notebook::Page* pageRenewableClusterList;

private:
    //! A thermal cluster has just been selected, we have to update the GUI accordinly
    void onClusterChanged(Data::RenewableCluster* cluster);
    //! Event: The selected area has been changed
    void onAreaChangedForData(Data::Area* area);
    //! Event: The page has been changed
    void onPageChanged(Component::Notebook::Page&);
    //! Event: a study has just been loaded or a new study has just been created
    void onStudyLoaded();
    //! Delayed Event triggered when a new study has been loaded, executed by onStudyLoaded()
    void internalOnStudyLoaded();
    //! Delayed event to resize the splitter
    void delayedResizeSplitter();

private:
    Component::Notebook* pNotebookCluster;
    Data::Area* pAreaForCommonData;
    Toolbox::InputSelector::Area* pAreaSelector;
    //! An internal increment, an ugly hack to avoid unnecessary refreshes
    uint64_t pStudyRevisionIncrement;
    /*!
    ** \brief The splitter window
    **
    ** This component is used to separater the cluster list from the
    ** properties of the cluster currently selected.
    */
    wxSplitterWindow* pSplitter;

}; // class Panel

} // namespace Antares::Window::Renewable

#endif // __ANTARES_WINDOW_RENEWABLE_PANEL_H__
