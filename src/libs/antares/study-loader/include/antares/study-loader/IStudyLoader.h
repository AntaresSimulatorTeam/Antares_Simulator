
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares
{

namespace Data
{
class Study;

}

namespace Solver
{
class IResultWriter;

} // namespace Solver

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
     * @return A pair which is a std::unique_ptr<Data::Study> and Solver::IResultWriter::Ptr
     */
    [[nodiscard]] virtual std::pair<std::unique_ptr<Data::Study>,
                                    std::shared_ptr<Solver::IResultWriter>>
    load() const = 0;
};
} // namespace Antares
