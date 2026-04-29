// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/writer/i_writer.h"

namespace Antares::Solver
{

class NullResultWriter: public IResultWriter
{
public:
    ~NullResultWriter() override = default;
    void addEntryFromBuffer(const std::filesystem::path&, std::string&) override;
    void addEntryFromFile(const std::filesystem::path&, const std::filesystem::path&) override;
    void flush() override;
    bool needsTheJobQueue() const override;
    void finalize(bool) override;
};

} // namespace Antares::Solver
