# pragma once

#include <antares/expressions/Registry.hxx>
#include "antares/expressions/visitors/CloneVisitor.h"
#include "antares/study/system-model/component.h"

#include <antares/expressions/Registry.hxx>
#include "antares/expressions/visitors/NodeVisitor.h"

class ComponentExpressionVisitor: public Antares::Expressions::Visitors::CloneVisitor {

public:
    ComponentExpressionVisitor(
      Antares::Expressions::Registry<Antares::Expressions::Nodes::Node>& registry,
      std::map<std::string, double> parameter_values):
        CloneVisitor(registry),
        parameter_values_(parameter_values)
    {
    }
    std::string name() const override;
    Antares::Expressions::Nodes::Node* visit(const Antares::Expressions::Nodes::ParameterNode* node) override;
private:
    std::map<std::string, double> parameter_values_;
};



