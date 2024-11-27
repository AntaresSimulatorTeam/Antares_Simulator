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

#pragma once
#include "ConstraintBuilder.h"

class ShortTermStorageCostVariation: public ConstraintFactory
{
public:
    ShortTermStorageCostVariation(ConstraintBuilder& builder, ShortTermStorageData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    virtual void add(unsigned int pdt, unsigned int pays) = 0;

    ShortTermStorageData& data;

protected:
    virtual void buildConstraint(int index) = 0;

    virtual bool IsConstraintEnabled(const ShortTermStorage::PROPERTIES& properties) = 0;

    virtual int& TargetConstraintIndex(int pdt, int index) = 0;

    void addStorageConstraint(const std::string& constraintName, int pdt, int pays);
};

class ShortTermStorageCostVariationInjectionBackward: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const ShortTermStorage::PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};

class ShortTermStorageCostVariationInjectionForward: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const ShortTermStorage::PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};

class ShortTermStorageCostVariationWithdrawalBackward: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const ShortTermStorage::PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};

class ShortTermStorageCostVariationWithdrawalForward: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const ShortTermStorage::PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};
