//
// Created by marechaljas on 11/05/23.
//

#include "BindingConstraintLoader.h"
#include <memory>
#include <vector>
#include "BindingConstraint.h"
#include "yuni/core/string/string.h"
#include "antares/study/version.h"

namespace Antares::Data {
using namespace Yuni;

std::vector<std::shared_ptr<BindingConstraint>>
BindingConstraintLoader::load(EnvForLoading env) {
    auto bc = std::make_shared<BindingConstraint>();
    bc->clear();

    // Foreach property in the section...
    for (const IniFile::Property *p = env.section->firstProperty; p; p = p->next) {
        if (p->key.empty())
            continue;

        if (p->key == "name") {
            bc->pName = p->value;
            continue;
        }
        if (p->key == "id") {
            bc->pID = p->value;
            bc->pID.toLower(); // force the lowercase
            continue;
        }
        if (p->key == "enabled") {
            bc->pEnabled = p->value.to<bool>();
            continue;
        }
        if (p->key == "type") {
            bc->pType = BindingConstraint::StringToType(p->value);
            continue;
        }
        if (p->key == "operator") {
            bc->pOperator = BindingConstraint::StringToOperator(p->value);
            continue;
        }
        if (p->key == "filter-year-by-year") {
            bc->pFilterYearByYear = stringIntoDatePrecision(p->value);
            continue;
        }
        if (p->key == "filter-synthesis") {
            bc->pFilterSynthesis = stringIntoDatePrecision(p->value);
            continue;
        }
        if (p->key == "comments") {
            bc->pComments = p->value;
            continue;
        }
        if (p->key == "group") {
            bc->group_ = p->value.c_str();
            continue;
        }

        // initialize the values
        double w = .0;
        int o = 0;

        // Separate the value
        if (auto setKey = p->key.find('%'); setKey != 0 && setKey != String::npos) // It is a link
        {

            if (bool ret = SeparateValue(env, p, w, o); !ret)
                continue;

            const AreaLink *lnk = env.areaList.findLinkFromINIKey(p->key);
            if (!lnk) {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: link not found";
                continue;
            }
            if (!Math::Zero(w))
                bc->weight(lnk, w);

            if (!Math::Zero(o))
                bc->offset(lnk, o);

            continue;
        } else // It must be a cluster
        {
            // Separate the key
            setKey = p->key.find('.');
            if (0 == setKey || setKey == String::npos) {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: invalid key";
                continue;
            }


            if (bool ret = SeparateValue(env, p, w, o); !ret)
                continue;

            const ThermalCluster *clstr = env.areaList.findClusterFromINIKey(p->key);
            if (!clstr) {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: cluster not found";
                continue;
            }
            if (!Math::Zero(w))
                bc->weight(clstr, w);

            if (!Math::Zero(o))
                bc->offset(clstr, o);

            continue;
        }
    }

    // Checking for validity
    if (!bc->pName || !bc->pID || bc->pOperator == BindingConstraint::opUnknown ||
        bc->pType == BindingConstraint::typeUnknown) {
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
            if (env.version >= version870) {
                logs.error() << env.iniFilename << ": in [" << env.section->name
                             << "]: Missing binding constraint group";
            } else {
                bc->group_ = "legacy_study_group";
            }
        }

        // Invalid binding constraint
        return {};
    }

    // The binding constraint can not be enabled if there is no weight in the table
    if (bc->pLinkWeights.empty() && bc->pClusterWeights.empty())
        bc->pEnabled = false;

    if (bc->operatorType() == BindingConstraint::opBoth) {
        auto greater_bc = std::make_shared<BindingConstraint>();
        greater_bc->copyFrom(bc.get());
        greater_bc->name(bc->name());
        greater_bc->pID = bc->pID;
        bc->operatorType(BindingConstraint::opLess);
        greater_bc->operatorType(BindingConstraint::opGreater);
        if (loadTimeSeries(env, bc.get()) && loadTimeSeries(env, greater_bc.get())) {
            return {bc, greater_bc};
    }
    return {};
}

    if (loadTimeSeries(env, bc.get())) {
        return {bc};
    }
    return {};
}

