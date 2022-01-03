

#include "scenario-builder-ntc-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"
#include "../../../../application/study.h" // OnStudyChanged

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
    ntcScBuilderRenderer::ntcScBuilderRenderer()
    {
      OnStudyChanged.connect(this, &ntcScBuilderRenderer::onStudyChanged);
    }

    ntcScBuilderRenderer::~ntcScBuilderRenderer()
    {
        destroyBoundEvents();
    }

    void ntcScBuilderRenderer::onStudyChanged(Data::Study& study)
    {
      // Study has been created, snatch a reference to the (ui-runtime) links
      pLink = &study.uiinfo->pLink;
      // Force refresh
      invalidate = true;
    }

    bool ntcScBuilderRenderer::valid() const
    {
        return !(!study) && pRules && !study->areas.empty() && !(!pLink);
    }

    int ntcScBuilderRenderer::height() const
    {
       if (!pLink)
          return 0;
       return pLink->size();
    }

    wxString ntcScBuilderRenderer::rowCaption(int rowIndx) const
    {
      // Unlikely, but can happen if the study has not yet been loaded into memory
      if (!pLink)
        return "";
      if (rowIndx >= pLink->size())
        return "";
      return (*pLink)[rowIndx]->getName().c_str();
    }

    bool ntcScBuilderRenderer::cellValue(int x, int y, const String& value)
    {
      if (!pLink || !study || !pRules)
          return false;
      if ((uint)x >= study->parameters.nbYears)
          return false;
      if ((uint)y >= pLink->size())
          return false;

      const Data::AreaLink* link = (*pLink)[y];
      const uint areaIndex = link->from->index;
      uint val = fromStringToTSnumber(value);
      pRules->linksNTC[areaIndex].set(link, x, val);
      return true;
    }

    double ntcScBuilderRenderer::cellNumericValue(int x, int y) const
    {
      if (!pLink || !study || !pRules)
          return 0.;
      if ((uint)x >= study->parameters.nbYears || (uint)y >= pLink->size())
          return 0.;
      if ((uint)y >= pLink->size())
            return 0.;

      const Data::AreaLink* link = (*pLink)[y];
      const uint areaIndex = link->from->index;
      const uint linkIndex = link->indexForArea;
      return pRules->linksNTC[areaIndex].get_value(x, linkIndex);
    }
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
