// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

    virtual bool IsConstraintEnabled(const PROPERTIES& properties) = 0;

    virtual int& TargetConstraintIndex(int pdt, int index) = 0;

    void addStorageConstraint(const std::string& constraintName, int pdt, int pays);
};

class ShortTermStorageCostVariationInjectionBackward final: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};

class ShortTermStorageCostVariationInjectionForward final: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};

class ShortTermStorageCostVariationWithdrawalBackward final: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};

class ShortTermStorageCostVariationWithdrawalForward final: ShortTermStorageCostVariation
{
public:
    using ShortTermStorageCostVariation::ShortTermStorageCostVariation;

    void add(unsigned int pdt, unsigned int pays) override;

    bool IsConstraintEnabled(const PROPERTIES& properties) override;

    int& TargetConstraintIndex(int pdt, int index) override;

    void buildConstraint(int index) override;
};
