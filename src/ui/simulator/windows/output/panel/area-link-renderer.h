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
#ifndef __ANTARES_WINDOWS_OUTPUT_PANEL_AREA_LINK_RENDERER_H__
#define __ANTARES_WINDOWS_OUTPUT_PANEL_AREA_LINK_RENDERER_H__

#include "../../../toolbox/components/datagrid/renderer.h"
#include <yuni/core/string.h>
#include <antares/array/matrix.h>
#include "../output.h"

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
class AreaLinkRenderer final : public Antares::Component::Datagrid::Renderer::IRenderer
{
public:
    //! Convenient alias
    using IRenderer = Antares::Component::Datagrid::Renderer::IRenderer;
    //! The output viewer
    using OutputViewerComponent = Antares::Window::OutputViewer::Component;
    //! Grid
    using DatagridType = Antares::Component::Datagrid::Component;
    //!
    using CellType = Yuni::CString<64, false>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    AreaLinkRenderer(OutputViewerComponent* component, const Antares::Matrix<CellType>* matrix);

    //! Destructor
    virtual ~AreaLinkRenderer();
    //@}

    virtual int width() const;

    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int, int) const;

    virtual double cellNumericValue(int, int) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

    virtual IRenderer::CellStyle cellStyle(int, int) const;
    virtual wxColour cellBackgroundColor(int, int) const;
    virtual wxColour cellTextColor(int, int) const;
    virtual wxColour verticalBorderColor(int x, int y) const;
    virtual int cellAlignment(int, int) const;

    virtual bool valid() const;

    virtual void onScroll();
    virtual void onSelectCell(unsigned x, unsigned y);
    virtual void onExternalSelectCell(unsigned x, unsigned y);

    //! \name Grid
    //@{
    void grid(DatagridType* grid)
    {
        pGrid = grid;
    }
    //@}

protected:
    //! Reference to the parent control
    OutputViewerComponent* pComponent;
    //!
    DatagridType* pGrid;

    //! Matrix
    using MatrixType = Antares::Matrix<CellType>;
    //! The matrix
    const MatrixType* pMatrix;
    //! The pseudo Width
    uint pWidth;
    //! The pseudo height
    uint pHeight;
    //!
    std::vector<wxString> pColNames;
    std::vector<bool> pColToUseForRowCaption;

    //! Current selection
    uint pSelectionCellX;
    uint pSelectionCellY;

}; // class AreaLinkRenderer

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_PANEL_AREA_LINK_RENDERER_H__
