#pragma once

#include "ts-management.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class TSmanagementAggregatedAsRenewable final : public TSmanagement
{
public:
	TSmanagementAggregatedAsRenewable();
	~TSmanagementAggregatedAsRenewable() = default;

	wxString columnCaption(int colIndx) const override;

}; // class TSmanagementAggregatedAsRenewable

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

