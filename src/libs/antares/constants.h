/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_CONSTANTS_H__
#define __ANTARES_LIBS_CONSTANTS_H__

/*! Name of the app to use into logs */
#define LOG_APPLICATION_NAME "antares"
/*! Vendor */
#define LOG_APPLICATION_VENDOR "RTE"

/*! Days per year */
#define DAYS_PER_YEAR 365

/*! Hours per year */
#define HOURS_PER_YEAR 8760

/*! Enable / Disable the expansion mode */
#define STUDY_MODE_EXPANSION 0

/*! Max number of MC years */
constexpr uint MAX_NB_MC_YEARS = 50000;

/*! */
/* # define ANTARES_USE_GLOBAL_MAXIMUM_COST 0 */

enum AntaresConstants
{
    ant_k_area_name_max_length = 128,
    ant_k_cluster_name_max_length = 128,
    ant_k_constraint_name_max_length = 128,
};

#endif /* __ANTARES_LIBS_CONSTANTS_H__ */
