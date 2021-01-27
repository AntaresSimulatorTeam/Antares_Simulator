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
#ifndef __SOLVER_VARIABLE_ADEQUACY_DRAFT_H__
#define __SOLVER_VARIABLE_ADEQUACY_DRAFT_H__

#include "../variable.h"
#include "../area.h"
#include "lold_is.h"
#include "lold_cn.h"
#include "ens_is.h"
#include "ens_cn.h"
#include "lolp_is.h"
#include "lolp_cn.h"
#include "maxdepth_is.h"
#include "maxdepth_cn.h"
#include "minmarg_is.h"
#include "minmarg_cn.h"

#include "lold_is_system.h"
#include "lold_cn_system.h"
#include "lolp_is_system.h"
#include "lolp_cn_system.h"
#include "ens_is_system.h"
#include "ens_cn_system.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace AdequacyDraft
{
/*!
** \brief All variables for a single area (adequacy)
*/
typedef LOLD_IS<
  LOLD_CN<LOLP_IS<LOLP_CN<ENS_IS<ENS_CN<MinMarg_IS<MinMarg_CN<MaxDepth_IS<MaxDepth_CN<>>>>>>>>>>
  VariablesPerArea;

typedef LOLD_IS_System<LOLD_CN_System<
  LOLP_IS_System<LOLP_CN_System<ENS_IS_System<ENS_CN_System<Variable::Areas<VariablesPerArea>>>>>>>
  ItemList;

/*!
** \brief All variables for an simulation in Adequacy mode
*/
typedef Container::List<ItemList> AllVariables;

} // namespace AdequacyDraft
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ADEQUACY_DRAFT_H__
