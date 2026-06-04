# Extending CSR Post-Processing to Modeler (Hybrid) Areas

## Context

The **adequacy patch** (Antares v8.3+) applies two post-processing steps after the main weekly LP is solved:

**LMR (Local Matching Rule):**  
For each type-2 area A, computes `DENS(A,h) = max(0, ENS_init + net_position_type2_init)`
analytically from the main LP solution — no second LP solve.  
Implementation: `calculateAreaFlowBalance()` in
`src/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.cpp:38`,
called from `CurtailmentSharingPostProcessCmd::calculateDensNewAndTotalLmrViolation()`
in `src/solver/optimisation/post_process_commands.cpp:249`.

**CSR (Curtailment Sharing Rule):**  
For each hour where `Σ_A ENS(A,h) > threshold`
(`post_process_commands.cpp:292` `identifyHoursForCurtailmentSharing`), solves a small hourly
QP over the type-2 subnetwork with decision variables `{ENS(A), Spillage(A), Flow(A,B)}`
(`post_process_commands.cpp:239`, `HourlyCSRProblem::run()`).

The QP has three structural constraints per area, with LHS built in
`src/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.cpp`
and RHS set in
`src/solver/optimisation/adequacy_patch_csr/construct_problem_constraints_RHS.cpp`:

| Constraint | LHS function | RHS function | Current RHS formula |
|---|---|---|---|
| Area balance | `setNodeBalanceConstraints` (`:42`) | `setRHSnodeBalanceValue` (`:32`) | `ENS_init + net_pos_type2_init − Spillage_init` |
| Fictitious load (`Spillage ≤ ?`) | `setFictitiousLoadConstraints` (`:92`) | `setRHSfictitiousLoadValue` (`:59`) | `STt − (1−BT)·STmint + BH·(Ht + STS_net) + BF·(Ft−Lt)` |
| Max ENS (`ENS ≤ ?`) | `setMaxEnsLoadConstraints` (`:112`) | `setRHSMaxEnsLoadValue` (`:154`) | `ConsommationsAbattues + max(0, AllMustRunGeneration) + ε` |

## Problem

The **Modeler** (GEMS) allows custom components to connect to legacy areas. A component with an
area connection modifies three LP constraints in the main weekly LP via
`ComponentToAreaConnectionFiller::addConstraints()`
(`src/solver/optimisation/ComponentToAreaConnectionFiller.cpp:183`):

```
inject_to_balance       →  NumeroDeContrainteDesBilansPays
spillage_bound          →  NumeroDeContraintePourEviterLesChargesFictives
unsupplied_energy_bound →  NumeroDeContraintePourBornerLaDefaillance
```

After `ComponentToAreaConnectionFiller` runs, the fictitious load and max ENS constraints become:

```
Spillage ≤  legacy_spillage_bound  +  eval(spillage_bound, h)
ENS      ≤  legacy_ens_bound       +  eval(unsupplied_energy_bound, h)
```

The CSR post-processing ignores these Modeler contributions entirely, producing incorrect bounds for hybrid areas.

## Solution

Add `eval(spillage_bound, h)` and `eval(unsupplied_energy_bound, h)` to the CSR RHS computations, without touching the existing legacy formulas. This preserves the BT/BH/BF flag semantics and introduces no behavioral change for legacy-only areas.

### Step 1 — Pre-evaluate port-field expressions after the main LP solve

`OptimEntityContainer` (needed to map GEMS variable indices to LP columns) exists only inside
`runWeeklyOptimization` in `opt_appel_solveur_lineaire.cpp`. The values must therefore be
computed and stored in `PROBLEME_HEBDO` while the LP is still alive, immediately after
`ORTOOLS_Simplexe` succeeds:

