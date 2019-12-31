#include "areas-trimming-grid.h"
#include <antares/study/filter.h>
#include "../../../../windows/inspector.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	areasTrimmingGrid::areasTrimmingGrid() :
		pControl(nullptr)
	{
	}


	areasTrimmingGrid::~areasTrimmingGrid()
	{
	}


	bool areasTrimmingGrid::valid() const
	{
		if (!study)
			std::cout << "not valid ! \n";
		return !(!study);
	}


	int areasTrimmingGrid::height() const
	{
		return (not study) ? 0 : (int)study->areas.size();
	}

	wxString areasTrimmingGrid::columnCaption(int colIndx) const
	{
		switch (colIndx)
		{
		case 0: return wxT(" YEAR BY YEAR \n hourly ");
		case 1: return wxT(" YEAR BY YEAR \n daily ");
		case 2: return wxT(" YEAR BY YEAR \n weekly ");
		case 3: return wxT(" YEAR BY YEAR \n monthly ");
		case 4: return wxT(" YEAR BY YEAR \n Annualy ");
		case 5: return wxT(" SYNTHESIS \n hourly ");
		case 6: return wxT(" SYNTHESIS \n daily ");
		case 7: return wxT(" SYNTHESIS \n weekly ");
		case 8: return wxT(" SYNTHESIS \n monthly ");
		case 9: return wxT(" SYNTHESIS \n Annualy ");
		}
		return wxEmptyString;
	}


	wxString areasTrimmingGrid::rowCaption(int rowIndx) const
	{
		return (!(!study) && (uint)rowIndx < study->areas.size())
			? wxStringFromUTF8(study->areas.byIndex[rowIndx]->name)
			: wxString();
	}

	// Setting cell value
	bool areasTrimmingGrid::cellValue(int col, int row, const Yuni::String& value)
	{
		if (!(!study) && (uint)row < study->areas.size())
		{
			// year-by-year trimming or synthesis trimming ?
			uint& filterToModify = (col < 5) ? study->areas.byIndex[row]->filterYearByYear : study->areas.byIndex[row]->filterSynthesis;
			
			// Hourly ? Daily ? weekly ? ...
			uint flag = Data::filterIndexToFilter(col % 5);
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


	double areasTrimmingGrid::cellNumericValue(int col, int row) const
	{
		if (!(!study) && (uint)row < study->areas.size())
		{
			// year-by-year trimming or synthesis trimming ?
			uint& filter = (col < 5) ? study->areas.byIndex[row]->filterYearByYear : study->areas.byIndex[row]->filterSynthesis;

			return  (0 != (filter & Data::filterIndexToFilter(col % 5)));
		}
		
		return 0.;
	}

	// Getting cell value
	wxString areasTrimmingGrid::cellValue(int col, int row) const
	{
		if (!(!study) && (uint)row < study->areas.size())
		{
			// year-by-year trimming or synthesis trimming ?
			uint& filter = (col < 5) ? study->areas.byIndex[row]->filterYearByYear : study->areas.byIndex[row]->filterSynthesis;

			return  (0 != (filter & Data::filterIndexToFilter(col % 5))) ? wxT("True") : wxT("False");
		}
		return wxEmptyString;
	}


	IRenderer::CellStyle areasTrimmingGrid::cellStyle(int col, int row) const
	{
		if (!(!study) && (uint)row < study->areas.size())
		{
			// year-by-year trimming or synthesis trimming ?
			uint& filter = (col < 5) ? study->areas.byIndex[row]->filterYearByYear : study->areas.byIndex[row]->filterSynthesis;

			if (col < 5)
				return  (0 != (filter & Data::filterIndexToFilter(col % 5))) ? 
					IRenderer::cellStyleFilterYearByYearOn : IRenderer::cellStyleFilterYearByYearOff;
			else
				return  (0 != (filter & Data::filterIndexToFilter(col % 5))) ?
					IRenderer::cellStyleFilterSynthesisOn : IRenderer::cellStyleFilterSynthesisOff;
		}
		return IRenderer::cellStyleFilterUndefined;
	}



} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares