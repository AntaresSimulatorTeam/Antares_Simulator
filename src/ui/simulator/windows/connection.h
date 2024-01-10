/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
*/
#ifndef __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
#define __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__

#include "../toolbox/components/datagrid/renderer/connection.h"
#include "link-property-buttons.h"

namespace Antares
{
namespace Window
{
/*!
**
*/

// Forward declaration
class Interconnection;

class linkGrid
{
public:
    linkGrid() = default;
    virtual ~linkGrid() = default;
    virtual void add(wxBoxSizer* sizer,
                     wxWindow* parent,
                     Interconnection* intercoWindow,
                     Toolbox::InputSelector::Connections* notifier)
      = 0;
};

class linkParametersGrid : public linkGrid
{
public:
    linkParametersGrid() = default;
    ~linkParametersGrid() override = default;
    void add(wxBoxSizer* sizer,
             wxWindow* parent,
             Interconnection* intercoWindow,
             Toolbox::InputSelector::Connections* notifier) override;
};

class linkNTCgrid : public linkGrid
{
public:
    linkNTCgrid() = default;
    ~linkNTCgrid() override = default;
    void add(wxBoxSizer* sizer,
             wxWindow* parent,
             Interconnection* intercoWindow,
             Toolbox::InputSelector::Connections* notifier) override;
};

class Interconnection : public wxScrolledWindow, public Yuni::IEventObserver<Interconnection>
{
public:
    //! \name Constructor & Destructor
    //@{
    Interconnection(wxWindow* parent,
                    Toolbox::InputSelector::Connections* notifier,
                    linkGrid* link_grid);
    //! Destructor
    virtual ~Interconnection();
    //@}

private:
    /*!
    ** \brief Event: The selection of the connection has changed
    */
    void onConnectionChanged(Data::AreaLink* link);

    void onStudyLinkChanged(Data::AreaLink* link);

    bool checkLinkView(Data::AreaLink* link);
    void updateLinkView(Data::AreaLink* link);
    void finalizeView();

private:
    //! Pointer to the current link
    Data::AreaLink* pLink = nullptr;

    ntcUsageButton* ntcUsageButton_ = nullptr;
    captionButton* captionButton_ = nullptr;
    hurdleCostsUsageButton* hurdleCostsUsageButton_ = nullptr;
    assetTypeButton* assetTypeButton_ = nullptr;
    loopFlowUsageButton* loopFlowUsageButton_ = nullptr;
    phaseShifterUsageButton* phaseShifterUsageButton_ = nullptr;

    //! No Link
    wxWindow* pNoLink;
    //! Link data
    wxWindow* pLinkData;

}; // class Interconnection

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
