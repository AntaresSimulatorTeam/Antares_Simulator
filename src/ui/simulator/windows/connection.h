// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
#define __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__

#include "../toolbox/components/datagrid/renderer/connection.h"
#include "link-property-buttons.h"

namespace Antares::Window
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

class linkParametersGrid: public linkGrid
{
public:
    linkParametersGrid() = default;
    ~linkParametersGrid() override = default;
    void add(wxBoxSizer* sizer,
             wxWindow* parent,
             Interconnection* intercoWindow,
             Toolbox::InputSelector::Connections* notifier) override;
};

class linkNTCgrid: public linkGrid
{
public:
    linkNTCgrid() = default;
    ~linkNTCgrid() override = default;
    void add(wxBoxSizer* sizer,
             wxWindow* parent,
             Interconnection* intercoWindow,
             Toolbox::InputSelector::Connections* notifier) override;
};

class Interconnection: public wxScrolledWindow, public Yuni::IEventObserver<Interconnection>
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

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
