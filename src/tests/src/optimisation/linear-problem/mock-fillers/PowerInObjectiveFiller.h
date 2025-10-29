/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#pragma once

#include <antares/optimisation/linear-problem-api/linearProblemFiller.h>
#include "antares/solver/optim-model-filler/ComponentFiller.h"

class PowerInObjectiveFiller: public Antares::Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit PowerInObjectiveFiller(
      Antares::Optimisation::OptimEntityContainer& optimEntityContainer):
        LinearProblemFiller(),
        optimEntityContainer_(optimEntityContainer)
    {
        Antares::ModelerStudy::SystemModel::Objective obj = {
          "obj-with-power",
          Antares::ModelerStudy::SystemModel::Expression{"a ^ 2", {}}};
        std::vector<Antares::ModelerStudy::SystemModel::Objective> objectives;
        objectives.emplace_back(std::move(obj));
        Antares::ModelerStudy::SystemModel::ModelBuilder modelBuilder;
        model_ = std::make_unique<Antares::ModelerStudy::SystemModel::Model>(
          modelBuilder.withId("dummy").withObjectives(std::move(objectives)).build());
        Antares::ModelerStudy::SystemModel::ComponentBuilder componentBuilder;
        component_ = std::make_unique<Antares::ModelerStudy::SystemModel::Component>(
          componentBuilder.withId("comp-with-power-in-objective")
            .withModel(model_.get())
            .withIndex(0)
            .build());
        stut_ = std::make_unique<Antares::Optimisation::ComponentFiller>(
          *component_,
          optimEntityContainer_,
          Antares::Optimisation::ScenarioGroupRepository{});
        optimEntityContainer_.addFromSystemComponents({*component_});
    }

    void addVariables(
      [[maybe_unused]] const Antares::Optimisation::LinearProblemApi::FillContext& ctx)
    {
        optimEntityContainer_.Problem().addNumVariable(0, 1, added_var_name_);
    }

    void addConstraints(
      [[maybe_unused]] const Antares::Optimisation::LinearProblemApi::FillContext& ctx)
    {
    }

    void addObjectives(
      [[maybe_unused]] const Antares::Optimisation::LinearProblemApi::FillContext& ctx)
    {
        stut_->addObjectives(ctx);
    }

    Antares::Optimisation::OptimEntityContainer& optimEntityContainer_;
    std::string added_var_name_ = "var-by-PowerFiller";
    std::unique_ptr<Antares::ModelerStudy::SystemModel::Model> model_;
    std::unique_ptr<Antares::ModelerStudy::SystemModel::Component> component_;
    std::unique_ptr<Antares::Optimisation::ComponentFiller> stut_;
    NodeRegistry power_node_registry_;
};
