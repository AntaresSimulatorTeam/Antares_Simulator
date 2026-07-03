// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

#include "mockModelerObjects.h"

class MockMipVariable: public Antares::LinearProblem::Api::IMipVariable
{
public:
    MockMipVariable(double value,
                    Antares::LinearProblem::Api::MipBasisStatus status,
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

    Antares::LinearProblem::Api::MipBasisStatus getMipBasisStatus() const override
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
    Antares::LinearProblem::Api::MipBasisStatus status_;
    bool integer_;
    std::string name_ = "test_var";
};

class MockMipConstraint: public Antares::LinearProblem::Api::IMipConstraint
{
public:
    MockMipConstraint(Antares::LinearProblem::Api::MipBasisStatus status):
        status_(status)
    {
    }

    Antares::LinearProblem::Api::MipBasisStatus getMipBasisStatus() const override
    {
        return status_;
    }

    // IMipConstraint interface
    void setCoefficient(Antares::LinearProblem::Api::IMipVariable*, double) override
    {
    }

    double getCoefficient(
      const Antares::LinearProblem::Api::IMipVariable*) const override
    {
        return 1.0;
    }

    [[nodiscard]] std::vector<std::pair<int, double>> getCoefficients() const override
    {
        return {};
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
    Antares::LinearProblem::Api::MipBasisStatus status_;
    std::string name_ = "test_constraint";
};

class MockLinearProblem: public Antares::LinearProblem::Api::ILinearProblem
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

    Antares::LinearProblem::Api::IMipConstraint* lookupConstraint(
      const std::string&) const override
    {
        static MockMipConstraint mockConstraint(
          Antares::LinearProblem::Api::MipBasisStatus::BASIC);
        return &mockConstraint;
    }

    // ILinearProblem interface (minimal implementation for testing)
    Antares::LinearProblem::Api::IMipVariable* addVariable(double,
                                                                       double,
                                                                       bool,
                                                                       const std::string&) override
    {
        variables_.push_back(RandomVariable());
        return variables_.back().get();
    }

    Antares::LinearProblem::Api::IMipConstraint*
    addConstraint(double, double, const std::string&) override
    {
        constraints_.push_back(RandomConstraint());
        return constraints_.back().get();
    }

    void setObjectiveCoefficient(Antares::LinearProblem::Api::IMipVariable*,
                                 double) override
    {
    }

    Antares::LinearProblem::Api::IMipSolution* solve(bool) override
    {
        return nullptr;
    }

    double infinity() const override
    {
        return std::numeric_limits<double>::infinity();
    }

    Antares::LinearProblem::Api::IMipVariable*
    addNumVariable(double, double, const std::string&) override
    {
        variableCount_++;
        variables_.push_back(RandomVariable());
        return variables_.back().get();
    }

    Antares::LinearProblem::Api::IMipVariable*
    addIntVariable(double, double, const std::string&) override
    {
        variableCount_++;
        variables_.push_back(RandomVariable());
        return variables_.back().get();
    }

    static std::unique_ptr<Antares::LinearProblem::Api::IMipVariable> RandomVariable()
    {
        std::unique_ptr<Antares::LinearProblem::Api::IMipVariable>
          mockMipVariable = std::make_unique<MockMipVariable>(
            12.25,
            Antares::LinearProblem::Api::MipBasisStatus::AT_LOWER_BOUND,
            false);
        return mockMipVariable;
    }

    static std::unique_ptr<Antares::LinearProblem::Api::IMipConstraint>
    RandomConstraint()
    {
        std::unique_ptr<Antares::LinearProblem::Api::IMipConstraint>
          mockMipConstraint = std::make_unique<MockMipConstraint>(
            Antares::LinearProblem::Api::MipBasisStatus::AT_LOWER_BOUND);
        return mockMipConstraint;
    }

    [[nodiscard]] Antares::LinearProblem::Api::IMipVariable* getVariable(
      std::size_t t) const override
    {
        return variables_.at(t).get();
    }

    [[nodiscard]]
    const std::vector<std::unique_ptr<Antares::LinearProblem::Api::IMipVariable>>&
    getVariables() const override
    {
        return variables_;
    }

    [[nodiscard]]
    Antares::LinearProblem::Api::IMipConstraint* getConstraint(
      std::size_t) const override
    {
        static MockMipConstraint mock(
          Antares::LinearProblem::Api::MipBasisStatus::AT_LOWER_BOUND);
        return &mock;
    }

    [[nodiscard]]
    const std::vector<std::unique_ptr<Antares::LinearProblem::Api::IMipConstraint>>&
    getConstraints() const override
    {
        return constraints_;
    }

    [[nodiscard]] Antares::LinearProblem::Api::IMipVariable* lookupVariable(
      const std::string&) const override
    {
        static MockMipVariable mock(
          12.25,
          Antares::LinearProblem::Api::MipBasisStatus::AT_LOWER_BOUND,
          false);
        return &mock;
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
      const Antares::LinearProblem::Api::IMipVariable*) const override
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

    [[nodiscard]] double objectiveValue() const override
    {
        return 0.0;
    }

protected:
    bool isLP_;
    std::vector<std::unique_ptr<Antares::LinearProblem::Api::IMipVariable>> variables_;
    std::vector<std::unique_ptr<Antares::LinearProblem::Api::IMipConstraint>>
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
          Antares::LinearProblem::Api::MipBasisStatus::AT_LOWER_BOUND,
          false));
        variableCount_++;
    }
};

inline Antares::LinearProblem::ScenarioGroupRepository makeScenarioGroupRepo(
  const Antares::ModelerStudy::SystemModel::Component& component)
{
    Antares::LinearProblem::ScenarioGroupRepository repository;
    repository.addScenario(
      component.getScenarioGroupId(),
      std::make_unique<Antares::LinearProblem::Api::EmptyScenario>());
    return repository;
}

struct MyDummyFixture: Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>
{
    Antares::LinearProblem::DataImpl::LinearProblemData data;
    Antares::ModelerStudy::SystemModel::Model model = createModelWithoutParameters();
    std::vector<Antares::ModelerStudy::SystemModel::Component> components = {
      createComponent(model)};
    Antares::LinearProblem::ScenarioGroupRepository scenarioGroupRepository = makeScenarioGroupRepo(
      components.front());

    MockLinearProblem linearProblem = MockLinearProblem(true);
    Antares::LinearProblem::Api::FillContext ctx{0, 0, 0, 0, 0};

    Antares::LinearProblem::OptimEntityContainer optimEntityContainer = Antares::LinearProblem::
      OptimEntityContainer(linearProblem);

    std::unique_ptr<Antares::Expressions::Visitors::EvalVisitor> defaultComponentEvalVisitor;

    MyDummyFixture()
    {
        optimEntityContainer.addFromSystemComponents(components);
        for (const auto& compo: components)
        {
            const auto& scenario = scenarioGroupRepository.scenario(compo.getScenarioGroupId());
            defaultComponentEvalVisitor = std::make_unique<
              Antares::Expressions::Visitors::EvalVisitor>(optimEntityContainer,
                                                           ctx,
                                                           compo,
                                                           &data,
                                                           scenario);
        }
    }

    Antares::ModelerStudy::SystemModel::Component* addComponent(
      const std::string& id,
      Antares::ModelerStudy::SystemModel::Model& model,
      std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue>
        paramsAndValues)
    {
        components.emplace_back(createComponent(model, id, paramsAndValues));
        optimEntityContainer.addFromSystemComponents(components);
        return &components.back();
    }
};
