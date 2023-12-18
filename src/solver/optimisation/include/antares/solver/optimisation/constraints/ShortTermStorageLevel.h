#pragma once
#include "ConstraintBuilder.h"

struct ShortTermStorageLevelData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;

    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};

class ShortTermStorageLevel : private ConstraintFactory
{
public:
    ShortTermStorageLevel(ConstraintBuilder& builder,
                          ShortTermStorageLevelData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    void add(int pdt, int pays);

private:
    ShortTermStorageLevelData& data;
};
