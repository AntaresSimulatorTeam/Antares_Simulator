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
#ifndef __ANTARES_APPLICATION_WINDOW_CONSTRAINTSBUILDER_H__
#define __ANTARES_APPLICATION_WINDOW_CONSTRAINTSBUILDER_H__

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/timer.h>
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/constraintsbuilder/links.h"
#include "../../../../solver/constraints-builder/cbuilder.h"
#include <ui/common/component/spotlight.h>
#include <yuni/thread/thread.h>
#include <yuni/core/event.h>

#include <memory>

namespace Antares
{
namespace Window
{
class ConstraintsBuilderWizard final : public wxDialog
{
public:
    //! File mapping
    using FileMapping = Component::Datagrid::Renderer::ConstraintsBuilder::Links::Map;
    //!
    using RecordVector = Component::Datagrid::Renderer::ConstraintsBuilder::Links::Record::Vector;
    //! Smartptr for file mapping
    using FileMappingPtr = std::shared_ptr<FileMapping>;

    enum IDs
    {
        mnIDCancel = wxID_HIGHEST + 1,
        mnIDPrefixMesh,
        mnIDPrefixGrid,
        mnIDPrefixNetwork,
        mnIDAutoClean,
        mnIDUpdateInfo,
        mnIDUpdateFileMapping,
        mnIDGridUpdate,
        mnIDCanRunAnalyzer,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent frame
    */
    ConstraintsBuilderWizard(wxFrame* parent);
    //! Destructor
    virtual ~ConstraintsBuilderWizard();
    void onIncludeLoopFlow(wxCommandEvent& evt);
    void onUsePhaseShift(wxCommandEvent& evt);
    void onUpdateLinesStatus(void*);
    void onCheckNodalLoopFlow(wxCommandEvent& evt);
    void onUpdateInfiniteValue(wxCommandEvent& evt);
    //@}

    void fileMapping(FileMappingPtr m);

    /*!
    ** \brief Force the refresh of the grid
    */
    void refreshGrid();

    /*!
    ** \brief Export all infos into an INI file
    */
    bool saveToFile(const Yuni::String& filename) const;

    const Yuni::String& cBuilderInfoFile() const
    {
        return pInfoFilename;
    }

    void cBuilderInfoFile(const Yuni::String& f)
    {
        pInfoFilename = f;
    }

    void beforeUpdate(int, int);
    void afterUpdate(int, int);

private:
    void onCancel(void*);
    void onBuild(void*);
    void onDelete(void*);

    void onPrefix(Component::Button&, wxMenu& menu, void*);
    void evtPrefixChanged(wxCommandEvent& evt);
    void enableAll(bool v);

    void gridAppend(wxFlexGridSizer& sizer,
                    const wxString& title,
                    const wxString& key,
                    wxWindow* value,
                    bool bold = false);

    void updateBeginningHourLabel(wxEvent& evt);

    void updateEndHourLabel(wxEvent& evt);

private:
    wxStaticText* mapName;
    wxStaticText* startingHourLabel;
    wxStaticText* endHourLabel;
    wxTextCtrl* startingHourCtrl;
    wxTextCtrl* endHourCtrl;
    wxSizer* startingHourSizer;
    wxSizer* endHourSizer;
    wxFlexGridSizer* pFlexSizer;

    wxCheckBox* pIncludeLoopFlow;
    wxCheckBox* pIncludePhaseShifts;
    wxCheckBox* pCheckNodalLoopFlow;
    wxTextCtrl* pInfiniteValue;

    Yuni::String pInfoFilename;

    wxButton* pBtnDelete;
    wxButton* pBtnBuild;
    Component::Button* pBtnPrefix;
    wxTimer* pProceedTimer;

    Component::Datagrid::Component* pGrid;

    wxRadioButton* pPrefixMesh;
    wxRadioButton* pPrefixGrid;
    wxRadioButton* pPrefixNetwork;

    bool pDelete;
    wxStaticText* pTextDelete;

    FileMappingPtr pFileMapping;

    using RendererType = Component::Datagrid::Renderer::ConstraintsBuilder::Links;
    RendererType* pRenderer;

    CBuilder* pCBuilder;

    DECLARE_EVENT_TABLE()

}; // class ConstraintsBuilderWizard

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_CONSTRAINTSBUILDER_H__
