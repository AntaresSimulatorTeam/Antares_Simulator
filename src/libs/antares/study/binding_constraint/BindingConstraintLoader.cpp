// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 11/05/23.
//

#include "antares/study/binding_constraint/BindingConstraintLoader.h"

#include <memory>
#include <vector>

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/utils/utils.h>
#include "antares/study/binding_constraint/BindingConstraint.h"
#include "antares/study/version.h"

namespace Antares::Data
{

std::vector<std::shared_ptr<BindingConstraint>> BindingConstraintLoader::load(EnvForLoading env)
{
    auto bc = std::make_shared<BindingConstraint>();
    bc->clear();

    populateConstraint(env, bc);

    // Checking for validity
    if (!validate(env, bc))
    {
        return {};
    }

    // The binding constraint can not be enabled if there is no weight in the table
    if (bc->pLinkWeights.empty() && bc->pClusterWeights.empty())
    {
        bc->pEnabled = false;
    }

    if (!bc->pEnabled)
    {
        /// This BC won't be used, return it without loading time series
        logs.debug() << "BC " << bc->name() << " is disabled, skipping time series loading";
        return {bc};
    }
    return loadByOperator(env, bc);
}

bool BindingConstraintLoader::SeparateValue(const EnvForLoading& env,
                                            const IniFile::Property* p,
                                            double& w,
                                            int& o)
{
    bool ret = true;
    const std::string value = std::string(p->value);
    const std::size_t setVal = value.find('%');
    unsigned int occurrence = 0;

    std::size_t pos = 0;
    while (pos <= value.size())
    {
        const std::size_t end = value.find('%', pos);
        const std::string part = value.substr(pos,
                                              end == std::string::npos ? std::string::npos
                                                                       : end - pos);
        if (occurrence == 0)
        {
            if (setVal == 0) // weight is null
            {
                if (!Antares::stringToInt(part, o))
                {
                    logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                                 << p->key << "`: invalid offset";
                    ret = false;
                }
            }
            else // weight is not null
            {
                if (!Antares::stringToDouble(part, w))
                {
                    logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                                 << p->key << "`: invalid weight";
                    ret = false;
                }
            }
        }

        if (occurrence == 1 && setVal != 0 && !Antares::stringToInt(part, o))
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                         << "`: invalid offset";
            ret = false;
        }

        ++occurrence;
        if (end == std::string::npos)
        {
            break;
        }
        pos = end + 1;
    }
    return ret;
}

bool BindingConstraintLoader::loadTimeSeries(EnvForLoading& env,
                                             BindingConstraint* bindingConstraint)
{
    if (env.version >= StudyVersion(8, 7))
    {
        return loadTimeSeries(env, bindingConstraint->operatorType(), bindingConstraint);
    }

    return loadTimeSeriesLegacyStudies(env, bindingConstraint);
}

bool BindingConstraintLoader::loadTimeSeries(EnvForLoading& env,
                                             BindingConstraint::Operator operatorType,
                                             BindingConstraint* bindingConstraint) const
{
    env.buffer = bindingConstraint->timeSeriesFileName(env);
    bool load_ok = bindingConstraint->RHSTimeSeries_.loadFromCSVFile(
      env.buffer,
      1,
      (bindingConstraint->type() == BindingConstraint::typeHourly) ? 8784 : 366,
      Matrix<>::optNone,
      &env.matrixBuffer);
    if (load_ok)
    {
        logs.info() << " loaded time series for `" << bindingConstraint->name() << "` ("
                    << BindingConstraint::TypeToCString(bindingConstraint->type()) << ", "
                    << BindingConstraint::OperatorToShortCString(operatorType) << ')';
        return true;
    }
    else
    {
        logs.error() << " unable to load time series for `" << bindingConstraint->name() << "` ("
                     << BindingConstraint::TypeToCString(bindingConstraint->type()) << ", "
                     << BindingConstraint::OperatorToShortCString(operatorType) << ')';
        return false;
    }
}

bool BindingConstraintLoader::loadTimeSeriesLegacyStudies(
  EnvForLoading& env,
  BindingConstraint* bindingConstraint) const
{
    std::filesystem::path path = env.folder / (bindingConstraint->pID + ".txt");
    Matrix<> intermediate;
    const int height = (bindingConstraint->pType == BindingConstraint::typeHourly) ? 8784 : 366;
    if (intermediate.loadFromCSVFile(path.string(),
                                     BindingConstraint::columnMax,
                                     height,
                                     Matrix<>::optFixedSize,
                                     &env.matrixBuffer))
    {
        logs.info() << " added `" << bindingConstraint->pName << "` ("
                    << BindingConstraint::TypeToCString(bindingConstraint->pType) << ", "
                    << BindingConstraint::OperatorToShortCString(bindingConstraint->pOperator)
                    << ')';

        // 0 is BindingConstraint::opLess
        int columnNumber;
        if (bindingConstraint->operatorType() == BindingConstraint::opLess)
        {
            columnNumber = BindingConstraint::Column::columnInferior;
        }
        else if (bindingConstraint->operatorType() == BindingConstraint::opGreater)
        {
            columnNumber = BindingConstraint::Column::columnSuperior;
        }
        else if (bindingConstraint->operatorType() == BindingConstraint::opEquality)
        {
            columnNumber = BindingConstraint::Column::columnEquality;
        }
        else
        {
            logs.error("Cannot load time series of type other that eq/gt/lt");
            return false;
        }

        bindingConstraint->RHSTimeSeries_.resize(1, height);
        bindingConstraint->RHSTimeSeries_.pasteToColumn(0, intermediate[columnNumber]);
        return true;
    }

    return false;
}

