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
    ntcScBuilderRenderer() = default;
    ~ntcScBuilderRenderer() = default;

    bool valid() const;
    int height() const;
    wxString rowCaption(int rowIndx) const;

    bool cellValue(int x, int y, const Yuni::String& value);
    double cellNumericValue(int x, int y) const;
}; // class ntcScBuilderRenderer

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