```cpp
// In runWeeklyOptimization, after ORTOOLS_Simplexe succeeds
if (problemeHebdo->modelerData) {
    const auto& solverVars = ortoolsProblem.getVariables();
    for (const auto& component : problemeHebdo->modelerData->system->Components()) {
        for (const auto& [portId, areaId] : component.portToAreaConnections()) {
            const unsigned areaIndex = areaIndices_.at(areaId);
            const auto& portAreaConn = component.findPort(portId, "").Type().areaConnection();
            if (!portAreaConn) continue;

            ReadLinearExpressionVisitor visitor(optimEntityContainer, fillCtx, component,
                                               problemeHebdo->modelerData->dataSeries.get(),
                                               problemeHebdo->modelerData->scenarioGroupRepository);

            auto evalExpr = [&](const std::string& fieldName) -> std::vector<double> {
                if (fieldName.empty()) return std::vector<double>(fillCtx.getLocalNumberOfTimeSteps(), 0.);
                auto expr = visitor.visitMergeDuplicates(component.nodeAtPortField(portId, fieldName))
                                   .expandToSize(fillCtx.getLocalNumberOfTimeSteps());
                std::vector<double> vals(fillCtx.getLocalNumberOfTimeSteps());
                for (unsigned h = 0; h < fillCtx.getLocalNumberOfTimeSteps(); ++h) {
                    vals[h] = expr[h].constant();
                    for (const auto& [colIdx, coef] : expr[h])
                        vals[h] += coef * solverVars.at(colIdx)->solutionValue();
                }
                return vals;
            };

            const auto spillVals = evalExpr(portAreaConn->spillage_bound);
            const auto ensVals   = evalExpr(portAreaConn->unsupplied_energy_bound);

            for (unsigned h = 0; h < fillCtx.getLocalNumberOfTimeSteps(); ++h) {
                problemeHebdo->ModelerSpillageContrib[h][areaIndex] += spillVals[h];
                problemeHebdo->ModelerEnsContrib[h][areaIndex]      += ensVals[h];
            }
        }
    }
}
```

`ModelerSpillageContrib[h][area]` and `ModelerEnsContrib[h][area]` are new fields to be added
to `PROBLEME_HEBDO`, initialised to zero at the start of each weekly optimisation.

### Step 2 — Use pre-computed values in CSR RHS functions

In `construct_problem_constraints_RHS.cpp`, at the end of each per-area RHS computation:

```cpp
// setRHSfictitiousLoadValue — after computing rhs from legacy formula:
if (problemeHebdo_->modelerData)
    rhs += problemeHebdo_->ModelerSpillageContrib[triggeredHour][Area];
problemeAResoudre_.SecondMembre[Cnt] = rhs;

// setRHSMaxEnsLoadValue — after computing load from legacy formula:
if (problemeHebdo_->modelerData)
    load += problemeHebdo_->ModelerEnsContrib[triggeredHour][Area];
if (load >= 0.)
    SecondMembre[Cnt] = load + 1e-5;
```

## What does not change

- **DENS computation** (`adq_patch_curtailment_sharing.cpp:38`): DENS is based on `ENS_init + net_position_type2_init`, which already reflects the Modeler's contribution via the main LP solution. No change needed.
- **CSR area balance RHS** (`construct_problem_constraints_RHS.cpp:32`): same reasoning — already correct through the observable triple.
- **Legacy CSR formulas** (BT/BH/BF terms): untouched.

## Files to modify

| File | Change |
|---|---|
| `sim_structure_probleme_economique.h` | Add `ModelerSpillageContrib` and `ModelerEnsContrib` fields to `PROBLEME_HEBDO` |
| `sim_alloc_probleme_hebdo.cpp` | Allocate and zero-initialise the new fields |
| `opt_appel_solveur_lineaire.cpp` | Add Step 1 evaluation after `ORTOOLS_Simplexe` succeeds |
| `adequacy_patch_csr/construct_problem_constraints_RHS.cpp` | Add Step 2 additions in `setRHSfictitiousLoadValue` and `setRHSMaxEnsLoadValue` |
