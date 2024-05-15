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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMALPREPRO_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMALPREPRO_H__

#include "../../../../input/thermal-cluster.h"
#include "../matrix.h"
#include <antares/solver/ts-generator/prepro.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ThermalClusterPrepro : public Renderer::Matrix<>
{
public:
    using MatrixAncestorType = Renderer::Matrix<>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ThermalClusterPrepro(wxWindow* control, Toolbox::InputSelector::ThermalCluster* notifier);

    //! Destructor
    virtual ~ThermalClusterPrepro();
    //@}

    virtual int width() const
    {
        return 8;
    }
    virtual int height() const
    {
        return DAYS_PER_YEAR;
    }

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int, int, const Yuni::String&);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual Date::Precision precision()
    {
        return Date::daily;
    }

    virtual bool valid() const
    {
        return MatrixAncestorType::valid();
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual wxColour verticalBorderColor(int x, int y) const;
    virtual wxColour horizontalBorderColor(int x, int y) const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual uint maxHeightResize() const
    {
        return 0;
    }

protected:
    virtual void internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster);

private:
    virtual void onStudyClosed() override;

private:
    Antares::Data::PreproAvailability* pPreproAvailability = nullptr;
    Antares::Data::ThermalCluster* pCluster;

}; // class ThermalClusterPrepro

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_THERMALPREPRO_H__
