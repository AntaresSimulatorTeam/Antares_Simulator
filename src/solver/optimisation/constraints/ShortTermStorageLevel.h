#pragma once
#include "ConstraintBuilder.h"

struct ShortTermStorageLevelData
{
    std::vector<int>& ShortTermStorageLevelConstraint;
    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};

class ShortTermStorageLevel : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    void add(int pdt, int pays, std::shared_ptr<ShortTermStorageLevelData> data);
};
