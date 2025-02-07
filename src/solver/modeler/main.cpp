/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include <antares/solver/modeler/parameters/parseModelerParameters.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include "antares/optimisation/linear-problem-api/linearProblem.h"

using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares;
using namespace Antares::Solver;
using namespace Antares::Optimisation::LinearProblemApi;

class SystemLinearProblem
{
public:
    explicit SystemLinearProblem(const Study::SystemModel::System& system):
        system_(system)
    {
    }

    ~SystemLinearProblem() = default;

    void Provide(ILinearProblem& pb, const ModelerParameters& parameters)
    {
        std::vector<std::unique_ptr<Optimization::ComponentFiller>> fillers;
        std::vector<LinearProblemFiller*> fillers_ptr;
        for (const auto& [_, component]: system_.Components())
        {
            auto cf = std::make_unique<Optimization::ComponentFiller>(component);
            fillers.push_back(std::move(cf));
        }
        for (auto& component_filler: fillers)
        {
            fillers_ptr.push_back(component_filler.get());
        }

        LinearProblemBuilder linear_problem_builder(fillers_ptr);
        Optimisation::LinearProblemDataImpl::LinearProblemData dummy_data;
        FillContext dummy_time_scenario_ctx = {parameters.firstTimeStep, parameters.lastTimeStep};
        linear_problem_builder.build(pb, dummy_data, dummy_time_scenario_ctx);
    }

private:
    const Study::SystemModel::System& system_;
};

static void usage()
{
    std::cout << "Usage:\n"
              << "antares-modeler <path/to/study>\n";
}

int main(int argc, const char** argv)
{
    logs.applicationName("modeler");
    if (argc <= 1)
    {
        logs.error() << "No study path provided, exiting.";
        usage();
        return EXIT_FAILURE;
    }

    std::filesystem::path studyPath(argv[1]);
    logs.info() << "Study path: " << studyPath;

    if (!std::filesystem::is_directory(studyPath))
    {
        logs.error() << "The path provided isn't a valid directory, exiting";
        return EXIT_FAILURE;
    }

    try
    {
        const auto parameters = LoadFiles::loadParameters(studyPath);
        logs.info() << "Parameters loaded";
        const auto libraries = LoadFiles::loadLibraries(studyPath);
        logs.info() << "Libraries loaded";
        const auto system = LoadFiles::loadSystem(studyPath, libraries);
        logs.info() << "System loaded";
        SystemLinearProblem system_linear_problem(system);

        auto outputPath = studyPath / "output";
        if (!parameters.noOutput)
        {
            logs.info() << "Output folder : " << outputPath;
            if (!std::filesystem::is_directory(outputPath)
                && !std::filesystem::create_directory(outputPath))
            {
                logs.error() << "Failed to create output directory. Exiting simulation.";
                return EXIT_FAILURE;
            }
        }

        logs.info() << "linear problem of System loaded";
        OrtoolsLinearProblem ortools_linear_problem(true, parameters.solver);

        system_linear_problem.Provide(ortools_linear_problem, parameters);

        logs.info() << "Linear problem provided";

        logs.info() << "Number of variables: " << ortools_linear_problem.variableCount();
        logs.info() << "Number of constraints: " << ortools_linear_problem.constraintCount();

        if (!parameters.noOutput)
        {
            logs.info() << "Writing problem.lp...";
            auto lp_path = outputPath / "problem.lp";
            ortools_linear_problem.WriteLP(lp_path.string());
        }

        logs.info() << "Launching resolution...";
        auto* solution = ortools_linear_problem.solve(parameters.solverLogs);
        switch (solution->getStatus())
        {
        case MipStatus::OPTIMAL:
        case MipStatus::FEASIBLE:
            if (!parameters.noOutput)
            {
                logs.info() << "Writing variables...";
                std::ofstream sol_out(outputPath / "solution.csv");
                for (const auto& [name, value]: solution->getOptimalValues())
                {
                    sol_out << name << " " << value << std::endl;
                }
            }
            break;
        default:
            logs.error() << "Problem during linear optimization";
        }
    }
    catch (const LoadFiles::ErrorLoadingYaml&)
    {
        logs.error() << "Error while loading files, exiting";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        logs.error() << e.what();
        logs.error() << "Error during the execution, exiting";
        return EXIT_FAILURE;
    }

    return 0;
}
