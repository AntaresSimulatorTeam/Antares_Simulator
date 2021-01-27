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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MATRIX_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MATRIX_H__

#include <antares/wx-wrapper.h>
#include "../renderer.h"
#include "../../../input/area.h"

#include "../component.h"
#include <antares/date.h>
#include "../../../../application/study.h"
#include <ui/common/lock.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
// Forward declaration
class Component;

namespace Renderer
{
template<class T = double, class ReadWriteT = T, uint PrecisionT = 6>
class Matrix : public virtual IRenderer
{
public:
    enum
    {
        //! Precision (double/float only)
        matrixPrecision = PrecisionT,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \parent parent The parent window
    ** \param matrix A matrix
    */
    explicit Matrix(wxWindow* parent, Antares::Matrix<T, ReadWriteT>* matrix = nullptr);

    //! Destructor
    virtual ~Matrix();
    //@}

    //! \name Size
    //@{
    //! Get the width of the datagrid
    virtual int width() const override;
    //! Get the height of the datagrid
    virtual int height() const override;
    //@}

    //! \name Captions
    //@{
    //! Get the column caption
    virtual wxString columnCaption(int colIndx) const override;
    //! Get the row caption
    virtual wxString rowCaption(int rowIndx) const override;
    //@}

    //! \name Cells
    //@{
    /*!
    ** \brief Get the value of a cell
    **
    ** \param x The X-Coordinate of the cell
    ** \param y The Y-Coordinate of the cell
    */
    virtual wxString cellValue(int x, int y) const override;

    /*!
    ** \brief Get the value of a cell
    **
    ** \param x The X-Coordinate of the cell
    ** \param y The Y-Coordinate of the cell
    */
    virtual double cellNumericValue(int x, int y) const override;

    /*!
    ** \brief Set the value of a cell
    **
    ** \param x The X-Coordinate of the cell
    ** \param y The Y-Coordinate of the cell
    ** \param value The new value
    ** \return True if the operation succeeded, false otherwise (the cell cannot be modified)
    */
    virtual bool cellValue(int x, int y, const Yuni::String& value) override;
    //@}

    //! \name Formatting
    //@{
    virtual IRenderer::CellStyle cellStyle(int, int y) const override;

    virtual void resetColors(int, int, wxColour&, wxColour&) const override
    { /* Do nothing*/
    }
    //@}

    //! \name Matrix
    //@{
    //! Get the linked matrix
    Antares::Matrix<T, ReadWriteT>* matrix() const;
    //! Set the linked matrix
    void matrix(Antares::Matrix<T, ReadWriteT>* m);
    //@}

    virtual bool ensureDataAreLoaded() override;

    virtual uint maxWidthResize() const override
    {
        return 1000;
    }

    virtual void resizeMatrixToXColumns(uint m) override;

    virtual bool valid() const override;

    virtual bool circularShiftRowsUntilDate(MonthName month, uint daymonth) override;

protected:
    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

protected:
    //! The source control
    wxWindow* pControl;
    //! The linked matrix
    Antares::Matrix<T, ReadWriteT>* pMatrix;

}; // class Matrix

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#include "matrix.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MATRIX_H__