void BindingConstraintLoader::populateConstraint(const EnvForLoading& env,
                                                 std::shared_ptr<BindingConstraint>& bc)
{
    // Foreach property in the section...
    for (const IniFile::Property* p = env.section->firstProperty; p; p = p->next)
    {
        if (p->key.empty())
        {
            continue;
        }

        if (p->key == "name")
        {
            bc->pName = std::string(p->value);
            continue;
        }
        if (p->key == "id")
        {
            bc->pID = std::string(p->value);
            boost::to_lower(bc->pID);
            continue;
        }
        if (p->key == "enabled")
        {
            bc->pEnabled = Antares::stringToBool(std::string(p->value));
            continue;
        }
        if (p->key == "type")
        {
            bc->pType = BindingConstraint::StringToType(std::string(p->value));
            continue;
        }
        if (p->key == "operator")
        {
            bc->pOperator = BindingConstraint::StringToOperator(std::string(p->value));
            continue;
        }
        if (p->key == "filter-year-by-year")
        {
            bc->pFilterYearByYear = stringIntoDatePrecision(std::string(p->value));
            continue;
        }
        if (p->key == "filter-synthesis")
        {
            bc->pFilterSynthesis = stringIntoDatePrecision(std::string(p->value));
            continue;
        }
        // Metadata, don't read
        // Kept for compatibility with existing studies
        if (p->key == "comments")
        {
            continue;
        }
        if (p->key == "group")
        {
            bc->group_ = std::string(p->value);
            continue;
        }
        parseWeightAndOffset(env, p, bc);
    }
}

void BindingConstraintLoader::parseWeightAndOffset(const EnvForLoading& env,
                                                   const IniFile::Property* p,
                                                   std::shared_ptr<BindingConstraint>& bc)
{
    // initialize the values
    double w = .0;
    int o = 0;

    const std::string key = std::string(p->key);
    // Separate the value
    if (auto setKey = key.find('%'); setKey != 0 && setKey != std::string::npos) // It is a link
    {
        if (bool ret = SeparateValue(env, p, w, o); !ret)
        {
            return;
        }

        const AreaLink* lnk = env.areaList.findLinkFromINIKey(key);
        if (!lnk)
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << key
                         << "`: link not found";
            return;
        }
        if (!Utils::isZero(w))
        {
            bc->weight(lnk, w);
        }

        if (!Utils::isZero(o))
        {
            bc->offset(lnk, o);
        }

        return;
    }
    else // It must be a cluster
    {
        // Separate the key
        setKey = key.find('.');
        if (0 == setKey || setKey == std::string::npos)
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << key
                         << "`: invalid key";
            return;
        }

        if (bool ret = SeparateValue(env, p, w, o); !ret)
        {
            return;
        }

        const ThermalCluster* clstr = env.areaList.findClusterFromINIKey(key);
        if (!clstr)
        {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << key
                         << "`: cluster not found";
            return;
        }
        if (!Utils::isZero(w))
        {
            bc->weight(clstr, w);
        }

        if (!Utils::isZero(o))
        {
            bc->offset(clstr, o);
        }

        return;
    }
}

bool BindingConstraintLoader::validate(const EnvForLoading& env,
                                       const std::shared_ptr<BindingConstraint>& bc)
{
    if (bc->pName.empty())
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid binding constraint name";
        return false;
    }
    if (bc->pID.empty())
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid binding constraint id";
        return false;
    }
    if (bc->pType == bc->typeUnknown)
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid type [hourly,daily,weekly]";
        return false;
    }
    if (bc->pOperator == BindingConstraint::opUnknown)
    {
        logs.error() << env.iniFilename << ": in [" << env.section->name
                     << "]: Invalid operator [less,greater,equal,both]";
        return false;
    }
    return true;
}

std::vector<std::shared_ptr<BindingConstraint>> BindingConstraintLoader::loadByOperator(
  EnvForLoading& env,
  std::shared_ptr<BindingConstraint>& bc)
{
    switch (bc->operatorType())
    {
    case BindingConstraint::opLess:
    case BindingConstraint::opEquality:
    case BindingConstraint::opGreater:
    {
        if (loadTimeSeries(env, bc.get()))
        {
            return {bc};
        }
        break;
    }
    case BindingConstraint::opBoth:
    {
        auto greaterBc = std::make_shared<BindingConstraint>();
        const auto originalName = bc->name();
        greaterBc->copyFrom(bc.get());
        greaterBc->name(originalName + "_sup");
        greaterBc->pID = bc->pID;
        greaterBc->operatorType(BindingConstraint::opGreater);
        bc->name(originalName + "_inf");
        bc->operatorType(BindingConstraint::opLess);

        if (loadTimeSeries(env, bc.get()) && loadTimeSeries(env, greaterBc.get()))
        {
            return {bc, greaterBc};
        }
        break;
    }
    default:
    {
        logs.error() << "Wrong binding constraint operator type for constraint " << bc->name();
        return {};
    }
    }
    return {};
}

} // namespace Antares::Data
