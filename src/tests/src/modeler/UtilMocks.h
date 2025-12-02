/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#pragma once

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

#include "mockModelerObjects.h"

class MockMipVariable: public Antares::Optimisation::LinearProblemApi::IMipVariable
{
public:
    MockMipVariable(double value,
                    Antares::Optimisation::LinearProblemApi::MipBasisStatus status,
                    bool integer = false):
        value_(value),
        status_(status),
        integer_(integer)
    {
    }

    double solutionValue() const override
    {
        return value_;
    }

    double reducedCost() const override
    {
        return 4.96; // arbitrary value for testing
    }

    Antares::Optimisation::LinearProblemApi::MipBasisStatus getMipBasisStatus() const override
    {
        return status_;
    }

    bool isInteger() const override
    {
        return integer_;
    }

    // IHasBounds interface
    void setLb(double) override
    {
    }

    void setUb(double) override
    {
    }

    double getLb() const override
    {
        return 0.0;
    }

    double getUb() const override
    {
        return 1.0;
    }

    void setBounds(double lb, double ub) override
    {
        setLb(lb);
        setUb(ub);
    }

    // IHasName interface
    const std::string& getName() const override
    {
        return name_;
    }

private:
    double value_;
    Antares::Optimisation::LinearProblemApi::MipBasisStatus status_;
    bool integer_;
    std::string name_ = "test_var";
};

class MockMipConstraint: public Antares::Optimisation::LinearProblemApi::IMipConstraint
{
public:
    MockMipConstraint(Antares::Optimisation::LinearProblemApi::MipBasisStatus status):
        status_(status)
    {
    }

    Antares::Optimisation::LinearProblemApi::MipBasisStatus getMipBasisStatus() const override
    {
        return status_;
    }

    // IMipConstraint interface
    void setCoefficient(Antares::Optimisation::LinearProblemApi::IMipVariable*, double) override
    {
    }

    double getCoefficient(
      const Antares::Optimisation::LinearProblemApi::IMipVariable*) const override
    {
        return 1.0;
    }

    // IHasBounds interface
    void setLb(double) override
    {
    }

    void setUb(double) override
    {
    }

    double getLb() const override
    {
        return 0.0;
    }

    double getUb() const override
    {
        return 1.0;
    }

    void setBounds(double lb, double ub) override
    {
        setLb(lb);
        setUb(ub);
    }

    double dual() const override
    {
        return 8.23; // arbitrary value for testing
    }

    // IHasName interface
    const std::string& getName() const override
    {
        return name_;
    }

private:
    Antares::Optimisation::LinearProblemApi::MipBasisStatus status_;
    std::string name_ = "test_constraint";
};

class MockLinearProblem: public Antares::Optimisation::LinearProblemApi::ILinearProblem
{
public:
    explicit MockLinearProblem(bool isLP):
        isLP_(isLP)
    {
    }

    bool isLP() const override
    {
        return isLP_;
    }

    Antares::Optimisation::LinearProblemApi::IMipConstraint* lookupConstraint(
      const std::string&) const override
    {
        static MockMipConstraint mockConstraint(
          Antares::Optimisation::LinearProblemApi::MipBasisStatus::BASIC);
        return &mockConstraint;
    }

    // ILinearProblem interface (minimal implementation for testing)
    Antares::Optimisation::LinearProblemApi::IMipVariable* addVariable(double,
                                                                       double,
                                                                       bool,
                                                                       const std::string&) override
    {
        variables_.push_back(RandomVariable());
        return variables_.back().get();
    }

    Antares::Optimisation::LinearProblemApi::IMipConstraint*
    addConstraint(double, double, const std::string&) override
    {
        constraints_.push_back(RandomConstraint());
        return constraints_.back().get();
    }

    void setObjectiveCoefficient(Antares::Optimisation::LinearProblemApi::IMipVariable*,
                                 double) override
    {
    }

    Antares::Optimisation::LinearProblemApi::IMipSolution* solve(bool) override
    {
        return nullptr;
    }

    double infinity() const override
    {
        return std::numeric_limits<double>::infinity();
    }

    Antares::Optimisation::LinearProblemApi::IMipVariable*
    addNumVariable(double, double, const std::string&) override
    {
        variableCount_++;
        variables_.push_back(RandomVariable());
        return variables_.back().get();
    }

    Antares::Optimisation::LinearProblemApi::IMipVariable*
    addIntVariable(double, double, const std::string&) override
    {
        variableCount_++;
        variables_.push_back(RandomVariable());
        return variables_.back().get();
    }

    static std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipVariable> RandomVariable()
    {
        std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipVariable>
          mockMipVariable = std::make_unique<MockMipVariable>(
            12.25,
            Antares::Optimisation::LinearProblemApi::MipBasisStatus::AT_LOWER_BOUND,
            false);
        return mockMipVariable;
    }

    static std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>
    RandomConstraint()
    {
        std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>
          mockMipConstraint = std::make_unique<MockMipConstraint>(
            Antares::Optimisation::LinearProblemApi::MipBasisStatus::AT_LOWER_BOUND);
        return mockMipConstraint;
    }

    [[nodiscard]] Antares::Optimisation::LinearProblemApi::IMipVariable* getVariable(
      std::size_t t) const override
    {
        return variables_.at(t).get();
    }

    [[nodiscard]]
    const std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipVariable>>&
    getVariables() const override
    {
        return variables_;
    }