bool
BindingConstraintLoader::SeparateValue(const EnvForLoading &env, const IniFile::Property *p, double &w, int &o) {
    bool ret = true;
    CString<64> stringWO = p->value;
    String::Size setVal = p->value.find('%');
    uint occurrence = 0;
    stringWO.words("%", [&occurrence, &setVal, &env, &ret, &p, &w, &o](const CString<64> &part) {
        if (occurrence == 0) {
            if (setVal == 0) // weight is null
            {
                if (!part.to<int>(o)) {
                    logs.error() << env.iniFilename << ": in [" << env.section->name
                                 << "]: `" << p->key << "`: invalid offset";
                    ret = false;
                }
            } else // weight is not null
            {
                if (!part.to<double>(w)) {
                    logs.error() << env.iniFilename << ": in [" << env.section->name
                                 << "]: `" << p->key << "`: invalid weight";
                    ret = false;
                }
            }
        }

        if (occurrence == 1 && setVal != 0 && !part.to<int>(o)) {
            logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                         << p->key << "`: invalid offset";
            ret = false;
        }

        ++occurrence;
        return ret; // continue to iterate
    });
    return ret;
}

bool BindingConstraintLoader::loadTimeSeries(EnvForLoading &env, BindingConstraint *bindingConstraint)
{
    if (env.version >= version870)
        return loadTimeSeries(env, bindingConstraint->operatorType(), bindingConstraint);

    return loadTimeSeriesLegacyStudies(env, bindingConstraint);
}

bool
BindingConstraintLoader::loadTimeSeries(EnvForLoading &env, BindingConstraint::Operator operatorType,
                                        BindingConstraint *bindingConstraint) const {
    env.buffer.clear() << bindingConstraint->timeSeriesFileName(env);
    bool load_ok = bindingConstraint->RHSTimeSeries_.loadFromCSVFile(env.buffer,
                                                                     1,
                                          (bindingConstraint->type() == BindingConstraint::typeHourly) ? 8784 : 366,
                                                                     Matrix<>::optImmediate,
                                                                     &env.matrixBuffer);
    if (load_ok)
    {
        logs.info() << " loaded time series for `" << bindingConstraint->name() << "` (" << BindingConstraint::TypeToCString(bindingConstraint->type()) << ", "
                    << BindingConstraint::OperatorToShortCString(operatorType) << ')';
        return true;
    } else {
        logs.error() << " unable to load time series for `" << bindingConstraint->name() << "` (" << BindingConstraint::TypeToCString(bindingConstraint->type()) << ", "
                     << BindingConstraint::OperatorToShortCString(operatorType) << ')';
        return false;
    }
}

bool BindingConstraintLoader::loadTimeSeriesLegacyStudies(EnvForLoading &env, BindingConstraint *bindingConstraint) const {
    env.buffer.clear() << env.folder << IO::Separator << bindingConstraint->pID << ".txt";
    Matrix<> intermediate;
    const int height = (bindingConstraint->pType == BindingConstraint::typeHourly) ? 8784 : 366;
    if (intermediate.loadFromCSVFile(env.buffer,
                                     BindingConstraint::columnMax,
                                     height,
                                     Matrix<>::optImmediate | Matrix<>::optFixedSize,
                                     &env.matrixBuffer))
    {
        if (bindingConstraint->pComments.empty())
            logs.info() << " added `" << bindingConstraint->pName << "` (" << BindingConstraint::TypeToCString(bindingConstraint->pType) << ", "
                        << BindingConstraint::OperatorToShortCString(bindingConstraint->pOperator) << ')';
        else
            logs.info() << " added `" << bindingConstraint->pName << "` (" << BindingConstraint::TypeToCString(bindingConstraint->pType) << ", "
                        << BindingConstraint::OperatorToShortCString(bindingConstraint->pOperator) << ") " << bindingConstraint->pComments;

        // 0 is BindingConstraint::opLess
        int columnNumber;
        if (bindingConstraint->operatorType() == BindingConstraint::opLess)
            columnNumber = BindingConstraint::Column::columnInferior;
        else if (bindingConstraint->operatorType() == BindingConstraint::opGreater)
            columnNumber = BindingConstraint::Column::columnSuperior;
        else if (bindingConstraint->operatorType() == BindingConstraint::opEquality)
            columnNumber = BindingConstraint::Column::columnEquality;
        else
            logs.error("Cannot load time series of type other that eq/gt/lt");

        bindingConstraint->RHSTimeSeries_.resize(1, height);
        bindingConstraint->RHSTimeSeries_.pasteToColumn(0, intermediate[columnNumber]);
        return true;
    }

    return false;
}
} // Data
