

#include "scenario-builder-ntc-renderer.h"
// #include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
    bool ntcScBuilderRenderer::valid() const
    {
        // return !(!study) && pRules && study->areas.size() != 0 && !(!pRules) && pArea;
        return true;
    }

    int ntcScBuilderRenderer::height() const
    {
        // to be filled
        return 0;
    }

    wxString ntcScBuilderRenderer::rowCaption(int rowIndx) const
    {
        // to be filled 
        return wxEmptyString;
    }

    bool ntcScBuilderRenderer::cellValue(int x, int y, const String& value)
    {
        // to be filled
        return false;
    }

    double ntcScBuilderRenderer::cellNumericValue(int x, int y) const
    {
        // to be filled
        return 0.;
    }

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
