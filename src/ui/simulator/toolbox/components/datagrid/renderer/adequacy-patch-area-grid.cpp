#include "adequacy-patch-area-grid.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
AdequacyPatchAreaGrid::AdequacyPatchAreaGrid() : pControl(nullptr)
{
}

AdequacyPatchAreaGrid::~AdequacyPatchAreaGrid()
{
}

bool AdequacyPatchAreaGrid::valid() const
{
    if (!study)
        std::cout << "not valid ! \n";
    return !(!study);
}

int AdequacyPatchAreaGrid::height() const
{
    return (not study) ? 0 : gridSize();
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

        bool vir = s == "0" || s == "virtual" || s == "virtual area";
        bool ins = s == "2" || s == "inside" || s == "physical area inside patch";
        bool out = s == "1" || s == "outside" || s == "physical area outside patch";

        if (vir)
            study->areas.byIndex[row]->adequacyPatchMode = Data::AdequacyPatch::adqmVirtualArea;
        else if (ins)
            study->areas.byIndex[row]->adequacyPatchMode
              = Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch;
        else
            study->areas.byIndex[row]->adequacyPatchMode
              = Data::AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch;

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
        // return study->areas.byIndex[row]->adequacyPatchMode;
    }
    return 0.;
}

wxString AdequacyPatchAreaGrid::cellValue(int, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        switch (study->areas.byIndex[row]->adequacyPatchMode)
        {
        case Data::AdequacyPatch::adqmVirtualArea:
            return wxT("virtual");
            break;
        case Data::AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch:
            return wxT("outside");
            break;
        case Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch:
            return wxT("inside");
            break;
        default:
            break;
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
        case Data::AdequacyPatch::adqmVirtualArea:
            return IRenderer::cellStyleAdqPatchVirtual;
            break;
        case Data::AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch:
            return IRenderer::cellStyleAdqPatchOutside;
            break;
        case Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch:
            return IRenderer::cellStyleAdqPatchInside;
            break;
        default:
            break;
        }
    }
    return IRenderer::cellStyleConstraintNoWeight;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares