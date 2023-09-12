#pragma once
#include "constraint_builder.h"

struct ShortTermStorageLevelData
{
    std::vector<int>& ShortTermStorageLevelConstraint;
    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};

class ShortTermStorageLevel : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int pays, ShortTermStorageLevelData& data);
};
