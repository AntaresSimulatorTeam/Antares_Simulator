// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "i_table_writer.h"
#include "table_format.h"

namespace Antares::Writer
{

ITableWriter::Ptr makeTableWriter(TableFormat fmt);

} // namespace Antares::Writer
