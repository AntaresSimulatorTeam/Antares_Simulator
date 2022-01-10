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
    int height() const;
    wxString rowCaption(int rowIndx) const;

    bool cellValue(int x, int y, const Yuni::String& value);
    double cellNumericValue(int x, int y) const;

private:
    void onStudyChanged(Data::Study& study);
    Data::AreaLink::Vector* pListOfLinks = nullptr;
}; // class ntcScBuilderRenderer

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
