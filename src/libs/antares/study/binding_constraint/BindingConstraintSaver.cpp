//
// Created by marechaljas on 16/05/23.
//
#include "BindingConstraintSaver.h"
#include "BindingConstraintTimeSeries.h"
#include "BindingConstraint.h"
#include <yuni/yuni.h>
#include "antares/study/area/area.h"
#include "antares/study/fwd.h"

namespace Antares::Data {

using namespace Yuni;
bool BindingConstraintSaver::saveToEnv(EnvForSaving& env, BindingConstraint *const bindingConstraint) const
{
    env.section->add("name", bindingConstraint->pName);
    env.section->add("id", bindingConstraint->pID);
    env.section->add("enabled", bindingConstraint->pEnabled);
    env.section->add("type", BindingConstraint::TypeToCString(bindingConstraint->pType));
    env.section->add("operator", BindingConstraint::OperatorToCString(bindingConstraint->pOperator));
    env.section->add("filter-year-by-year", datePrecisionIntoString(bindingConstraint->pFilterYearByYear));
    env.section->add("filter-synthesis", datePrecisionIntoString(bindingConstraint->pFilterSynthesis));

    if (not bindingConstraint->pComments.empty())
        env.section->add("comments", bindingConstraint->pComments);

    if (not bindingConstraint->pLinkWeights.empty())
    {
        auto end = bindingConstraint->pLinkWeights.end();
        for (auto i = bindingConstraint->pLinkWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid link");
            assert(i->first->from and "Invalid area name");
            assert(i->first->with and "Invalid area name");

            const AreaLink &lnk = *(i->first);
            env.key.clear() << lnk.from->id << '%' << lnk.with->id;
            String value;
            value << i->second;
            if (bindingConstraint->pLinkOffsets.find(i->first) != bindingConstraint->pLinkOffsets.end())
                value << '%' << bindingConstraint->pLinkOffsets[i->first];
            // env.section->add(env.key, i->second);
            env.section->add(env.key, value);
        }
    }

    if (not bindingConstraint->pClusterWeights.empty())
    {
        auto end = bindingConstraint->pClusterWeights.end();
        for (auto i = bindingConstraint->pClusterWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid thermal cluster");

            const ThermalCluster &clstr = *(i->first);
            env.key.clear() << clstr.getFullName();
            String value;
            value << i->second;
            if (bindingConstraint->pClusterOffsets.find(i->first) != bindingConstraint->pClusterOffsets.end())
                value << '%' << bindingConstraint->pClusterOffsets[i->first];
            // env.section->add(env.key, i->second);
            env.section->add(env.key, value);
        }
    }

    // Exporting the matrix
    env.matrixFilename.clear() << env.folder << IO::Separator << bindingConstraint->pID << ".txt";
    return bindingConstraint->time_series.saveToCSVFile(env.matrixFilename.c_str());
}
}