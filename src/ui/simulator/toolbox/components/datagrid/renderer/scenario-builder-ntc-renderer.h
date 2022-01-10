#pragma once

#include "scenario-builder-renderer-base.h"
#include "../../../input/connection.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ntcScBuilderRenderer : public ScBuilderRendererBase
{
public:
    ntcScBuilderRenderer();

    ntcScBuilderRenderer& operator=(const ntcScBuilderRenderer&) = delete;
    ntcScBuilderRenderer(const ntcScBuilderRenderer&) = delete;

    bool valid() const;
    int height() const override;
    wxString rowCaption(int rowIndx) const override;

    bool cellValue(int x, int y, const Yuni::String& value) override;
    double cellNumericValue(int x, int y) const override;

private:
    void onStudyChanged(Data::Study& study);
    Data::AreaLink::Vector* pListOfLinks = nullptr;
}; // class ntcScBuilderRenderer

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
