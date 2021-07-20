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
#ifndef __ANTARES_WINDOW_RENEWABLE_PANEL_H__
#define __ANTARES_WINDOW_RENEWABLE_PANEL_H__

// #include "../../toolbox/wx-wrapper.h"
#include "../../toolbox/input/area.h"
#include "../../toolbox/components/notebook/notebook.h"
// #include <ui/common/component/panel.h>
// #include <antares/study/area/area.h>
// #include <antares/study/parts/thermal/cluster.h>

#include <antares/study/parts/renewable/cluster.h>

namespace Antares
{
namespace Window
{
namespace Renewable
{
class Panel : public Component::Panel, public Yuni::IEventObserver<Panel>
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
    // gp : to be adapted
    //! The page related to the cluster's timeseries
    Component::Notebook::Page* pageRenewableTimeSeries;
    //! The page related to the cluster's ts-generator data
    // Component::Notebook::Page* pageThermalPrepro;
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
    Yuni::uint64 pStudyRevisionIncrement;
    /*!
    ** \brief The splitter window
    **
    ** This component is used to separater the cluster list from the
    ** properties of the cluster currently selected.
    */
    wxSplitterWindow* pSplitter;

}; // class Panel

} // namespace Thermal
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOW_RENEWABLE_PANEL_H__
