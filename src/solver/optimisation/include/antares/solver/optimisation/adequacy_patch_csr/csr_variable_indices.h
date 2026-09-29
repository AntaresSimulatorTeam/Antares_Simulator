// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

/// \brief Variable indices of one hourly CSR (curtailment sharing) problem.
///
/// The adequacy patch's CSR step builds a small standalone optimization problem for a single
/// hour. Only five kinds of variables ever appear in it: unsupplied energy and spillage of the
/// areas inside the patch, and the (algebraic, positive direct, positive indirect) flows of the
/// links between two areas inside the patch.
///
/// HourlyCSRProblem owns an instance of this struct instead of writing into
/// PROBLEME_HEBDO::CorrespondanceVarNativesVarOptim: the weekly problem's correspondence table
/// is then never touched by the CSR machinery, and later readers (e.g. the simulation table
/// dump) keep resolving the weekly variables against the weekly problem's own numbering.
///
/// Entries are indexed by the global area / interconnection index. Areas and links outside the
/// adequacy patch are never variables of the CSR problem: their entries keep the -1 sentinel,
/// which ConstraintBuilder::AddVariable skips.
struct CsrVariableIndices
{
    std::vector<int> unsuppliedEnergy;     //!< ENS variable of each area
    std::vector<int> spillage;             //!< spilled energy variable of each area
    std::vector<int> directFlow;           //!< algebraic flow variable of each link
    std::vector<int> positiveDirectFlow;   //!< positive direct flow variable of each link
    std::vector<int> positiveIndirectFlow; //!< positive indirect flow variable of each link
};
