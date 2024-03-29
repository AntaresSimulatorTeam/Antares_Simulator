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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_CORRELATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_CORRELATION_H__

#include "../renderer.h"
#include <memory>
#include <antares/study/study.h>

class wxWindow;
namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class CorrelationMatrix final : public Renderer::IRenderer
{
public:
    class IDatasource
    {
    public:
        using Ptr = std::shared_ptr<IDatasource>;

    public:
        IDatasource()
        {
        }
        virtual ~IDatasource()
        {
        }

        virtual wxString name() const = 0;
        virtual const char* icon() const = 0;
        virtual void reload() = 0;
        virtual uint size() const = 0;
        virtual const Data::Area* at(uint i) const = 0;
        virtual uint areaIndex(uint i) const = 0;

        virtual IRenderer::CellStyle cellStyle(int, int) const
        {
            return IRenderer::cellStyleDefault;
        }
        virtual wxColour cellBackgroundColor(int, int) const
        {
            return wxColour(255, 255, 255);
        }
        virtual wxColour cellTextColor(int, int) const
        {
            return wxColour(255, 0, 0);
        }

        wxString name(uint i) const;
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    CorrelationMatrix();

    //! Destructor
    virtual ~CorrelationMatrix();
    //@}

    void datasource(const IDatasource::Ptr& s);
    void matrix(Antares::Matrix<>* matrix)
    {
        pMatrix = matrix;
    }
    void control(wxWindow* control)
    {
        pControl = control;
    }

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
        return Date::stepAny;
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;
    virtual wxColour cellBackgroundColor(int, int) const;
    virtual wxColour cellTextColor(int, int) const;

    virtual bool valid() const;

protected:
    virtual void onUpdate();

    virtual void onStudyClosed() override;

private:
    IDatasource::Ptr pSource;
    Antares::Matrix<>* pMatrix;
    wxWindow* pControl;

}; // class CorrelationMatrix

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_CORRELATION_H__
