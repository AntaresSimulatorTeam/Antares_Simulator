// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scenario-builder-thermal-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares::Component::Datagrid::Renderer
{
int thermalScBuilderRenderer::height() const
{
    if (!(!study) && !(!pRules) && selectedArea())
    {
        return (int)selectedArea()->thermal.list.allClustersCount();
    }
    return 0;
}

wxString thermalScBuilderRenderer::rowCaption(int rowIndx) const
{
    if (!(!study) && !(!pRules) && selectedArea()
        && (uint)rowIndx < selectedArea()->thermal.list.allClustersCount())
    {
        return wxString() << wxT(" ")
                          << wxStringFromUTF8(selectedArea()->thermal.list[rowIndx]->name())
                          << wxT("  ");
    }
    return wxEmptyString;
}

bool thermalScBuilderRenderer::cellValue(int x, int y, const String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->thermal.list.allClustersCount())
    {
        assert(selectedArea()->index < pRules->areaCount());
        assert((uint)y < pRules->thermal[selectedArea()->index].width());
        assert((uint)x < pRules->thermal[selectedArea()->index].height());
        uint val = fromStringToTSnumber(value);
        pRules->thermal[selectedArea()->index].setTSnumber(selectedArea()->thermal.list[y].get(),
                                                           x,
                                                           val);
        return true;
    }
    return false;
}

double thermalScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->thermal.list.allClustersCount())
    {
        assert((uint)y < pRules->thermal[selectedArea()->index].width());
        assert((uint)x < pRules->thermal[selectedArea()->index].height());
        return pRules->thermal[selectedArea()->index].get_value(x, y);
    }
    return 0.;
}

} // namespace Antares::Component::Datagrid::Renderer
