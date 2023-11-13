#pragma once
#include "ConstraintBuilder.h"

struct ShortTermStorageLevelData
{
    std::vector<int>& ShortTermStorageLevelConstraint;
    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};

class ShortTermStorageLevel : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pdt, int pays, std::shared_ptr<ShortTermStorageLevelData> data);
};
