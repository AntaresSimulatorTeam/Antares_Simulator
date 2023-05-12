//
// Created by marechaljas on 11/05/23.
//

#include "BindingConstraintLoader.h"
#include <vector>
#include "BindingConstraint.h"
#include "yuni/core/string/string.h"
#include "antares/study/version.h"

namespace Antares::Data {
using namespace Yuni;

std::vector<BindingConstraint *>
BindingConstraintLoader::load(EnvForLoading env, unsigned int years) const {
    BindingConstraint* bc = new BindingConstraint;
    bc->clear();

    // Foreach property in the section...
    for (const IniFile::Property* p = env.section->firstProperty; p; p = p->next)
    {
        if (p->key.empty())
            continue;

        if (p->key == "name")
        {
            bc->pName = p->value;
            continue;
        }
        if (p->key == "id")
        {
            bc->pID = p->value;
            bc->pID.toLower(); // force the lowercase
            continue;
        }
        if (p->key == "enabled")
        {
            bc->pEnabled = p->value.to<bool>();
            continue;
        }
        if (p->key == "type")
        {
            bc->pType = BindingConstraint::StringToType(p->value);
            continue;
        }
        if (p->key == "operator")
        {
            bc->pOperator = BindingConstraint::StringToOperator(p->value);
            continue;
        }
        if (p->key == "filter-year-by-year")
        {
            bc->pFilterYearByYear = stringIntoDatePrecision(p->value);
            continue;
        }
        if (p->key == "filter-synthesis")
        {
            bc->pFilterSynthesis = stringIntoDatePrecision(p->value);
            continue;
        }
        if (p->key == "comments")
        {
            bc->pComments = p->value;
            continue;
        }
        if (p->key == "group")
        {
            bc->group_ = p->value.c_str();
            continue;
        }

        // It may be a link
        // Separate the key
        String::Size setKey = p->key.find('%');

        // initialize the values
        double w = .0;
        int o = 0;

        // Separate the value
        if (setKey != 0 && setKey != String::npos) // It is a link
        {
            CString<64> stringWO = p->value;
            String::Size setVal = p->value.find('%');
            uint occurence = 0;
            bool ret = true;
            stringWO.words("%", [&](const CString<64> &part) -> bool {
                if (occurence == 0)
                {
                    if (setVal == 0) // weight is null
                    {
                        if (not part.to<int>(o))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid offset";
                            ret = false;
                        }
                    }
                    else // weight is not null
                    {
                        if (not part.to<double>(w))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid weight";
                            ret = false;
                        }
                    }
                }

                if (occurence == 1 && setVal != 0)
                {
                    if (not part.to<int>(o))
                    {
                        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                                     << p->key << "`: invalid offset";
                        ret = false;
                    }
                }

                ++occurence;
                return ret; // continue to iterate
            });

            if (not ret)
                continue;

            const AreaLink *lnk = env.areaList.findLinkFromINIKey(p->key);
            if (!lnk)
            {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: link not found";
                continue;
            }
            if (not Math::Zero(w))
                bc->weight(lnk, w);

            if (not Math::Zero(o))
                bc->offset(lnk, o);

            continue;
        }
        else // It must be a cluster
        {
            // Separate the key
            String::Size setKey = p->key.find('.');
            if (0 == setKey or setKey == String::npos)
            {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: invalid key";
                continue;
            }

            CString<64> stringWO = p->value;
            String::Size setVal = p->value.find('%');
            uint occurence = 0;
            bool ret = true;
            stringWO.words("%", [&](const CString<64> &part) -> bool {
                if (occurence == 0)
                {
                    if (setVal == 0) // weight is null
                    {
                        if (not part.to<int>(o))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid offset";
                            ret = false;
                        }
                    }
                    else // weight is not null
                    {
                        if (not part.to<double>(w))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid weight";
                            ret = false;
                        }
                    }
                }

                if (occurence == 1 && setVal != 0)
                {
                    if (not part.to<int>(o))
                    {
                        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                                     << p->key << "`: invalid offset";
                        ret = false;
                    }
                }

                ++occurence;
                return ret; // continue to iterate
            });

            if (not ret)
                continue;

            const ThermalCluster *clstr = env.areaList.findClusterFromINIKey(p->key);
            if (!clstr)
            {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: cluster not found";
                continue;
            }
            if (not Math::Zero(w))
                bc->weight(clstr, w);

            if (not Math::Zero(o))
                bc->offset(clstr, o);

            continue;
        }
    }

    // Checking for validity
    if (!bc->pName || !bc->pID || bc->pOperator == BindingConstraint::opUnknown || bc->pType == BindingConstraint::typeUnknown) {
        // Reporting the error into the logs
        if (!bc->pName)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid binding constraint name";
        if (!bc->pID)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid binding constraint id";
        if (bc->pType == bc->typeUnknown)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid type [hourly,daily,weekly]";
        if (bc->pOperator == BindingConstraint::opUnknown)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid operator [less,greater,equal,both]";
        if (bc->group_.empty()) {
            if (env.version >= version860) {
                logs.error() << env.iniFilename << ": in [" << env.section->name
                             << "]: Missing binding constraint group";
            } else {
                bc->group_ = std::string() + bc->name().c_str() + "_" + bc->id().c_str();
            }
        }

        // Invalid binding constraint
        delete bc;
        return {};
    }

    // The binding constraint can not be enabled if there is no weight in the table
    if (bc->pLinkWeights.empty() && bc->pClusterWeights.empty())
        bc->pEnabled = false;

    if (bc->operatorType() == BindingConstraint::opBoth) {
        BindingConstraint* greater_bc = new BindingConstraint;
        greater_bc->copyFrom(bc);
        bc->operatorType(BindingConstraint::opLess);
        greater_bc->operatorType(BindingConstraint::opGreater);
        if (bc->loadTimeSeries(env) && greater_bc->loadTimeSeries(env)) {
            return {bc, greater_bc};
        }
        delete bc;
        delete greater_bc;
        return {};
    }

    if (bc->loadTimeSeries(env)) {
        return {bc};
    }
    delete bc;
    return {};
}
} // Data