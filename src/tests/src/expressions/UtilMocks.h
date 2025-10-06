#pragma once
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>

#include "../modeler/mockModelerObjects.h"

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
      const std::string& name) const override
    {
        // static MockMipConstraint mockConstraint(
        //   Antares::Optimisation::LinearProblemApi::MipBasisStatus::BASIC);
        // return &mockConstraint;
        return nullptr;
    }

    // ILinearProblem interface (minimal implementation for testing)
    Antares::Optimisation::LinearProblemApi::IMipVariable* addVariable(double,
                                                                       double,
                                                                       bool,
                                                                       const std::string&) override
    {
        // variables_.push_back(std::move(RandomVariable()));
        // return variables_.back().get();
        return nullptr;
    }

    Antares::Optimisation::LinearProblemApi::IMipConstraint*
    addConstraint(double, double, const std::string&) override
    {
        // constraints_.push_back(std::move(RandomConstraint()));
        // return constraints_.back().get();
        return nullptr;
    }

    void setObjectiveCoefficient(Antares::Optimisation::LinearProblemApi::IMipVariable*,
                                 double) override
    {
    }

    Antares::Optimisation::LinearProblemApi::IMipSolution* solve(bool) override
    {
        return nullptr;
    }

    void WriteLP(const std::string&) const override
    {
    }

    double infinity() const override
    {
        return std::numeric_limits<double>::infinity();
    }

    Antares::Optimisation::LinearProblemApi::IMipVariable*
    addNumVariable(double lb, double ub, const std::string& name) override
    {
        // variables_.push_back(std::move(RandomVariable()));
        // return variables_.back().get();
        return nullptr;
    }

    Antares::Optimisation::LinearProblemApi::IMipVariable*
    addIntVariable(double lb, double ub, const std::string& name) override
    {
        // variables_.push_back(std::move(RandomVariable()));
        // return variables_.back().get();
        return nullptr;
    }

    static std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipVariable> RandomVariable()
    {
        std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipVariable>
          mockMipVariable = std::make_unique<MockMipVariable>(
            12.25,
            Antares::Optimisation::LinearProblemApi::MipBasisStatus::AT_LOWER_BOUND,
            false);
        return std::move(mockMipVariable);
    }

    static std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>
    RandomConstraint()
    {
        std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>
          mockMipConstraint = std::make_unique<MockMipConstraint>(
            Antares::Optimisation::LinearProblemApi::MipBasisStatus::AT_LOWER_BOUND);
        return std::move(mockMipConstraint);
    }

    [[nodiscard]] Antares::Optimisation::LinearProblemApi::IMipVariable* getVariable(
      std::size_t t) const override
    {
        // return variables_.at(t).get();
        return nullptr;
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
        // return RandomConstraint().get();
        return nullptr;
    }

    [[nodiscard]]
    const std::vector<std::unique_ptr<Antares::Optimisation::LinearProblemApi::IMipConstraint>>&
    getConstraints() const override
    {
        return constraints_;
    }

    [[nodiscard]] Antares::Optimisation::LinearProblemApi::IMipVariable* lookupVariable(
      const std::string& name) const override
    {
        // return RandomVariable().get();
        return nullptr;
    }

    [[nodiscard]] int variableCount() const override
    {
        return variables_.size();
    }

    [[nodiscard]] int constraintCount() const override
    {
        return constraints_.size();
    }

    double getObjectiveCoefficient(
      const Antares::Optimisation::LinearProblemApi::IMipVariable* var) const override
    {
        return 0.;
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
    }
};

inline Antares::Optimisation::ScenarioGroupRepository getscenarioGroupRepository(
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
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    Antares::ModelerStudy::SystemModel::Model model = createModelWithoutParameters();
    std::vector<Antares::ModelerStudy::SystemModel::Component> components = {
      std::move(createComponent(model))};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepository
      = getscenarioGroupRepository(components.front());

    MockLinearProblem linearProblem = MockLinearProblem(true);
    Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};

    Antares::Optimisation::OptimEntityContainer optimEntityContainer = Antares::Optimisation::
      OptimEntityContainer(linearProblem, &data, &scenarioGroupRepository);

    std::unique_ptr<Antares::Expressions::Visitors::EvalVisitor> defaultComponentEvalVisitor;

    MyDummyFixture()
    {
        for (const auto& compo: components)
        {
            optimEntityContainer.addFromSystemComponent(compo);
            defaultComponentEvalVisitor = std::make_unique<
              Antares::Expressions::Visitors::EvalVisitor>(optimEntityContainer, ctx, compo);
        }
    }

    Antares::ModelerStudy::SystemModel::Component* addComponent(
      const std::string& id,
      const Antares::ModelerStudy::SystemModel::Model& model,
      std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue>
        paramsAndValues)
    {
        components.emplace_back(createComponent(model, id, paramsAndValues, components.size()));
        optimEntityContainer.addFromSystemComponent(components.back());
        return &components.back();
    }
};
