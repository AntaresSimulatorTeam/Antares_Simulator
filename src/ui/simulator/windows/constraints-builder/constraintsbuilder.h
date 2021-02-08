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
#ifndef __ANTARES_APPLICATION_WINDOW_CONSTRAINTSBUILDER_H__
#define __ANTARES_APPLICATION_WINDOW_CONSTRAINTSBUILDER_H__

#include <antares/wx-wrapper.h>
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

namespace Antares
{
namespace Window
{
class ConstraintsBuilderWizard final : public wxDialog
{
public:
    //! File mapping
    typedef Component::Datagrid::Renderer::ConstraintsBuilder::Links::Map FileMapping;
    //!
    typedef Component::Datagrid::Renderer::ConstraintsBuilder::Links::Record::Vector RecordVector;
    //! Smartptr for file mapping
    typedef Yuni::SmartPtr<FileMapping> FileMappingPtr;

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
    /*

    public:
            /*!
            ** \brief Reset the address of the last used folder
            **
            ** The variable will be reset to <current study>/user.
            */
    // static void ResetLastFolderToCurrentStudyUser();

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

    void fileMapping(FileMapping* m);

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

    typedef Component::Datagrid::Renderer::ConstraintsBuilder::Links RendererType;
    RendererType* pRenderer;

    CBuilder* pCBuilder;

    DECLARE_EVENT_TABLE()

}; // class ConstraintsBuilderWizard

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_CONSTRAINTSBUILDER_H__
