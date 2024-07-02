/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_H__
#define __ANTARES_LIBS_STUDY_PARTS_H__

// Load
#include "load/container.h"

// Solar
#include "solar/container.h"
#include "solar/prepro.h"

// Hydro
#include "hydro/container.h"
#include "hydro/hydromaxtimeseriesreader.h"
#include "hydro/prepro.h"
#include "hydro/series.h"

// Wind
#include "wind/container.h"
#include "wind/prepro.h"

// Thermal
#include "thermal/cluster.h"
#include "thermal/container.h"
#include "thermal/defines.h"

// Renewable
#include "renewable/cluster.h"
#include "renewable/container.h"
#include "renewable/defines.h"

// Short-term storage
#include "short-term-storage/container.h"

#endif // __ANTARES_LIBS_STUDY_PARTS_H__
