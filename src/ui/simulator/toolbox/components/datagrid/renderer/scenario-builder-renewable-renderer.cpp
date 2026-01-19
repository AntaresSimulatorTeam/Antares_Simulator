// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scenario-builder-renewable-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares::Component::Datagrid::Renderer
{
int renewableScBuilderRenderer::height() const
{
    if (!(!study) && !(!pRules) && selectedArea())
    {
        return (int)selectedArea()->renewable.list.allClustersCount();
    }
    return 0;
}

wxString renewableScBuilderRenderer::rowCaption(int rowIndx) const
{
    if (!(!study) && !(!pRules) && selectedArea()
        && (uint)rowIndx < selectedArea()->renewable.list.allClustersCount())
    {
        return wxString() << wxT(" ")
                          << wxStringFromUTF8(selectedArea()->renewable.list[rowIndx]->name())
                          << wxT("  ");
    }
    return wxEmptyString;
}

bool renewableScBuilderRenderer::cellValue(int x, int y, const String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->renewable.list.allClustersCount())
    {
        assert(selectedArea()->index < pRules->areaCount());
        assert((uint)y < pRules->renewable[selectedArea()->index].width());
        assert((uint)x < pRules->renewable[selectedArea()->index].height());
        uint val = fromStringToTSnumber(value);
        pRules->renewable[selectedArea()->index]
          .setTSnumber(selectedArea()->renewable.list[y].get(), x, val);
        return true;
    }
    return false;
}

double renewableScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->renewable.list.allClustersCount())
    {
        assert((uint)y < pRules->renewable[selectedArea()->index].width());
        assert((uint)x < pRules->renewable[selectedArea()->index].height());
        return pRules->renewable[selectedArea()->index].get_value(x, y);
    }
    return 0.;
}

} // namespace Antares::Component::Datagrid::Renderer
