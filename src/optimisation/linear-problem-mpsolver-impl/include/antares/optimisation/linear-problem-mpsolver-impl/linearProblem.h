// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipConstraint.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>

namespace operations_research
{
class MPSolver;
class MPSolverParameters;
class MPObjective;
} // namespace operations_research

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

class OrtoolsLinearProblem: public LinearProblemApi::ILinearProblem
{
public:
    OrtoolsLinearProblem(bool isMip, const std::string& solverName);
    ~OrtoolsLinearProblem() override = default;

    LinearProblemApi::IMipVariable* addNumVariable(double lb,
                                                   double ub,
                                                   const std::string& name) override;

    LinearProblemApi::IMipVariable* addIntVariable(double lb,
                                                   double ub,
                                                   const std::string& name) override;

    LinearProblemApi::IMipVariable* addVariable(double lb,
                                                double ub,
                                                bool integer,
                                                const std::string& name) override;
    [[nodiscard]] const std::vector<std::unique_ptr<LinearProblemApi::IMipVariable>>& getVariables()
      const override;
    [[nodiscard]] LinearProblemApi::IMipVariable* getVariable(std::size_t index) const override;
    [[nodiscard]] LinearProblemApi::IMipVariable* lookupVariable(
      const std::string& name) const override;

    [[nodiscard]] int variableCount() const override;

    LinearProblemApi::IMipConstraint* addConstraint(double lb,
                                                    double ub,
                                                    const std::string& name) override;
    [[nodiscard]] const std::vector<std::unique_ptr<LinearProblemApi::IMipConstraint>>&
    getConstraints() const override;
    [[nodiscard]] LinearProblemApi::IMipConstraint* getConstraint(std::size_t index) const override;
    [[nodiscard]] LinearProblemApi::IMipConstraint* lookupConstraint(
      const std::string& name) const override;
    [[nodiscard]] int constraintCount() const override;

    void setObjectiveCoefficient(LinearProblemApi::IMipVariable* var, double coefficient) override;
    double getObjectiveCoefficient(const LinearProblemApi::IMipVariable* var) const override;

    /**
     * @brief Sets the constant offset for the objective function.
     *
     * The objective offset is a constant term added to the objective function.
     * It shifts the objective value but does not affect the optimal solution.
     * For the Sirius solver, this value is stored locally; for other solvers, it uses OR-Tools'
     * native support.
     *
     * @param objectiveOffset The constant offset to add to the objective function.
     */
    void setObjectiveOffset(double objectiveOffset) override;

    /**
     * @brief Returns the current objective offset value.
     *
     * Retrieves the constant term currently added to the objective function.
     * For the Sirius solver, this value is stored locally; for other solvers, it uses OR-Tools'
     * native support.
     *
     * @return The current objective offset.
     */
    [[nodiscard]] double getObjectiveOffset() const override;

    void setMinimization() override;
    void setMaximization() override;

    [[nodiscard]] bool isMinimization() const override;
    [[nodiscard]] bool isMaximization() const override;

    OrtoolsMipSolution* solve(bool verboseSolver) override;

    /**
     * Return the last solution or solve if no solution exist.
     * @param verboseSolver
     * @return The solution
     */
    OrtoolsMipSolution* solution(bool verboseSolver);
    double objectiveValue() const override;

    [[nodiscard]] double infinity() const override;
    [[nodiscard]] bool isLP() const override;

    friend void Write(const OrtoolsLinearProblem& problem, const std::filesystem::path& path);

protected:
    std::shared_ptr<operations_research::MPSolver> mpSolver_;

private:
    operations_research::MPObjective* objective_{nullptr};
    operations_research::MPSolverParameters params_;

    std::vector<std::unique_ptr<LinearProblemApi::IMipVariable>> variables_;
    std::vector<std::unique_ptr<LinearProblemApi::IMipConstraint>> constraints_;

    std::unique_ptr<OrtoolsMipSolution> solution_;
    bool isLP_{true};
};

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
