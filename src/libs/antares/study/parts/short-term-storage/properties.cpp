/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/study/parts/short-term-storage/properties.h"

#include <stdexcept>

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/logs/logs.h>

namespace Antares::Data::ShortTermStorage
{

bool Properties::loadKey(const IniFile::Property* p)
{
    auto valueForOptional = [p](std::optional<double>& opt)
    {
        if (double tmp; p->value.to<double>(tmp))
        {
            opt = tmp;
            return true;
        }
        return false;
    };

    if (p->key == "injectionnominalcapacity")
    {
        return valueForOptional(this->injectionNominalCapacity);
    }

    if (p->key == "withdrawalnominalcapacity")
    {
        return valueForOptional(this->withdrawalNominalCapacity);
    }

    if (p->key == "reservoircapacity")
    {
        return valueForOptional(this->reservoirCapacity);
    }

    if (p->key == "efficiency")
    {
        return p->value.to<double>(this->injectionEfficiency);
    }

    if (p->key == "efficiencywithdrawal")
    {
        return p->value.to<double>(this->withdrawalEfficiency);
    }

    if (p->key == "name")
    {
        return p->value.to<std::string>(this->name);
    }

    if (p->key == "initiallevel")
    {
        return p->value.to<double>(this->initialLevel);
    }

    if (p->key == "initialleveloptim")
    {
        return p->value.to<bool>(this->initialLevelOptim);
    }

    if (p->key == "group")
    {
        this->groupName = p->value.c_str();
        boost::to_upper(this->groupName);
        return true;
    }

    if (p->key == "penalize-variation-withdrawal")
    {
        return p->value.to<bool>(this->penalizeVariationWithdrawal);
    }

    if (p->key == "penalize-variation-injection")
    {
        return p->value.to<bool>(this->penalizeVariationInjection);
    }

    if (p->key == "enabled")
    {
        return p->value.to<bool>(this->enabled);
    }

    return false;
}

void Properties::save(IniFile& ini) const
{
    IniFile::Section* s = ini.addSection(this->name);

    s->add("name", this->name);
    s->add("group", this->groupName);
    s->add("reservoircapacity", this->reservoirCapacity);
    s->add("initiallevel", this->initialLevel);
    s->add("injectionnominalcapacity", this->injectionNominalCapacity);
    s->add("withdrawalnominalcapacity", this->withdrawalNominalCapacity);

    s->add("efficiency", this->injectionEfficiency);
    s->add("efficiencyWithdrawal", this->withdrawalEfficiency);
    s->add("initialleveloptim", this->initialLevelOptim);
    s->add("penalize-variation-injection", this->penalizeVariationInjection);
    s->add("penalize-variation-withdrawal", this->penalizeVariationWithdrawal);
    s->add("enabled", this->enabled);
}

bool Properties::validate()
{
    auto checkMandatory = [this](const std::optional<double>& prop, const std::string& label)
    {
        if (!prop.has_value())
        {
            logs.error() << "Property " << label << " is mandatory for short term storage "
                         << this->name;
            return false;
        }
        return true;
    };

    if (!checkMandatory(injectionNominalCapacity, "injectionnominalcapacity"))
    {
        return false;
    }

    if (!checkMandatory(withdrawalNominalCapacity, "withdrawalnominalcapacity"))
    {
        return false;
    }

    if (!checkMandatory(reservoirCapacity, "reservoircapacity"))
    {
        return false;
    }

    if (injectionNominalCapacity < 0)
    {
        logs.error() << "Property injectionnominalcapacity must be >= 0 "
                     << "for short term storage " << name;
        return false;
    }
    if (withdrawalNominalCapacity < 0)
    {
        logs.error() << "Property withdrawalnominalcapacity must be >= 0 "
                     << "for short term storage " << name;
        return false;
    }
    if (reservoirCapacity < 0)
    {
        logs.error() << "Property reservoircapacity must be >= 0 " << "for short term storage "
                     << name;
        return false;
    }

    if (injectionEfficiency < 0)
    {
        logs.warning() << "Property efficiency must be >= 0 " << "for short term storage " << name;
        injectionEfficiency = 0;
    }

    if (injectionEfficiency > 1)
    {
        logs.warning() << "Property efficiency must be <= 1 " << "for short term storage " << name;
        injectionEfficiency = 1;
    }

    if (withdrawalEfficiency < 0)
    {
        logs.warning() << "Property efficiencyWithdrawal must be >= 0 " << "for short term storage "
                       << name;
        withdrawalEfficiency = 0;
    }

    if (withdrawalEfficiency > 1)
    {
        logs.warning() << "Property efficiencyWithdrawal must be <= 1 " << "for short term storage "
                       << name;
        withdrawalEfficiency = 1;
    }

    if (initialLevel < 0)
    {
        initialLevel = initiallevelDefault;
        logs.warning() << "initiallevel for cluster: " << name
                       << " should be positive, value has been set to " << initialLevel;
    }

    if (initialLevel > 1)
    {
        initialLevel = initiallevelDefault;
        logs.warning() << "initiallevel for cluster: " << name
                       << " should be inferior to 1, value has been set to " << initialLevel;
    }

    return true;
}

} // namespace Antares::Data::ShortTermStorage
