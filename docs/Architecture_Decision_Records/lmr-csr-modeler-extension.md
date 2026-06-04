# Extending CSR Post-Processing to Modeler (Hybrid) Areas

## Context

The **adequacy patch** (Antares v8.3+) applies two post-processing steps after the main weekly LP is solved:

**LMR (Local Matching Rule):**  
For each type-2 area A, computes `DENS(A,h)` analytically from the main LP solution — no
second LP solve. The formula has two branches depending on the
`setToZeroOutsideInsideLinks` parameter (`adq_patch_curtailment_sharing.cpp:82-91`):

```
DENS = max(0, ENS_init + net_position_type2_init + flows(outside→A))   if setToZeroOutsideInsideLinks == false  (default)
DENS = max(0, ENS_init + net_position_type2_init)                      if setToZeroOutsideInsideLinks == true
```

where `flows(outside→A)` is the sum of imports from areas *outside* the patch (type-1→type-2
links, clamped to imports only, lines 58-59 & 74-75).  
Implementation: `calculateAreaFlowBalance()` in
`src/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.cpp:38`,
called from `CurtailmentSharingPostProcessCmd::calculateDensNewAndTotalLmrViolation()`
in `src/solver/optimisation/post_process_commands.cpp:249`.

**CSR (Curtailment Sharing Rule):**  
For each hour where `Σ_A ENS(A,h) > threshold`
(`post_process_commands.cpp:292` `identifyHoursForCurtailmentSharing`), solves a small hourly
QP over the type-2 subnetwork. The decision variables are the area `ENS(A)` and `Spillage(A)`,
plus, per type-2/type-2 link, three flow variables (algebraic, positive-direct,
positive-indirect) — see `construct_problem_variables.cpp:78-100`. CSR is run via
`HourlyCSRProblem::run()` (`post_process_commands.cpp:245`).

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

`OptimEntityContainer` (needed to map GEMS variable indices to LP columns) is a local variable
of **`OPT_TryToCallSimplex`** (a `static` function in `opt_appel_solveur_lineaire.cpp:150`,
container constructed at line 179). It is destroyed when that function returns, so the
port-field values must be evaluated there, immediately after the solve succeeds (after the
`ORTOOLS_Simplexe` call at line 211 and the `ExistenceDUneSolution == OUI_SPX` check at line
221), and stored in `PROBLEME_HEBDO` for the CSR step to read later.

**Two-pass caveat.** `OPT_TryToCallSimplex` is invoked once per pass of
`runWeeklyOptimization` (`opt_optimisation_lineaire.cpp:128`), which runs **twice** per week:
`PREMIERE_OPTIMISATION` (line 281), then `DEUXIEME_OPTIMISATION` after the thermal heuristic
(line 298). The CSR consumes **final-pass** results (the legacy CSR formula reads
`ResultatsHoraires...ProductionThermique`, which hold 2nd-pass values). Therefore the storage
arrays must be **zeroed at the start of every `OPT_TryToCallSimplex` call** and filled with
`+=` (to sum multiple components on the same area *within* a pass). Re-zeroing each pass means
the final pass overwrites the first — never accumulating across passes.

```cpp
// In OPT_TryToCallSimplex, after ORTOOLS_Simplexe succeeds (ExistenceDUneSolution == OUI_SPX).
// Zero the two arrays for every area/hour at function entry (see "Two-pass caveat" above).
if (problemeHebdo->modelerData) {
    const auto areaIndices = associateIndicesToAreas(problemeHebdo); // map<string,unsigned>
    const auto& solverVars = optimEntityContainer.Problem().getVariables();
    const auto nbTs = fillCtx.getLocalNumberOfTimeSteps();
    for (const auto& component : problemeHebdo->modelerData->system->Components()) {
        for (const auto& [portId, areaId] : component.portToAreaConnections()) {
            // Guard exactly like ComponentToAreaConnectionFiller::addConstraints (lines 190-195):
            // the per-component connection is the guard; the field names come from the port type.
            if (!component.areaConnectionAtPort(portId)) continue;
            const auto& portAreaConn = component.findPort(portId, "").Type().areaConnection();
            if (!portAreaConn) continue;
            const unsigned areaIndex = areaIndices.at(areaId);

            ReadLinearExpressionVisitor visitor(optimEntityContainer, fillCtx, component,
                                               problemeHebdo->modelerData->dataSeries.get(),
                                               problemeHebdo->modelerData->scenarioGroupRepository);

            auto evalExpr = [&](const std::string& fieldName) -> std::vector<double> {
                if (fieldName.empty()) return std::vector<double>(nbTs, 0.);
                auto expr = visitor.visitMergeDuplicates(component.nodeAtPortField(portId, fieldName))
                                   .expandToSize(nbTs);
                std::vector<double> vals(nbTs);
                for (unsigned h = 0; h < nbTs; ++h) {
                    vals[h] = expr[h].constant();
                    for (const auto& [colIdx, coef] : expr[h])
                        vals[h] += coef * solverVars.at(colIdx)->solutionValue();
                }
                return vals;
            };

            const auto spillVals = evalExpr(portAreaConn->spillage_bound);
            const auto ensVals   = evalExpr(portAreaConn->unsupplied_energy_bound);

            for (unsigned h = 0; h < nbTs; ++h) {
                problemeHebdo->ModelerSpillageContrib[h][areaIndex] += spillVals[h];
                problemeHebdo->ModelerEnsContrib[h][areaIndex]      += ensVals[h];
            }
        }
    }
}
```

`ModelerSpillageContrib[h][area]` and `ModelerEnsContrib[h][area]` are new fields to be added
to `PROBLEME_HEBDO`. We evaluate the **raw** port-field expression (`const + Σ coef·sol`),
which equals the contribution `ComponentToAreaConnectionFiller` adds to the main-LP RHS
(variable terms → `-coef` on the LHS, constant → `+bound`); evaluated at the solution and
moved to the RHS, the two signs cancel, so adding the raw value to the CSR RHS in Step 2 is
correct.

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

- **DENS computation** (`adq_patch_curtailment_sharing.cpp:38`): DENS is built from `ENS_init`, link flows, and `Spillage_init` (see the two-branch formula above), all of which already reflect the Modeler's contribution via the main LP solution. No change needed.
- **CSR area balance RHS** (`construct_problem_constraints_RHS.cpp:32`): same reasoning — already correct through the observable triple.
- **Legacy CSR formulas** (BT/BH/BF terms): untouched.

## Files to modify

| File | Change |
|---|---|
| `sim_structure_probleme_economique.h` | Add `ModelerSpillageContrib` and `ModelerEnsContrib` fields to `PROBLEME_HEBDO` |
| `sim_alloc_probleme_hebdo.cpp` | Allocate the new fields |
| `opt_appel_solveur_lineaire.cpp` (`OPT_TryToCallSimplex`) | Zero the two arrays at function entry, then add the Step 1 evaluation after the solve succeeds (so the final pass overwrites the first — see the two-pass caveat) |
| `adequacy_patch_csr/construct_problem_constraints_RHS.cpp` | Add Step 2 additions in `setRHSfictitiousLoadValue` and `setRHSMaxEnsLoadValue` |
