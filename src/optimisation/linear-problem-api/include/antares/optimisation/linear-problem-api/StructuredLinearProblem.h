// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <limits>
#include <map>
#include <string>
#include <vector>

#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-api/mipConstraint.h>
#include <antares/optimisation/linear-problem-api/mipVariable.h>

namespace Antares::Optimisation::LinearProblemApi
{

class StructuredMipVariable final: public IMipVariable
{
public:
    StructuredMipVariable(double lb,
                          double ub,
                          bool integer,
                          const std::string& name,
                          unsigned int index);
    ~StructuredMipVariable() override = default;

    void setLb(double lb) override;
    void setUb(double ub) override;
    void setBounds(double lb, double ub) override;

    double getLb() const override;
    double getUb() const override;

    const std::string& getName() const override;

    bool isInteger() const override;

    MipBasisStatus getMipBasisStatus() const override;
    double solutionValue() const override;
    double reducedCost() const override;

    [[nodiscard]] unsigned int index() const;

private:
    double lb_;
    double ub_;
    bool integer_;
    std::string name_;
    unsigned int index_;
};

class StructuredMipConstraint final: public IMipConstraint
{
public:
    StructuredMipConstraint(double lb, double ub, const std::string& name, unsigned int index);
    ~StructuredMipConstraint() override = default;

    void setLb(double lb) override;
    void setUb(double ub) override;
    void setBounds(double lb, double ub) override;

    double getLb() const override;
    double getUb() const override;

    void setCoefficient(IMipVariable* var, double coefficient) override;

    double getCoefficient(const IMipVariable* var) const override;
    [[nodiscard]] std::vector<std::pair<int, double>> getCoefficients() const override;

    double dual() const override;

    [[nodiscard]] const std::string& getName() const override;
    [[nodiscard]] MipBasisStatus getMipBasisStatus() const override;

    [[nodiscard]] unsigned int index() const;

private:
    double lb_;
    double ub_;
    std::string name_;
    unsigned int index_;
    std::map<unsigned int, double> coefficients_;
};

class StructuredLinearProblem final: public ILinearProblem
{
public:
    StructuredLinearProblem();
    ~StructuredLinearProblem() override = default;

    IMipVariable* addNumVariable(double lb, double ub, const std::string& name) override;
    IMipVariable* addIntVariable(double lb, double ub, const std::string& name) override;
    IMipVariable* addVariable(double lb, double ub, bool integer, const std::string& name) override;

    [[nodiscard]] const std::vector<std::unique_ptr<IMipVariable>>& getVariables() const override;
    [[nodiscard]] IMipVariable* getVariable(std::size_t index) const override;
    [[nodiscard]] IMipVariable* lookupVariable(const std::string& name) const override;
    [[nodiscard]] int variableCount() const override;

    IMipConstraint* addConstraint(double lb, double ub, const std::string& name) override;
    [[nodiscard]] const std::vector<std::unique_ptr<IMipConstraint>>& getConstraints()
      const override;
    [[nodiscard]] IMipConstraint* getConstraint(std::size_t index) const override;
    [[nodiscard]] IMipConstraint* lookupConstraint(const std::string& name) const override;
    [[nodiscard]] int constraintCount() const override;

    void setObjectiveCoefficient(IMipVariable* var, double coefficient) override;
    double getObjectiveCoefficient(const IMipVariable* var) const override;

    void setObjectiveOffset(double objectiveOffset) override;
    [[nodiscard]] double getObjectiveOffset() const override;

    void setMinimization() override;
    void setMaximization() override;

    [[nodiscard]] bool isMinimization() const override;
    [[nodiscard]] bool isMaximization() const override;

    IMipSolution* solve(bool verboseSolver) override;

    [[nodiscard]] double infinity() const override;
    [[nodiscard]] bool isLP() const override;

    double objectiveValue() const override;

private:
    std::vector<std::unique_ptr<IMipVariable>> variables_;
    std::vector<std::unique_ptr<IMipConstraint>> constraints_;
    std::vector<double> objectiveCoefficients_;
    double objectiveOffset_ = 0.0;
    bool minimize_ = true;
    bool isMip_ = false;
    double infinity_ = std::numeric_limits<double>::infinity();
};

} // namespace Antares::Optimisation::LinearProblemApi
