#pragma once
#include "new_constraint_builder.h"

struct ShortTermStorageLevelData
{
    std::vector<int>& ShortTermStorageLevelConstraint;
    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};

class ShortTermStorageLevel : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pdt, int pays, std::shared_ptr<ShortTermStorageLevelData> data);
};
