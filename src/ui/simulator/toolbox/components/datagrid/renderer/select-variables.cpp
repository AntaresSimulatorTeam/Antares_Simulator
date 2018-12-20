
#include "select-variables.h"
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


	SelectVariables::SelectVariables() :
		pControl(nullptr)
	{
	}


	SelectVariables::~SelectVariables()
	{
	}


	bool SelectVariables::valid() const
	{
		if (!study)
			std::cout << "not valid ! \n";
		return !(!study);
	}


	int SelectVariables::height() const
	{
		return (not study) ? 0 : (int) study->parameters.nbYears;
	}

	wxString SelectVariables::columnCaption(int) const
	{
		return wxT("   Status   ");
	}


	wxString SelectVariables::rowCaption(int rowIndx) const
	{
		return wxString(wxT("var  ")) << (rowIndx + 1) << wxT("  ");
	}


	bool SelectVariables::cellValue(int, int y, const Yuni::String& value)
	{
		if (!(!study) && (uint) y < study->parameters.nbYears)
		{
			String s = value;
			s.trim();
			s.toLower();
			bool v = s.to<bool>() || s == "active" || s == "enabled";
			assert(study->parameters.yearsFilter);
			study->parameters.yearsFilter[y] = v;
			onTriggerUpdate();
			Dispatcher::GUI::Refresh(pControl);
			return true;
		}
		return false;
	}


	double SelectVariables::cellNumericValue(int, int y) const
	{
		if (!(!study) && (uint) y < study->parameters.nbYears)
		{
			assert(study->parameters.yearsFilter);
			return study->parameters.yearsFilter[y];
		}
		return 0.;
	}


	wxString SelectVariables::cellValue(int, int y) const
	{
		if (!(!study) && static_cast<uint>(y) < study->parameters.nbYears)
		{
			assert(study->parameters.yearsFilter);
			return study->parameters.yearsFilter[y] ? wxT("Active") : wxT("skip");
		}
		return wxEmptyString;
	}


	IRenderer::CellStyle SelectVariables::cellStyle(int, int y) const
	{
		if (!(!study) && (uint) y < study->parameters.nbYears)
		{
			assert(study->parameters.yearsFilter);
			return !study->parameters.yearsFilter[y]
				? IRenderer::cellStyleConstraintNoWeight : IRenderer::cellStyleConstraintWeight;
		}
		return IRenderer::cellStyleConstraintNoWeight;
	}






} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

