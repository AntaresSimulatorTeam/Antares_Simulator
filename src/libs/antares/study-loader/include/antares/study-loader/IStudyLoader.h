
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares
{

namespace Data
{
class Study;
}

/**
 * @class IStudyLoader
 * @brief The IStudyLoader class is an interface for loading studies.
 * @details It declares the load method.
 */
class IStudyLoader
{
public:
    virtual ~IStudyLoader() = default;
    /**
     * @brief The load method is used to load a study.
     * @return A shared pointer to a Study object.
     */
    [[nodiscard]] virtual std::unique_ptr<Antares::Data::Study> load() const = 0;
};
} // namespace Antares
