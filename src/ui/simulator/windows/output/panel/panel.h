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
#ifndef __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_H__
#define __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_H__

#include <antares/wx-wrapper.h>
#include <ui/common/component/panel.h>
#include "../fwd.h"
#include <vector>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <antares/array/matrix.h>
#include <yuni/core/event.h>

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
    typedef std::vector<Panel*> Vector;
    //! Layer
    typedef Antares::Window::OutputViewer::Layer Layer;
    //! The output viewer
    typedef Antares::Window::OutputViewer::Component OutputViewerComponent;

    //! Matrix for loading CSV files
    typedef Antares::Matrix<Yuni::CString<64, false>> MatrixType;

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

    //! Mutex
    Yuni::Mutex pMutex;
    //! Data
    IData* pData;
    //! Filename currently used
    Yuni::String pFilename;

    //! The total number of panels currently calling loadDataFromFile
    // This method is often an async call
    static Yuni::Atomic::Int<32> pPanelsInCallingLoadDataFromFile;

}; // class Panel

/*!
** \brief Provides mutex for file locking
**
** Since Windows open file in exclusive mode by default, we have to lock any
** concurrent access to the same file to be allow to open the file.
** This variable is cleared when the study is closed or the user get back
** to the input
*/
Yuni::SmartPtr<Yuni::Mutex> ProvideLockingForFileLocking(const YString& filename);

/*!
** \brief Clear all mutex for file locking
*/
void ClearAllMutexForFileLocking();

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#include "panel.hxx"

#endif // __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_H__
