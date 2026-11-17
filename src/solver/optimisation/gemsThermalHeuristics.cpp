// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/gemsThermalHeuristics.h"

#include <algorithm>
#include <cmath>
#include <fmt/format.h>
#include <span>
#include <stdexcept>

#include <antares/logs/logs.h>
#include <antares/modeler-optimisation-container/EvaluationContext.h>
#include <antares/modeler-optimisation-container/OptimEntityContainer.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model-base/variabilityType.h>
#include <antares/utils/utils.h>
#include "antares/solver/optimisation/thermalHeuristicsCore.h"

using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Optimization
{

namespace
{
// Same threshold as the legacy heuristic "fast"
constexpr double ZERO_PMIN = 1.e-2;

unsigned variableIndex(const Component& component, const std::string& variableId)
{
    const auto& variables = component.getModel()->Variables();
    const auto it = std::ranges::find(variables, variableId, &Variable::Id);
    if (it == variables.end())
    {
        throw std::invalid_argument(fmt::format("Component '{}': no variable '{}' in the model",
                                                component.Id(),
                                                variableId));
    }
    return static_cast<unsigned>(std::distance(variables.begin(), it));
}

template<class State>
auto* capturedValues(State& state, HeuristicInputSource source)
{
    using Values = decltype(&state.solutions);
    switch (source)
    {
    case HeuristicInputSource::VariableSolution:
        return &state.solutions;
    case HeuristicInputSource::VariableLowerBound:
        return &state.lowerBounds;
    case HeuristicInputSource::VariableUpperBound:
        return &state.upperBounds;
    case HeuristicInputSource::Parameter:
        return static_cast<Values>(nullptr);
    }
    return static_cast<Values>(nullptr);
}

double readVariable(const LinearProblem::Api::IMipVariable& variable, HeuristicInputSource source)
{
    switch (source)
    {
    case HeuristicInputSource::VariableLowerBound:
        return variable.getLb();
    case HeuristicInputSource::VariableUpperBound:
        return variable.getUb();
    default:
        return variable.solutionValue();
    }
}

/// Values of a heuristic input over the week (a constant input is repeated at each time step)
std::vector<double> inputValues(const GemsHeuristicsState& state,
                                const Solver::ModelerData& modelerData,
                                const Component& component,
                                const HeuristicInput& input,
                                unsigned year)
{
    if (input.source != HeuristicInputSource::Parameter)
    {
        const auto* values = capturedValues(state, input.source);
        const auto it = values->find({component.Id(), input.id});
        if (it == values->end())
        {
            throw std::runtime_error(
              fmt::format("Component '{}': value of variable '{}' from the first optimisation is "
                          "not available for heuristic input '{}'",
                          component.Id(),
                          input.id,
                          toString(input.element)));
        }
        return it->second;
    }

    const LinearProblem::EvaluationContext context(
      &component,
      modelerData.dataSeries.get(),
      &modelerData.scenarioGroupRepository.scenario(component.getScenarioGroupId()));
    const auto parameter = context.getParameter(input.id);
    if (LinearProblem::isConstant(parameter.type))
    {
        return std::vector<double>(state.nbTimeSteps,
                                   LinearProblem::parameterValueToDouble(input.id,
                                                                         parameter.value));
    }
    if (parameter.type == LinearProblem::VariabilityType::VARYING_IN_SCENARIO_ONLY)
    {
        return std::vector<double>(state.nbTimeSteps, context.getParameterValue(input.id, year, 0));
    }
    std::vector<double> values(state.nbTimeSteps);
    for (unsigned t = 0; t < state.nbTimeSteps; ++t)
    {
        values[t] = context.getParameterValue(input.id, year, state.firstGlobalTimeStep + t);
    }
    return values;
}

/// Duration in time steps, bounded to [1, nbTimeSteps] as the legacy durations
int toDuration(double value, unsigned nbTimeSteps)
{
    const auto duration = static_cast<int>(std::lround(value));
    return std::clamp(duration, 1, static_cast<int>(nbTimeSteps));
}

void setOutput(GemsHeuristicsState& state,
               const Component& component,
               const HeuristicOutput& output,
               std::vector<double> values)
{
    if (output.target == HeuristicOutputTarget::VariableLowerBound)
    {
        state.boundOverrides.setLowerBounds(component.Id(), output.id, std::move(values));
    }
    else
    {
        state.boundOverrides.setUpperBounds(component.Id(), output.id, std::move(values));
    }
}

/// Legacy OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT, applied to a GEMS component
void runFastHeuristic(GemsHeuristicsState& state,
                      const Solver::ModelerData& modelerData,
                      const Component& component,
                      const HeuristicDefinition& heuristic,
                      unsigned year)
{
    auto input = [&](HeuristicElement element)
    { return inputValues(state, modelerData, component, *heuristic.input(element), year); };

    const auto production = input(HeuristicElement::GenerationPower);
    const auto clusterMaxGeneration = input(HeuristicElement::ClusterMaxGeneration);
    const double minPowerPerUnit = input(HeuristicElement::MinPowerPerUnit).at(0);
    const double maxPowerPerUnit = input(HeuristicElement::MaxPowerPerUnit).at(0);
    const double minUpDownDuration = std::max(input(HeuristicElement::MinUpDuration).at(0),
                                              input(HeuristicElement::MinDownDuration).at(0));

    if (std::fabs(minPowerPerUnit) < ZERO_PMIN)
    {
        return;
    }

    const int nbTimeSteps = static_cast<int>(state.nbTimeSteps);
    std::vector<int> guide(nbTimeSteps);
    std::vector<int> numberOfUnits(nbTimeSteps);
    OPT_CalculerLaCourbeGuideDuNombreDeGroupes(nbTimeSteps,
                                               [&production](int t) { return production[t]; },
                                               maxPowerPerUnit,
                                               guide);
    if (!OPT_CalculerLeNombreDeGroupesParBlocsDeMUTetMDT(nbTimeSteps,
                                                         toDuration(minUpDownDuration,
                                                                    state.nbTimeSteps),
                                                         guide,
                                                         numberOfUnits))
    {
        return;
    }

    std::vector<double> minimumGeneration(nbTimeSteps);
    for (int t = 0; t < nbTimeSteps; ++t)
    {
        minimumGeneration[t] = std::min(minPowerPerUnit * numberOfUnits[t],
                                        clusterMaxGeneration[t]);
    }
    setOutput(state,
              component,
              *heuristic.output(HeuristicElement::MinimumGenerationPower),
              std::move(minimumGeneration));
}

/// Legacy OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage, applied to a GEMS component.
/// The variables of the component are always relaxed, so the auxiliary linear problem is always
/// solved.
void runAccurateHeuristic(GemsHeuristicsState& state,
                          const Solver::ModelerData& modelerData,
                          const Component& component,
                          const HeuristicDefinition& heuristic,
                          unsigned year)
{
    auto input = [&](HeuristicElement element)
    { return inputValues(state, modelerData, component, *heuristic.input(element), year); };

    const auto numUnitsOn = input(HeuristicElement::NumUnitsOnOpt);
    const auto numUnitsMax = input(HeuristicElement::NumUnitsMax);
    const int minUpDuration = toDuration(input(HeuristicElement::MinUpDuration).at(0),
                                         state.nbTimeSteps);
    const int minDownDuration = toDuration(input(HeuristicElement::MinDownDuration).at(0),
                                           state.nbTimeSteps);

    const int nbTimeSteps = static_cast<int>(state.nbTimeSteps);
    std::vector<int> minNumUnits(nbTimeSteps);
    std::vector<int> maxNumUnits(nbTimeSteps);
    for (int t = 0; t < nbTimeSteps; ++t)
    {
        minNumUnits[t] = static_cast<int>(Antares::Utils::ceil(numUnitsOn[t]));
        maxNumUnits[t] = std::max(static_cast<int>(Antares::Utils::ceil(numUnitsMax[t])),
                                  minNumUnits[t]);
    }

    OPT_ResoudreLePbLineaireDuNombreMinDeGroupesEnMarche(minNumUnits,
                                                         maxNumUnits,
                                                         minUpDuration,
                                                         minDownDuration,
                                                         nbTimeSteps);

    setOutput(state,
              component,
              *heuristic.output(HeuristicElement::MinimumNumUnitsOn),
              std::vector<double>(minNumUnits.begin(), minNumUnits.end()));
}
} // namespace

void GemsHeuristicsState::reset(unsigned firstGlobalTimeStep_, unsigned nbTimeSteps_)
{
    firstGlobalTimeStep = firstGlobalTimeStep_;
    nbTimeSteps = nbTimeSteps_;
    solutions.clear();
    lowerBounds.clear();
    upperBounds.clear();
    boundOverrides.reset(firstGlobalTimeStep_);
}

void captureGemsHeuristicInputs(GemsHeuristicsState& state,
                                const Solver::ModelerData& modelerData,
                                const LinearProblem::OptimEntityContainer& optimEntityContainer,
                                const LinearProblem::Api::FillContext& fillContext)
{
    for (const auto& component: modelerData.system->Components())
    {
        const auto* heuristic = modelerData.heuristicFor(component);
        if (!heuristic)
        {
            continue;
        }

        for (const auto& input: heuristic->inputs)
        {
            auto* values = capturedValues(state, input.source);
            if (!values)
            {
                continue;
            }
            const auto index = variableIndex(component, input.id);
            const bool timeDependent = component.getModel()->Variables()[index].isTimeDependent();
            const std::size_t nbVariables = timeDependent ? fillContext.getLocalNumberOfTimeSteps()
                                                          : 1;
            const auto variables = optimEntityContainer.getComponentVariable(component,
                                                                             index,
                                                                             nbVariables);

            auto& weekValues = (*values)[{component.Id(), input.id}];
            weekValues.resize(state.nbTimeSteps, 0.);
            if (!timeDependent)
            {
                std::ranges::fill(weekValues, readVariable(*variables[0], input.source));
                continue;
            }
            for (std::size_t t = 0; t < nbVariables; ++t)
            {
                const auto globalTimeStep = fillContext.getGlobalFirstTimeStep() + t;
                if (globalTimeStep < state.firstGlobalTimeStep)
                {
                    continue;
                }
                const auto weekTimeStep = globalTimeStep - state.firstGlobalTimeStep;
                if (weekTimeStep < weekValues.size())
                {
                    weekValues[weekTimeStep] = readVariable(*variables[t], input.source);
                }
            }
        }
    }
}

void runGemsThermalHeuristics(GemsHeuristicsState& state,
                              const Solver::ModelerData& modelerData,
                              unsigned year)
{
    for (const auto& component: modelerData.system->Components())
    {
        const auto* heuristic = modelerData.heuristicFor(component);
        if (!heuristic)
        {
            continue;
        }

        switch (heuristic->type)
        {
        case HeuristicType::Fast:
            runFastHeuristic(state, modelerData, component, *heuristic, year);
            break;
        case HeuristicType::Accurate:
            runAccurateHeuristic(state, modelerData, component, *heuristic, year);
            break;
        }
    }
}

} // namespace Antares::Optimization
