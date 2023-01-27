
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
SelectVariables::SelectVariables() : pControl(nullptr)
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
    return (not study) ? 0 : (int)study->parameters.variablesPrintInfo.size();
}

wxString SelectVariables::columnCaption(int) const
{
    return wxT("   Status   ");
}

wxString SelectVariables::rowCaption(int rowIndx) const
{
    return wxString(wxT(" ")) << study->parameters.variablesPrintInfo[rowIndx]->name() << wxT("  ");
}

bool SelectVariables::cellValue(int, int var, const Yuni::String& value)
{
    if (!(!study) && (uint)var < study->parameters.variablesPrintInfo.size())
    {
        String s = value;
        s.trim();
        s.toLower();
        bool v = s.to<bool>() || s == "active" || s == "enabled";
        assert(!study->parameters.variablesPrintInfo.isEmpty());
        study->parameters.variablesPrintInfo[var]->enablePrint(v);
        onTriggerUpdate();
        Dispatcher::GUI::Refresh(pControl);
        return true;
    }
    return false;
}

double SelectVariables::cellNumericValue(int, int var) const
{
    if (!(!study) && (uint)var < study->parameters.variablesPrintInfo.size())
    {
        assert(!study->parameters.variablesPrintInfo.isEmpty());
        return study->parameters.variablesPrintInfo[var]->isPrinted();
    }
    return 0.;
}

wxString SelectVariables::cellValue(int, int var) const
{
    if (!(!study) && static_cast<uint>(var) < study->parameters.variablesPrintInfo.size())
    {
        assert(!study->parameters.variablesPrintInfo.isEmpty());
        return study->parameters.variablesPrintInfo[var]->isPrinted() ? wxT("Active") : wxT("skip");
    }
    return wxEmptyString;
}

IRenderer::CellStyle SelectVariables::cellStyle(int, int var) const
{
    if (!(!study) && (uint)var < study->parameters.variablesPrintInfo.size())
    {
        assert(!study->parameters.variablesPrintInfo.isEmpty());
        return !study->parameters.variablesPrintInfo[var]->isPrinted()
                 ? IRenderer::cellStyleConstraintNoWeight
                 : IRenderer::cellStyleConstraintWeight;
    }
    return IRenderer::cellStyleConstraintNoWeight;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
