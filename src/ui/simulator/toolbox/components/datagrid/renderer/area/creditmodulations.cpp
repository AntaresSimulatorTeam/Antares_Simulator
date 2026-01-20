// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "creditmodulations.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
CreditModulations::CreditModulations(wxWindow* control, Toolbox::InputSelector::Area* notifier):
    MatrixAncestorType(control),
    Renderer::ARendererArea(control, notifier)
{
}

CreditModulations::~CreditModulations()
{
    destroyBoundEvents();
}

wxString CreditModulations::columnCaption(int colIndx) const
{
    return wxStringFromUTF8(colIndx);
}

wxString CreditModulations::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

double CreditModulations::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

bool CreditModulations::cellValue(int x, int y, const String& value)
{
    double v;
    if (not value.to(v))
    {
        return MatrixAncestorType::cellValue(x, y, "0");
    }
    if (v < 0)
    {
        return MatrixAncestorType::cellValue(x, y, "0");
    }
    if (v > 1000000)
    {
        return MatrixAncestorType::cellValue(x, y, "1000000");
    }

    return MatrixAncestorType::cellValue(x, y, String() << Math::Round(v, 2));
}

void CreditModulations::internalAreaChanged(Antares::Data::Area* area)
{
    // FIXME for some reasons, the variable study here is not properly initialized
    if (area && !study)
    {
        study = GetCurrentStudy();
    }

    Data::PartHydro* pHydro = (area) ? &(area->hydro) : nullptr;
    Renderer::ARendererArea::internalAreaChanged(area);
    if (pHydro)
    {
        MatrixAncestorType::matrix(&pHydro->creditModulation);
    }
    else
    {
        MatrixAncestorType::matrix(nullptr);
    }
}

IRenderer::CellStyle CreditModulations::cellStyle(int col, int row) const
{
    auto& matrix = pArea->hydro.creditModulation;
    double d = matrix[col][row];
    if (d < 0)
    {
        return IRenderer::cellStyleError;
    }
    return IRenderer::cellStyleWithNumericCheck(col, row);
}

wxString CreditModulations::rowCaption(int row) const
{
    if (row == 0)
    {
        return wxT("Generating Power");
    }
    else
    {
        return wxT("Pumping Power");
    }
}

void CreditModulations::onStudyClosed()
{
    MatrixAncestorType::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void CreditModulations::onStudyLoaded()
{
    MatrixAncestorType::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Antares::Component::Datagrid::Renderer
