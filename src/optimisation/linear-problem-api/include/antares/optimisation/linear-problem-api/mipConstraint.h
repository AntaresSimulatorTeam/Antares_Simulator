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
#include "mipVariable.h"

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{
class OrtoolsVariableWrapper; // Forward declaration
}

namespace Antares::Optimisation::LinearProblemApi
{
// Utilisation du namespace pour simplifier l'accès
using Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsVariableWrapper;

// Concept pour valider qu'un type peut être utilisé comme contrainte de solveur interne
template<typename T, typename V>
concept SolverConstraint = requires(T t, const T ct, const V* v) {
    { ct.lb() } -> std::convertible_to<double>;
    { ct.ub() } -> std::convertible_to<double>;
    { ct.name() } -> std::convertible_to<const std::string&>;
    { ct.GetCoefficient(v) } -> std::convertible_to<double>;
    t.SetLB(double{});
    t.SetUB(double{});
    t.SetBounds(double{}, double{});
    t.SetCoefficient(v, double{});
};

template<SolverTag SolverTagType>
class IMipConstraint: public IHasBounds, public IHasName
{
public:
    using VariableType = typename SolverTagType::VariableType;
    using ConstraintType = typename SolverTagType::ConstraintType;

    explicit IMipConstraint(ConstraintType* innerConstraint):
        innerConstraint_(innerConstraint)
    {
    }

    virtual ~IMipConstraint() = default;

    // Implémentation des méthodes IHasBounds
    void setLb(double lb) override
    {
        innerConstraint_->SetLB(lb);
    }

    void setUb(double ub) override
    {
        innerConstraint_->SetUB(ub);
    }

    void setBounds(double lb, double ub) override
    {
        innerConstraint_->SetBounds(lb, ub);
    }

    double getLb() const override
    {
        return innerConstraint_->lb();
    }

    double getUb() const override
    {
        return innerConstraint_->ub();
    }

    // Implémentation des méthodes IHasName
    const std::string& getName() const override
    {
        return innerConstraint_->name();
    }

    // Méthodes spécifiques à MipConstraint
    virtual void setCoefficient(IMipVariable<VariableType>* var, double coefficient)
    {
        if constexpr (std::is_same_v<VariableType, OrtoolsVariableWrapper>)
        {
            // Pour OrTools, extraire l'objet MPVariable du wrapper
            innerConstraint_->SetCoefficient(var->getInnerVariable()->getMPVariable(), coefficient);
        }
        else
        {
            // Pour les autres solveurs, utiliser directement l'objet interne
            innerConstraint_->SetCoefficient(var->getInnerVariable(), coefficient);
        }
    }

    virtual double getCoefficient(const IMipVariable<VariableType>* var) const
    {
        if constexpr (std::is_same_v<VariableType, OrtoolsVariableWrapper>)
        {
            // Pour OrTools, extraire l'objet MPVariable du wrapper
            return innerConstraint_->GetCoefficient(var->getInnerVariable()->getMPVariable());
        }
        else
        {
            // Pour les autres solveurs, utiliser directement l'objet interne
            return innerConstraint_->GetCoefficient(var->getInnerVariable());
        }
    }

    // Accès direct au type interne
    const ConstraintType* getInnerConstraint() const
    {
        return innerConstraint_;
    }

    ConstraintType* getInnerConstraint()
    {
        return innerConstraint_;
    }

private:
    ConstraintType* innerConstraint_;
};

} // namespace Antares::Optimisation::LinearProblemApi
