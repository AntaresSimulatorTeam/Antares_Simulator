/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"

#include <memory>
#include <stdexcept>

class CumulationConstraint
{
public:
    virtual void build(unsigned int index) const = 0;

    virtual std::string name() const = 0;
    virtual ~CumulationConstraint() = default;
};

class WithdrawalCumulationConstraint: public CumulationConstraint
{
public:
    WithdrawalCumulationConstraint(ConstraintBuilder& builder):
        builder(builder)
    {
    }

    void build(unsigned int index) const override
    {
        builder.ShortTermStorageWithdrawal(index, 1.0);
    }

    std::string name() const override
    {
        return "WithdrawalSum";
    }

    ~WithdrawalCumulationConstraint() override = default;

    ConstraintBuilder& builder;
};

class InjectionCumulationConstraint: public CumulationConstraint
{
public:
    InjectionCumulationConstraint(ConstraintBuilder& builder):
        builder(builder)
    {
    }

    void build(unsigned int index) const override
    {
        builder.ShortTermStorageInjection(index, 1.0);
    }

    std::string name() const override
    {
        return "InjectionSum";
    }

    ~InjectionCumulationConstraint() override = default;

    ConstraintBuilder& builder;
};

class NettingCumulationConstraint: public CumulationConstraint
{
public:
    NettingCumulationConstraint(
      ConstraintBuilder& builder,
      const ::ShortTermStorage::PROPERTIES& short_term_storage_properties):
        builder(builder),
        short_term_storage_properties(short_term_storage_properties)
    {
    }

    void build(unsigned int index) const override
    {
        builder.ShortTermStorageInjection(index, short_term_storage_properties.injectionEfficiency)
          .ShortTermStorageWithdrawal(index, -short_term_storage_properties.withdrawalEfficiency);
    }

    std::string name() const override
    {
        return "NettingSum";
    }

    ~NettingCumulationConstraint() override = default;

    ConstraintBuilder& builder;
    const ShortTermStorage::PROPERTIES& short_term_storage_properties;
};

std::unique_ptr<CumulationConstraint> cumulationConstraintFactory(
  const std::string& variable,
  ConstraintBuilder& builder,
  const ShortTermStorage::PROPERTIES& short_term_storage_properties)
{
    if (variable == "withdrawal")
    {
        return std::make_unique<WithdrawalCumulationConstraint>(builder);
    }
    else if (variable == "injection")
    {
        return std::make_unique<InjectionCumulationConstraint>(builder);
    }
    else if (variable == "netting")
    {
        return std::make_unique<NettingCumulationConstraint>(builder,
                                                             short_term_storage_properties);
    }
    throw std::invalid_argument("Invalid cumulation constraint type");
}

char ConvertSense(const std::string& sense)
{
    if (sense == "greater")
    {
        return '>';
    }
    else if (sense == "less")
    {
        return '<';
    }
    else
    {
        return '=';
    }
}

void ShortTermStorageCumulation::add(int pays)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    for (const auto& storage: data.ShortTermStorage[pays])
    {
        for (const auto& additionalConstraints: storage.additionalConstraints)
        {
            // sum (var[h]) sign rhs, h in list provided by user where:
            // var = injection for InjectionCumulationConstraint
            // var = withdrawal for WithdrawalCumulationConstraint
            // var = injectionEfficiency * injection - withdrawalEfficiency * withdrawal for Netting
            auto cumulationConstraint = cumulationConstraintFactory(additionalConstraints.variable,
                                                                    builder,
                                                                    storage);
            for (const auto& [hours, globalIndex, localIndex]: additionalConstraints.constraints)
            {
                namer.ShortTermStorageCumulation(cumulationConstraint->name(),
                                                 builder.data.nombreDeContraintes,
                                                 storage.name,
                                                 additionalConstraints.name + "_"
                                                   + std::to_string(localIndex));
                const auto index = storage.clusterGlobalIndex;
                data.CorrespondanceCntNativesCntOptimHebdomadaires
                  .ShortTermStorageCumulation[globalIndex]
                  = builder.data.nombreDeContraintes;

                for (const auto& hour: hours)
                {
                    builder.updateHourWithinWeek(hour - 1);
                    cumulationConstraint->build(index);
                }
                builder.SetOperator(ConvertSense(additionalConstraints.operatorType)).build();
            }
        }
    }
}
