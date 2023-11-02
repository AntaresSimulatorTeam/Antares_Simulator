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

        // initialize the values
        MaintenanceGroup::Weights w = {0.0, 0.0, 0.0};

        // Separate the key
        auto setKey = p->key.find('.');
        if (0 == setKey || setKey == String::npos)
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                         << "`: invalid key";
            continue;
        }

        if (bool ret = SeparateValue(env, p, w); !ret)
            continue;

        const Area* area = env.areaList.findAreaFromINIKey(p->key);
        if (!area)
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                         << "`: area not found";
            continue;
        }

        mnt->loadWeight(area, w.load);
        mnt->renewableWeight(area, w.renewable);
        mnt->rorWeight(area, w.ror);
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

bool MaintenanceGroupLoader::SeparateValue(const EnvForLoading& env,
                                           const IniFile::Property* p,
                                           MaintenanceGroup::Weights& w)
{
    CString<64> string = p->value;
    string.trim(); // trim
    string.to<std::string>();

    // Split the string into parts based on '%'
    std::vector<std::string> parts;
    std::istringstream ss(string);
    std::string part;
    while (std::getline(ss, part, '%'))
    {
        parts.push_back(part);
    }

    // Check if there are exactly two parts
    if (parts.size() != 3)
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                     << "`: invalid number of weights";
        return false;
    }

    // Convert each part to a double and report an error if conversion fails
    double load, rnw, ror;
    if (!(std::istringstream(parts[0]) >> load) || !(std::istringstream(parts[1]) >> rnw)
        || !(std::istringstream(parts[2]) >> ror))
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                     << "`: one or more weights cannot be converted to numbers";
        return false;
    }

    // check if parsed numbers are between 0 and 1.0
    if (!(load >= 0.0 && load <= 1.0 && rnw >= 0.0 && rnw <= 1.0 && ror >= 0.0 && ror <= 1.0))
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                     << "`: one or more weights are not in the [0, 1] range";
        return false;
    }

    // Output the parsed numbers
    w.load = load;
    w.renewable = rnw;
    w.ror = ror;

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
