/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include "../memory/memory.h"
#include "study.h"
#include "memory-usage.h"
#include "runtime.h"
#include "../../../solver/simulation/simulation.h"
#include "../../../solver/variable/adequacy-draft/all.h"
#include "../../../solver/variable/economy/all.h"
#include "../logs.h"
#include "../../../solver/simulation/economy.h"
#include "../../../solver/simulation/solver.h"

using namespace Yuni;
using namespace Antares;

namespace Antares
{
namespace Data
{
class ThreadInputMemoryUsage : public Thread::IThread
{
public:
    ThreadInputMemoryUsage(const Study& study) : pStudy(study)
    {
    }

    virtual ~ThreadInputMemoryUsage()
    {
    }

protected:
    virtual bool onStarting()
    {
        return true;
    }

    virtual bool onExecute()
    {
        Data::StudyMemoryUsage m(pStudy);
        auto end = pStudy.areas.end();
        for (auto i = pStudy.areas.begin(); i != end; ++i)
        {
            auto& area = *(i->second);
            // Thermal
            {
                auto end = area.thermal.list.end();
                for (auto i = area.thermal.list.begin(); i != end; ++i)
                {
                    auto& cluster = *(i->second);

                    if (cluster.series)
                        cluster.series->estimateMemoryUsage(m, timeSeriesThermal);

                    if (shouldAbort())
                        return false;

                    if (cluster.prepro)
                        cluster.prepro->estimateMemoryUsage(m);

                    if (shouldAbort())
                        return false;
                }
            }
            // Renewable
            {
                auto end = area.renewable.list.end();
                for (auto i = area.renewable.list.begin(); i != end; ++i)
                {
                    auto& cluster = *(i->second);

                    if (cluster.series)
                        cluster.series->estimateMemoryUsage(m, timeSeriesRenewable);

                    if (shouldAbort())
                        return false;
                }
            }
            area.estimateMemoryUsage(m);

            if (shouldAbort())
                return false;
        }
        return false;
    }

    virtual void onKill()
    {
        // do nothing
    }

private:
    const Data::Study& pStudy;
};

Yuni::Thread::IThread::Ptr Study::createThreadToEstimateInputMemoryUsage() const
{
    return new ThreadInputMemoryUsage(*this);
}

void Study::estimateMemoryUsageForInput(StudyMemoryUsage& u) const
{
    u.gatheringInformationsForInput = true;

    // Base
    u.requiredMemoryForInput = sizeof(Study);
    // Folder
    u.requiredMemoryForInput += folder.capacity();

    // Strings
    // folderInput / folderOutput / folderSettings
    // This is a really simple estimation
    u.requiredMemoryForInput += ((uint64)folderInput.capacity() + 30u) * 3u;
    // Buffer
    u.requiredMemoryForInput += (dataBuffer.chunkSize * 2u); // estimation

    // Simulation
    u.requiredMemoryForInput += simulation.memoryUsage();
    // Areas
    areas.estimateMemoryUsage(u);

    // Memory consuming estimation for random numbers containers and solver
    Solver::Simulation::ISimulation<Solver::Simulation::Economy>::estimateMemoryUsage(u);

    // Binding constraints
    bindingConstraints.estimateMemoryUsage(u);

    // Correlation
    preproLoadCorrelation.estimateMemoryUsage(u);
    preproSolarCorrelation.estimateMemoryUsage(u);
    preproWindCorrelation.estimateMemoryUsage(u);
    preproHydroCorrelation.estimateMemoryUsage(u);

    // Runtime infos
    StudyRuntimeInfosEstimateMemoryUsage(u);

    // Tables allocation for simulation. See : sim_allocation_tableaux.cpp.
    if (not parameters.adequacyDraft())
    {
        const size_t sizeOfLongHours = HOURS_PER_YEAR * sizeof(int);
        const size_t sizeOfDoubleHours = HOURS_PER_YEAR * sizeof(double);
        uint nbLinks = areas.areaLinkCount();
        for (uint i = 0; i != nbLinks; i++)
        {
            u.requiredMemoryForInput += 7 * sizeOfDoubleHours;
            u.requiredMemoryForInput += 2 * sizeOfLongHours;
            u.requiredMemoryForInput += parameters.nbYears * sizeof(double);
        }
        for (uint i = 0; i != bindingConstraints.size(); ++i)
            u.requiredMemoryForInput += sizeOfDoubleHours;
    }

    dataBuffer.clear();
    dataBuffer.shrink();
}

void Study::estimateMemoryUsageForOutput(StudyMemoryUsage& u) const
{
    u.gatheringInformationsForInput = false;
    if (!u.years)
        return;

    // Average 2Mo per area - Used by the simplex
    // u.requiredMemoryForOutput += (2u * areas.size()) * 1024u * 1024u;

    switch (u.mode)
    {
    case stdmEconomy:
    {
        enum
        {
            maxColumnsForExportation = Solver::Variable::Container::BrowseAllVariables<
              Solver::Variable::Economy::AllVariables>::maxValue,
        };

        Solver::Variable::SurveyResults::EstimateMemoryUsage(maxColumnsForExportation, u);
        Solver::Variable::Economy::AllVariables::EstimateMemoryUsage(u);
        return;
    }
    case stdmAdequacy:
    {
        enum
        {
            maxColumnsForExportation = Solver::Variable::Container::BrowseAllVariables<
              Solver::Variable::Economy::AllVariables>::maxValue,
        };

        Solver::Variable::SurveyResults::EstimateMemoryUsage(maxColumnsForExportation, u);
        Solver::Variable::Economy::AllVariables::EstimateMemoryUsage(u);
        return;
    }
    case stdmAdequacyDraft:
    {
        // Survey results
        enum
        {
            maxColumnsForExportation = Solver::Variable::Container::BrowseAllVariables<
              Solver::Variable::AdequacyDraft::AllVariables>::maxValue,
        };

        Solver::Variable::SurveyResults::EstimateMemoryUsage(maxColumnsForExportation, u);
        Solver::Variable::AdequacyDraft::AllVariables::EstimateMemoryUsage(u);
        return;
    }
    case stdmUnknown:
    case stdmExpansion:
    case stdmMax:
        return;
    }
}

} // namespace Data
} // namespace Antares
