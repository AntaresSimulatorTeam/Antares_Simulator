// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "i_table_writer.h"

namespace Antares::Writer
{
// gp : here we return a shared ptr. Why not rather a unique ptr ?
ITableWriter::Ptr makeTableWriter(bool parquetFormatRequired);

void makeTableWriter();

} // namespace Antares::Writer
