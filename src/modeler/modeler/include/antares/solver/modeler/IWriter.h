// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "data.h"

namespace Antares::Optimization
{
class VariableDictionary;
}

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{
class OrtoolsLinearProblem;
class OrtoolsMipSolution;
} // namespace Antares::Optimisation::LinearProblemMpsolverImpl

namespace Antares::Optimisation
{
class OptimEntityContainer;

namespace LinearProblemApi
{
class IMipVariable;
class ILinearProblem;
class ILinearProblemData;
class IMipSolution;
class FillContext;
} // namespace LinearProblemApi
} // namespace Antares::Optimisation

namespace Antares::ModelerStudy::SystemModel
{
class Component;
}

namespace Antares::Solver
{
// TODO this class needs to be revamped
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void init(const std::string& simulationId) = 0;
    virtual void writeSimulationTable(
      const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
      const Optimisation::LinearProblemApi::IMipSolution& solution,
      const Antares::Modeler::Data& modelerData,
      const Optimisation::OptimEntityContainer& variableContainer,
      const Optimisation::LinearProblemApi::FillContext& fillContext) const
      = 0;
    virtual const std::filesystem::path& outputPath() const = 0;
};
} // namespace Antares::Solver
