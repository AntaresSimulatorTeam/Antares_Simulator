#pragma once

#include <string>
#include <antares/study/fwd.h>

namespace Antares
{
namespace Data
{

	std::string transmissionCapacitiesToString(const TransmissionCapacities & tc)
	{
		switch (tc)
		{
		case Data::tncEnabled:
			return "enabled";
		case Data::tncInfinite:
			return "infinite";
        default:
            return "ignore";
		}
	}

	std::string assetTypeToString(const AssetType & assetType)
	{
        switch (assetType)
        {
        case Data::atAC:
            return "ac";
        case Data::atDC:
            return "dc";
        case Data::atGas:
            return "gaz";
        case Data::atVirt:
            return "virt";
        default:
            return "other";
        }
	}

    std::string styleToString(const StyleType& style)
    {
        switch (style)
        {
        case Data::stDot:
            return "dot";
        case Data::stDash:
            return "dash";
        case Data::stDotDash:
            return "dotdash";
        default:
            return "plain";
        }
    }
}
}
