#include "adequacy-patch-area-grid.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
AdequacyPatchAreaGrid::AdequacyPatchAreaGrid() : pControl(nullptr)
{
}

AdequacyPatchAreaGrid::~AdequacyPatchAreaGrid() = default;

bool AdequacyPatchAreaGrid::valid() const
{
    if (!study)
        std::cout << "not valid ! \n";
    return !(!study);
}

int AdequacyPatchAreaGrid::height() const
{
    return (!study) ? 0 : gridSize();
}

wxString AdequacyPatchAreaGrid::columnCaption(int) const
{
    return wxT(" Adequacy Patch Area \n Mode ");
}

wxString AdequacyPatchAreaGrid::rowCaption(int rowIndx) const
{
    return (!(!study) && (uint)rowIndx < gridSize())
             ? wxStringFromUTF8(study->areas.byIndex[rowIndx]->name)
             : wxString();
}

bool AdequacyPatchAreaGrid::cellValue(int, int row, const Yuni::String& value)
{
    if (!(!study) && (uint)row < gridSize())
    {
        String s = value;
        s.trim();
        s.toLower();

        bool vir = s == "0" || s == "virtual" || s == "v";
        bool ins = s.to<int>() == 2 || s == "inside" || s == "i";

        if (vir)
            study->areas.byIndex[row]->adequacyPatchMode = Data::AdequacyPatch::virtualArea;
        else if (ins)
            study->areas.byIndex[row]->adequacyPatchMode
              = Data::AdequacyPatch::physicalAreaInsideAdqPatch;
        else
            study->areas.byIndex[row]->adequacyPatchMode
              = Data::AdequacyPatch::physicalAreaOutsideAdqPatch;

        onTriggerUpdate();
        Dispatcher::GUI::Refresh(pControl);
        return true;
    }
    return false;
}

double AdequacyPatchAreaGrid::cellNumericValue(int, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        assert(gridSize() != 0);
        // for saving into *.txt file
        switch (study->areas.byIndex[row]->adequacyPatchMode)
        {
        case Data::AdequacyPatch::virtualArea:
            return 0.;
        case Data::AdequacyPatch::physicalAreaOutsideAdqPatch:
            return 1.;
        case Data::AdequacyPatch::physicalAreaInsideAdqPatch:
            return 2.;
        default:
           return 0.;
        }
    }
    return 0.;
}

wxString AdequacyPatchAreaGrid::cellValue(int, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        switch (study->areas.byIndex[row]->adequacyPatchMode)
        {
        case Data::AdequacyPatch::virtualArea:
            return wxT("virtual");
        case Data::AdequacyPatch::physicalAreaOutsideAdqPatch:
            return wxT("outside");
        case Data::AdequacyPatch::physicalAreaInsideAdqPatch:
            return wxT("inside");
        default:
            return wxEmptyString;
        }
    }
    return wxEmptyString;
}

uint AdequacyPatchAreaGrid::gridSize() const
{
    return study->areas.size();
}

IRenderer::CellStyle AdequacyPatchAreaGrid::cellStyle(int, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        switch (study->areas.byIndex[row]->adequacyPatchMode)
        {
        case Data::AdequacyPatch::virtualArea:
            return IRenderer::cellStyleAdqPatchVirtual;
        case Data::AdequacyPatch::physicalAreaOutsideAdqPatch:
            return IRenderer::cellStyleAdqPatchOutside;
        case Data::AdequacyPatch::physicalAreaInsideAdqPatch:
            return IRenderer::cellStyleAdqPatchInside;
        default:
            return IRenderer::cellStyleConstraintNoWeight;
        }
    }
    return IRenderer::cellStyleConstraintNoWeight;
}
}
