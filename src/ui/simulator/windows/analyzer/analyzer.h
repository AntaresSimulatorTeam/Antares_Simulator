/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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
#ifndef __ANTARES_APPLICATION_WINDOW_ANALYZER_H__
#define __ANTARES_APPLICATION_WINDOW_ANALYZER_H__

#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/timer.h>
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/analyzer/areas.h"
#include <ui/common/component/spotlight.h>
#include <yuni/thread/thread.h>
#include <yuni/core/event.h>

#include <memory>

namespace Antares
{
namespace Window
{
class FileSearchProvider final : public Antares::Component::Spotlight::IProvider
{
public:
    //! The spotlight component (alias)
    using Spotlight = Antares::Component::Spotlight;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    FileSearchProvider() = default;
    //! Destructor
    ~FileSearchProvider() override = default;
    //@}

    /*!
    ** \brief Perform a new search
    */
    void search(Spotlight::IItem::Vector& out,
                const Spotlight::SearchToken::Vector& tokens,
                const Yuni::String& text = "") override;

    /*!
    ** \brief An item has been selected
    */
    bool onSelect(Spotlight::IItem::Ptr&) override;

    void onFileSearchAdd(const YString& filename);

    void onFileSearchClear();

private:
    //! List of files
    YString::Vector pFiles;
};

class AnalyzerWizard final : public wxDialog
{
public:
    //! File mapping
    using FileMapping = Component::Datagrid::Renderer::Analyzer::Areas::Map;
    //!
    using RecordVector = Component::Datagrid::Renderer::Analyzer::Areas::Record::Vector;
    //! Smartptr for file mapping
    using FileMappingPtr = std::shared_ptr<FileMapping>;

    enum IDs
    {
        mnIDCancel = wxID_HIGHEST + 1,
        mnIDTmpUseDefault,
        mnIDTmpUseStudyUserDir,
        mnIDTmpUseCustom,
        mnIDAutoClean,
        mnIDUpdateInfo,
        mnIDUpdateFileMapping,
        mnIDGridUpdate,
        mnIDCanRunAnalyzer,
    };

public:
    /*!
    ** \brief Reset the address of the last used folder
    **
    ** The variable will be reset to <current study>/user.
    */
    static void ResetLastFolderToCurrentStudyUser();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent frame
    */
    AnalyzerWizard(wxFrame* parent);
    //! Destructor
    virtual ~AnalyzerWizard();
    //@}

    void info(const Yuni::NullPtr&);
    void info(const AnyString& text);

    void fileMapping(FileMappingPtr m);

    /*!
    ** \brief Force the refresh of the grid
    */
    void refreshGrid();

    void enableAnalyzer(bool v);

    /*!
    ** \brief Force the analyze of each area
    */
    void recheckEachArea();

    /*!
    ** \brief Export all infos into an INI file
    */
    bool saveToFile(const Yuni::String& filename) const;

    const Yuni::String& analyzerInfoFile() const
    {
        return pInfoFilename;
    }

    void analyzerInfoFile(const Yuni::String& f)
    {
        pInfoFilename = f;
    }

private:
    void onCancel(void*);
    void onProceed(void*);

    void onBrowse(void*);
    void onBrowseMenu(Component::Button&, wxMenu& menu, void*);
    void onBrowseRefresh(wxCommandEvent&);
    void onBrowseOpenInExplorer(wxCommandEvent&);
    void onBrowseReset(wxCommandEvent&);

    void onSelectTimeseries(Component::Button&, wxMenu& menu, void*);
    void onSelectTSLoad(wxCommandEvent&);
    void onSelectTSWind(wxCommandEvent&);
    void onSelectTSSolar(wxCommandEvent&);

    void onBrowseTemp(void* evt);
    void evtUpdateInfo(wxCommandEvent& evt);
    void evtUpdateFileMapping(wxCommandEvent& evt);
    void evtGridUpdate(wxCommandEvent& evt);
    void evtCanRunAnalyzer(wxCommandEvent& evt);
    void evtTemporaryPathSelectorChanged(wxCommandEvent& evt);
    void evtLimitsChanged(wxCommandEvent& evt);
    void updateInfoForTempFolder();
    void updateMaxTimeseries();

    void browseDataFolder(const wxString& p);

    void enableAll(bool v);

    void beforeUpdate(int, int);
    void afterUpdate(int, int);

private:
    Yuni::Event<void(bool, uint)> onUpdateMaxTimeseries;
    Yuni::Event<void(Data::TimeSeriesType)> onUpdateTimeseriesType;

    //! \name Filesearch
    //@{
    Yuni::Event<void()> onFileSearchClear;
    Yuni::Event<void(const Yuni::String&)> onFileSearchAdd;
    //@}

    Yuni::String pInfoFilename;
    wxTextCtrl* pPath;
    wxTextCtrl* pPathTemp;
    wxButton* pBtnRun;
    wxButton* pBtnBrowseTemp;
    wxCheckBox* pAutoClean;
    wxRadioButton* pTmpUseDefault;
    wxRadioButton* pTmpUseStudyUserDir;
    wxRadioButton* pTmpUseCustom;
    wxStaticText* pLblInfo;
    wxStaticText* pLblCleanInfo;
    Component::Datagrid::Component* pGrid;
    wxTimer* pRefreshTimer;
    wxTimer* pProceedTimer;

    wxCheckBox* pCbMaxTimeseries;
    wxCheckBox* pCbUpperBound;
    wxCheckBox* pCbLowerBound;

    wxTextCtrl* pEdShortTermAutoCorr;
    wxTextCtrl* pEdMediumTermAutoCorr;
    wxTextCtrl* pEdTrim;
    wxTextCtrl* pEdMaxTimeseries;
    wxTextCtrl* pEdLowerBound;
    wxTextCtrl* pEdUpperBound;

    Yuni::Thread::IThread* pAnalyzeSource;
    Yuni::Thread::IThread* pCheckRelationship;
    FileMappingPtr pFileMapping;

    Component::Spotlight* pFileSearch;

    Data::TimeSeriesType pTSSelected;
    Component::Button* pTSSelector;

    bool pUpdating;

    using RendererType = Component::Datagrid::Renderer::Analyzer::Areas;
    RendererType* pRenderer;

    std::shared_ptr<FileSearchProvider> mProvider;

    DECLARE_EVENT_TABLE()

}; // class AnalyzerWizard

} // namespace Window
} // namespace Antares

#include "analyzer.hxx"

#endif // __ANTARES_APPLICATION_WINDOW_ANALYZER_H__
