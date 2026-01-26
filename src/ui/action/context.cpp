// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <action/context.h>

using namespace Yuni;

namespace Antares::Action
{
Context::Context(Data::Study::Ptr targetStudy, const size_t layer):
    study(targetStudy),
    extStudy(nullptr),
    layerID(layer),
    area(nullptr),
    cluster(nullptr),
    link(nullptr),
    originalPlant(nullptr),
    constraint(nullptr)
{
}

void Context::reset()
{
    area = nullptr;
    cluster = nullptr;
    link = nullptr;
    constraint = nullptr;

    view.clear();
    property.clear();
    areaNameMapping.clear();
    areaLowerNameMapping.clear();
}

} // namespace Antares::Action
