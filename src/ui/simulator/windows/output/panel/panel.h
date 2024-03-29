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
#ifndef __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_H__
#define __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_H__

#include <ui/common/component/panel.h>
#include "../fwd.h"
#include <vector>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <antares/array/matrix.h>
#include <yuni/core/event.h>

#include <mutex>
#include <memory>
#include <atomic>

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
class Panel final : public Antares::Component::Panel
{
public:
    //! Vector
    using Vector = std::vector<Panel*>;
    //! Layer
    using Layer = Antares::Window::OutputViewer::Layer;
    //! The output viewer
    using OutputViewerComponent = Antares::Window::OutputViewer::Component;

    //! Matrix for loading CSV files
    using MatrixType = Antares::Matrix<Yuni::CString<64, false>>;

    class IData
    {
    public:
        IData()
        {
        }
        virtual ~IData()
        {
        }
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Panel(OutputViewerComponent* component, wxWindow* parent);
    //! Destructor
    virtual ~Panel();
    //@}

    //! \name Index
    //@{
    //! Set the index
    void index(uint i);
    //! Get the index
    uint index() const;
    //@}

    //! \name Layer
    //@{
    //! Set the attached layer
    void layer(Layer* newLayer, bool forceUpdate = false);
    //! Get the attached layer (const)
    const Layer* layer() const;
    //! Get the attached layer
    Layer* layer();
    //@}

    //! \name Filename
    //@{
    //! Get the filename currently used (empty if the layer is virtual)
    const Yuni::String& filename() const;
    //@}

    //! \name Refresh
    //@{
    /*!
    ** \brief Force the GUI refresh
    */
    void forceRefresh();
    //@}

    //! \name Messages
    //@{
    /*!
    ** \brief Replace the whole panel by a message with an icon
    */
    void message(const wxString& msg, const char* image = NULL);

    void messageMergeYbY();
    //@}

    void loadVirtualLayer();
    void loadDataFromFile();
    void loadDataFromMatrix(MatrixType* matrix);
    void clearAllComponents();

private:
    void onProceed(void*);
    void runMerge();
    void executeAggregator();
    void noData();

private:
    //! Reference to the parent control
    OutputViewerComponent* pComponent;
    //! The attached layer
    Layer* pLayer;
    //! Index
    uint pIndex;

    //! Sizer for sub components
    wxSizer* pSizer;
    //! Text label
    wxWindow* pLabelMessage;
    wxStaticBitmap* pIconMessage;
    wxWindow* pButton;
    bool pShouldRebuildMessage;

    std::mutex pMutex;
    //! Data
    IData* pData;
    //! Filename currently used
    Yuni::String pFilename;

    //! The total number of panels currently calling loadDataFromFile
    // This method is often an async call
    static std::atomic<int> pPanelsInCallingLoadDataFromFile;

}; // class Panel

/*!
** \brief Provides mutex for file locking
**
** Since Windows open file in exclusive mode by default, we have to lock any
** concurrent access to the same file to be allow to open the file.
** This variable is cleared when the study is closed or the user get back
** to the input
*/
std::shared_ptr<std::mutex> ProvideLockingForFileLocking(const YString& filename);

/*!
** \brief Clear all mutex for file locking
*/
void ClearAllMutexForFileLocking();

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#include "panel.hxx"

#endif // __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_H__
