#pragma once

#include <string>
#include <antares/study/fwd.h>

namespace Antares
{
namespace Data
{

	std::string transmissionCapacitiesToString(const TransmissionCapacities & tc)
	{
		std::string to_return;
		switch (tc)
		{
		case Data::tncEnabled:
			to_return = "enabled";
            break;
		case Data::tncInfinite:
			to_return = "infinite";
            break;
        default:
            to_return = "ignore";
            break;
		}
		return to_return;
	}

	std::string assetTypeToString(const AssetType & assetType)
	{
        std::string to_return;
        switch (assetType)
        {
        case Data::atAC:
            to_return = "ac";
            break;
        case Data::atDC:
            to_return = "dc";
            break;
        case Data::atGas:
            to_return = "gaz";
            break;
        case Data::atVirt:
            to_return = "virt";
            break;
        default:
            to_return = "other";
            break;
        }
        return to_return;
	}

    std::string styleToString(const StyleType& style)
    {
        std::string to_return;
        switch (style)
        {
        case Data::stDot:
            to_return = "dot";
            break;
        case Data::stDash:
            to_return = "dash";
            break;
        case Data::stDotDash:
            to_return = "dotdash";
            break;
        default:
            to_return = "plain";
            break;
        }
        return to_return;
    }
}
}
