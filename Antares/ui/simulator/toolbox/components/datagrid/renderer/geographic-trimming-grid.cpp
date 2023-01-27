#include "geographic-trimming-grid.h"
#include <antares/study/filter.h>
#include "windows/inspector.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
geographicTrimmingGrid::geographicTrimmingGrid() : pControl(nullptr)
{
}

geographicTrimmingGrid::~geographicTrimmingGrid()
{
}

bool geographicTrimmingGrid::valid() const
{
    if (!study)
        std::cout << "not valid ! \n";
    return !(!study);
}

int geographicTrimmingGrid::height() const
{
    return (not study) ? 0 : gridSize();
}

wxString geographicTrimmingGrid::columnCaption(int colIndx) const
{
    switch (colIndx)
    {
    case 0:
        return wxT(" YEAR BY YEAR \n hourly ");
    case 1:
        return wxT(" YEAR BY YEAR \n daily ");
    case 2:
        return wxT(" YEAR BY YEAR \n weekly ");
    case 3:
        return wxT(" YEAR BY YEAR \n monthly ");
    case 4:
        return wxT(" YEAR BY YEAR \n Annualy ");
    case 5:
        return wxT(" SYNTHESIS \n hourly ");
    case 6:
        return wxT(" SYNTHESIS \n daily ");
    case 7:
        return wxT(" SYNTHESIS \n weekly ");
    case 8:
        return wxT(" SYNTHESIS \n monthly ");
    case 9:
        return wxT(" SYNTHESIS \n Annualy ");
    }
    return wxEmptyString;
}

wxString geographicTrimmingGrid::rowCaption(int rowIndx) const
{
    return (!(!study) && (uint)rowIndx < gridSize()) ? wxStringFromUTF8(getName(rowIndx))
                                                     : wxString();
}

// Setting cell value
bool geographicTrimmingGrid::cellValue(int col, int row, const Yuni::String& value)
{
    if (!(!study) && (uint)row < gridSize())
    {
        // year-by-year trimming or synthesis trimming ?
        uint& filterToModify = (col < 5) ? getYearByYearFilter(row) : getSynthesisFilter(row);

        // Hourly ? Daily ? weekly ? ...
        uint flag = Data::addTimeIntervallToDatePrecisionFilter(col % 5);
        if (!flag)
            return false;

        // Current grid cell target value
        String s = value;
        s.trim();
        s.toLower();
        bool v = s.to<bool>() || s == "True";

        // Changing the filter value
        if (v)
            filterToModify |= flag;
        else
            filterToModify &= ~flag;

        onTriggerUpdate();
        Dispatcher::GUI::Refresh(pControl);
        Window::Inspector::Refresh();
        OnInspectorRefresh(nullptr);
        return true;
    }

    return false;
}

double geographicTrimmingGrid::cellNumericValue(int col, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        // year-by-year trimming or synthesis trimming ?
        uint& filter = (col < 5) ? getYearByYearFilter(row) : getSynthesisFilter(row);

        return (0 != (filter & Data::addTimeIntervallToDatePrecisionFilter(col % 5)));
    }

    return 0.;
}

// Getting cell value
wxString geographicTrimmingGrid::cellValue(int col, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        // year-by-year trimming or synthesis trimming ?
        uint& filter = (col < 5) ? getYearByYearFilter(row) : getSynthesisFilter(row);

        return (0 != (filter & Data::addTimeIntervallToDatePrecisionFilter(col % 5)))
                 ? wxT("True")
                 : wxT("False");
    }
    return wxEmptyString;
}

IRenderer::CellStyle geographicTrimmingGrid::cellStyle(int col, int row) const
{
    if (!(!study) && (uint)row < gridSize())
    {
        // year-by-year trimming or synthesis trimming ?
        uint& filter = (col < 5) ? getYearByYearFilter(row) : getSynthesisFilter(row);

        if (col < 5)
            return (0 != (filter & Data::addTimeIntervallToDatePrecisionFilter(col % 5)))
                     ? IRenderer::cellStyleFilterYearByYearOn
                     : IRenderer::cellStyleFilterYearByYearOff;
        else
            return (0 != (filter & Data::addTimeIntervallToDatePrecisionFilter(col % 5)))
                     ? IRenderer::cellStyleFilterSynthesisOn
                     : IRenderer::cellStyleFilterSynthesisOff;
    }
    return IRenderer::cellStyleFilterUndefined;
}

uint areasTrimmingGrid::gridSize() const
{
    return study->areas.size();
}

AreaLinkName areasTrimmingGrid::getName(int index) const
{
    return study->areas.byIndex[index]->name;
}

uint& areasTrimmingGrid::getSynthesisFilter(int index) const
{
    return study->areas.byIndex[index]->filterSynthesis;
}

uint& areasTrimmingGrid::getYearByYearFilter(int index) const
{
    return study->areas.byIndex[index]->filterYearByYear;
}

uint linksTrimmingGrid::gridSize() const
{
    return study->uiinfo->linkCount();
}

AreaLinkName linksTrimmingGrid::getName(int index) const
{
    auto* link = study->uiinfo->link((uint)(index));

    AreaLinkName to_return = link->from->id;
    to_return += " / ";
    to_return += link->with->id;

    return to_return;
}

uint& linksTrimmingGrid::getSynthesisFilter(int index) const
{
    return study->uiinfo->link((uint)(index))->filterSynthesis;
}

uint& linksTrimmingGrid::getYearByYearFilter(int index) const
{
    return study->uiinfo->link((uint)(index))->filterYearByYear;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares