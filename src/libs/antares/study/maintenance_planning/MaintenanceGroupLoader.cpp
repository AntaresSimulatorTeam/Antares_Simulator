//
// Created by milos on 1/11/23.
//

#include "MaintenanceGroupLoader.h"
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include "MaintenanceGroup.h"
#include "yuni/core/string/string.h"
#include "antares/study/version.h"

namespace Antares::Data
{
using namespace Yuni;

std::vector<std::shared_ptr<MaintenanceGroup>> MaintenanceGroupLoader::load(EnvForLoading env)
{
    auto mnt = std::make_shared<MaintenanceGroup>();
    mnt->clear();

    // Foreach property in the section...
    for (const IniFile::Property* p = env.section->firstProperty; p; p = p->next)
    {
        if (p->key.empty())
            continue;

        if (p->key == "name")
        {
            mnt->name_ = p->value;
            continue;
        }
        if (p->key == "id")
        {
            mnt->ID_ = toLower(p->value);
            continue;
        }
        if (p->key == "enabled")
        {
            mnt->enabled_ = p->value.to<bool>();
            continue;
        }
        if (p->key == "residual-load-definition")
        {
            mnt->type_ = MaintenanceGroup::StringToResidualLoadDefinitionType(p->value);
            continue;
        }

        // Check if key is valid
        std::string setKey = p->key.to<std::string>();
        if (!((setKey.find(".weight-load") != std::string::npos)
              || (setKey.find(".weight-renewable") != std::string::npos)
              || (setKey.find(".weight-ror") != std::string::npos)))
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                         << "`: invalid key";
            continue;
        }

        // Separate the key
        const Area* area = env.areaList.findAreaFromINIKey(p->key);
        if (!area)
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                         << "`: area not found";
            continue;
        }

        // Initialize the value
        double val = 1;
        if (bool ret = CheckValue(env, p, val); !ret)
            continue;

        // check is it load, renewable or ror.
        auto offset = setKey.find("."); // It must be found because we already checked the validity of the key
        std::string weightName(setKey.c_str() + offset + 1, setKey.size() - (offset + 1));
        if (weightName == "weight-load")
            mnt->loadWeight(area, val);
        else if (weightName == "weight-renewable")
            mnt->renewableWeight(area, val);
        else if (weightName == "weight-ror")
            mnt->rorWeight(area, val);
        else
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                         << "`: invalid key";
            continue;
        }
    }

    // Checking for validity
    if (mnt->name_.empty())
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid maintenance group name";
        return {};
    }
    if (mnt->ID_.empty())
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid maintenance group id";
        return {};
    }
    if (mnt->type_ == mnt->typeUnknown)
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid Residual Load Definition Type";
        return {};
    }

    return {mnt};
}

bool MaintenanceGroupLoader::CheckValue(const EnvForLoading& env,
                                        const IniFile::Property* p,
                                        double& w)
{
    // Convert to a double and report an error if conversion fails
    auto stringValue = p->value.to<std::string>();
    char* endptr;
    strtod(stringValue.c_str(), &endptr);
    if (!(*endptr == '\0'))
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                     << "`: weight cannot be converted to number";
        return false;
    }

    // check if number is between 0 and 1.0
    double val = p->value.to<double>();
    if (!(val >= 0.0 && val <= 1.0))
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                     << "`: weight is not in the [0, 1] range";
        return false;
    }

    // Output the parsed numbers
    w = val;

    return true;
}

std::string MaintenanceGroupLoader::toLower(const std::string& str)
{
    std::string result = str; // Create a copy of the input string

    // Use std::transform to convert each character to lowercase
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return result;
}

} // namespace Antares::Data