    [[nodiscard]]
    Antares::Optimisation::LinearProblemApi::IMipConstraint* getConstraint(
      std::size_t) const override
    {
        return RandomConstraint().get();
    }

    [[nodiscard]]
    const std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>>&
    getConstraints() const override
    {
        return constraints_;
    }

    [[nodiscard]] Antares::Optimisation::LinearProblemApi::IMipVariable* lookupVariable(
      const std::string&) const override
    {
        return RandomVariable().get();
    }

    [[nodiscard]] int variableCount() const override
    {
        return variableCount_;
    }

    [[nodiscard]] int constraintCount() const override
    {
        return constraintCount_;
    }

    double getObjectiveCoefficient(
      const Antares::Optimisation::LinearProblemApi::IMipVariable*) const override
    {
        return 0.;
    }

    void setObjectiveOffset(double) override
    {
    }

    double getObjectiveOffset() const override
    {
        return {};
    }

    void setMinimization() override
    {
    }

    void setMaximization() override
    {
    }

    [[nodiscard]] bool isMinimization() const override
    {
        return true;
    }

    [[nodiscard]] bool isMaximization() const override
    {
        return !isMinimization();
    }

protected:
    bool isLP_;
    std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipVariable>> variables_;
    std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>>
      constraints_;
    int variableCount_ = 0;
    int constraintCount_ = 0;
};

// Mock component and model classes for testing template functions
class MockVariable
{
public:
    MockVariable(bool scenDependent, bool timeDependent):
        scenDependent_(scenDependent),
        timeDependent_(timeDependent)
    {
    }

    bool IsScenarioDependent() const
    {
        return scenDependent_;
    }

    bool isTimeDependent() const
    {
        return timeDependent_;
    }

private:
    bool scenDependent_;
    bool timeDependent_;
};

class MockConstraint
{
public:
    MockConstraint(const std::string& name):
        name_(name)
    {
    }

    // Mock expression method
    struct MockExpression
    {
        struct MockNode
        {
            // Mock node for constraint expression
        };

        MockNode* RootNode() const
        {
            return nullptr;
        }
    };

    MockExpression expression() const
    {
        return MockExpression{};
    }

private:
    std::string name_;
};

class MockModel
{
public:
    std::map<std::string, MockVariable> Variables() const
    {
        return {
          {"var1", MockVariable(false, false)}, // Neither time nor scenario dependent
          {"var2", MockVariable(true, false)},  // Scenario dependent only
          {"var3", MockVariable(false, true)},  // Time dependent only
          {"var4", MockVariable(true, true)}    // Both time and scenario dependent
        };
    }

    std::map<std::string, MockConstraint> Constraints() const
    {
        return {{"constraint1", MockConstraint("constraint1")},
                {"constraint2", MockConstraint("constraint2")}};
    }
};

class MockComponent
{
public:
    MockComponent(const std::string& id):
        id_(id),
        model_(std::make_shared<MockModel>())
    {
    }

    const std::string& Id() const
    {
        return id_;
    }

    std::shared_ptr<MockModel> getModel() const
    {
        return model_;
    }

private:
    std::string id_;
    std::shared_ptr<MockModel> model_;
};

struct PredfinedSolutionLinearProblemMock: MockLinearProblem
{
    PredfinedSolutionLinearProblemMock(bool lp):
        MockLinearProblem(lp)
    {
    }

    void addVariableValue(double value)
    {
        variables_.push_back(std::make_unique<MockMipVariable>(
          value,
          Antares::Optimisation::LinearProblemApi::MipBasisStatus::AT_LOWER_BOUND,
          false));
        variableCount_++;
    }
};

inline Antares::Optimisation::ScenarioGroupRepository makeScenarioGroupRepo(
  const Antares::ModelerStudy::SystemModel::Component& component)
{
    Antares::Optimisation::ScenarioGroupRepository repository;
    repository.addScenario(
      component.getScenarioGroupId(),
      std::make_unique<Antares::Optimisation::LinearProblemApi::EmptyScenario>());
    return repository;
}

struct MyDummyFixture: Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>
{
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    Antares::ModelerStudy::SystemModel::Model model = createModelWithoutParameters();
    std::vector<Antares::ModelerStudy::SystemModel::Component> components = {
      std::move(createComponent(model))};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepository = makeScenarioGroupRepo(
      components.front());

    MockLinearProblem linearProblem = MockLinearProblem(true);
    Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};

    Antares::Optimisation::OptimEntityContainer optimEntityContainer = Antares::Optimisation::
      OptimEntityContainer(linearProblem, &data, &scenarioGroupRepository);

    std::unique_ptr<Antares::Expressions::Visitors::EvalVisitor> defaultComponentEvalVisitor;

    MyDummyFixture()
    {
        optimEntityContainer.addFromSystemComponents(components);
        for (const auto& compo: components)
        {
            defaultComponentEvalVisitor = std::make_unique<
              Antares::Expressions::Visitors::EvalVisitor>(optimEntityContainer, ctx, compo);
        }
    }

    Antares::ModelerStudy::SystemModel::Component* addComponent(
      const std::string& id,
      Antares::ModelerStudy::SystemModel::Model& model,
      std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue>
        paramsAndValues)
    {
        components.emplace_back(createComponent(model, id, paramsAndValues, components.size()));
        optimEntityContainer.addFromSystemComponents(components);
        return &components.back();
    }
};
