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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_H__

#include <antares/wx-wrapper.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include "renderer.h"
#include <vector>
#include <yuni/core/math.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
/*!
** \brief Wrapper between a `wxGridTableBase` and a `renderer`
**
** This class is a custom implementation of a `wxGridTableBase`
** to make the interface with any renderer used to really provide
** the data to display (time-series, prepro...).
** It also manages custom filters, which may hide some rows. Si it
** provides pseudo aliases between rows/columns indexes.
*/
class VGridHelper final : public wxGridTableBase
{
public:
    //! Indices
    using Indices = std::vector<int>;

public:
    //! \name Constructor
    //@{
    VGridHelper(Renderer::IRenderer* renderer, bool markModified);
    //! Destructor
    virtual ~VGridHelper()
    {
    }
    //@}

    /*!
    ** \brief Get the number of rows that the wxGrid should display
    */
    virtual int GetNumberRows() override;

    /*!
    ** \brief Get the number of columns that the wxGrid should display
    */
    virtual int GetNumberCols() override;

    /*!
    ** \brief Get the label for a given row
    ** \param row The row index (zero-based)
    */
    virtual wxString GetRowLabelValue(int row) override;

    /*!
    ** \brief Get the label for a given colum
    ** \param col The column index (zero-based)
    */
    virtual wxString GetColLabelValue(int col) override;

    /*!
    ** \brief Get the string representation of the cell at a given position
    **
    ** \param row The row index (zero based)
    ** \param col The col index (zero based)
    ** \return The string representation
    */
    virtual wxString GetValue(int row, int col) override;

    virtual double GetValueAsDouble(int row, int col) override;

    virtual bool GetValueAsBool(int row, int col) override;

    /*!
    ** \brief Get the float value of the cell at a given position
    **
    ** There is no distinction between cells with a missing value and cells
    ** with a value equals to zero.
    ** \param row The row index (zero based)
    ** \param col The col index (zero based)
    ** \return The value of the cell
    */
    virtual double GetNumericValue(int row, int col) const; // override;

    /*!
    ** \brief Try to set the value of a given cell
    **
    ** \param row The row index (zero based)
    ** \param col The col index (zero based)
    ** \param v The new value of the cell, which may be converted
    */
    virtual void SetValue(int row, int col, const wxString& v) override;
    virtual void SetValue(int row, int col, const Yuni::String& v); // override

    /*!
    ** \brief Get if a cell is empty or not (unused)
    */
    bool IsEmptyCell(int, int) override;

    /*!
    ** \brief Get the real index used by the renderer of a given column index used by the wxGrid
    **
    ** This method is used to make the correspondance between raw data in
    ** memory and data displayed/modified by the wxGrid. In the most cases
    ** the indexes are strictly equals. But it may not be true when a filter
    ** is used.
    */
    int realCol(uint col) const;

    /*!
    ** \brief Get the real index used by the renderer of a given row index used by the wxGrid
    **
    ** This method is used to make the correspondance between raw data in
    ** memory and data displayed/modified by the wxGrid. In the most cases
    ** the indexes are strictly equals. But it may not be true when a filter
    ** is used.
    */
    int realRow(uint row) const;

    virtual bool CanValueAs(int, int, const wxString&); // override

    void resetIndicesToDefault();
    void resetIndicesToDefaultWithoutInit();

    void precision(const Date::Precision p);

    void loadFromMatrix();

    /*!
    ** \brief The renderer
    */
    Renderer::IRenderer* renderer() const;

    wxColour foregroundColorFromCellStyle(Renderer::IRenderer::CellStyle s) const;
    wxColour backgroundColorFromCellStyle(Renderer::IRenderer::CellStyle s) const;

    void appendCSSForAllCellStyles(wxString& s);
    void appendCSSForAllCellStyles2(Yuni::String& s);

    bool markTheStudyAsModified() const;

public:
    Indices indicesRows;
    Indices indicesCols;
    wxPoint virtualSize;
    //! Get if the grid is valid
    bool valid;

private:
    //! The renderer, the object that will really provide data
    Renderer::IRenderer* pRenderer;
    //! Datagrid precision
    Date::Precision pDataGridPrecision;

    bool pMarkStudyModifiedWhenModifyingCell;

}; // class VGridHelper

} // namespace Datagrid
} // namespace Component
} // namespace Antares

#include "gridhelper.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID__GRID_HELPER_H__
