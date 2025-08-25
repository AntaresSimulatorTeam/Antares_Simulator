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

#include <concepts>

#include "hasBounds.h"
#include "hasName.h"

namespace Antares::Optimisation::LinearProblemApi
{

// Concept pour identifier un tag de solveur
template<typename Tag>
concept SolverTag = requires {
    typename Tag::VariableType;
    typename Tag::ConstraintType;
};

// Concept pour valider qu'un type peut être utilisé comme variable de solveur interne
template<typename T>
concept SolverVariable = requires(T t, const T ct) {
    { ct.lb() } -> std::convertible_to<double>;
    { ct.ub() } -> std::convertible_to<double>;
    { ct.name() } -> std::convertible_to<std::string>;
    { ct.integer() } -> std::convertible_to<bool>;
    t.SetLB(double{});
    t.SetUB(double{});
    t.SetBounds(double{}, double{});
};

template<SolverVariable InnerSolverVariable>
class IMipVariable: public IHasBounds, public IHasName
{
public:
    explicit IMipVariable(InnerSolverVariable* innerVar):
        innerVar_(innerVar)
    {
    }

    virtual ~IMipVariable() = default;

    // Implémentation des méthodes IHasBounds
    void setLb(double lb) override
    {
        innerVar_->SetLB(lb);
    }

    void setUb(double ub) override
    {
        innerVar_->SetUB(ub);
    }

    void setBounds(double lb, double ub) override
    {
        innerVar_->SetBounds(lb, ub);
    }

    double getLb() const override
    {
        return innerVar_->lb();
    }

    double getUb() const override
    {
        return innerVar_->ub();
    }

    // Implémentation des méthodes IHasName
    const std::string& getName() const override
    {
        return innerVar_->name();
    }

    // Méthode spécifique à MipVariable
    virtual bool isInteger() const
    {
        return innerVar_->integer();
    }

    // Accès direct au type interne
    const InnerSolverVariable* getInnerVariable() const
    {
        return innerVar_;
    }

    InnerSolverVariable* getInnerVariable()
    {
        return innerVar_;
    }

private:
    InnerSolverVariable* innerVar_;
};

} // namespace Antares::Optimisation::LinearProblemApi
