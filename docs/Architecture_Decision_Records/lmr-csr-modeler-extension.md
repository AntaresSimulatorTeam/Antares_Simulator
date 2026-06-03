# LMR/CSR Post-Processing Extension to Modeler (Hybrid) Areas

## Context and Motivations

### Background: LMR and CSR post-processing

The **adequacy patch** (available since Antares v8.3) is an optional feature that enforces
fairness rules on how Energy Not Served (ENS) is distributed across areas when the network
experiences a shortage. It operates through two sequential post-processing steps applied after
the main weekly LP is solved:

**Local Matching Rule (LMR):**  
Computes, for each type-2 area A (physical area inside the adequacy patch), the quantity
`DENS(A)` — the domestic ENS, i.e. the ENS that area A would have in isolation.  
`DENS(A) = max(0, ENS_init(A) + net_position_type2_init(A))`  
where `net_position_type2_init(A)` is the sum of flows on type-2/type-2 links only.  
This quantity serves as an upper bound on ENS in the subsequent CSR step, enforcing that
*an area that can serve its own load locally should never export power while experiencing ENS*.

**Curtailment Sharing Rule (CSR):**  
For each hour where `Σ_A ENS(A) > threshold`, solves a small hourly quadratic program over
the type-2 subnetwork. Decision variables are `{ENS(A), Spillage(A), Flow(A,B)}`.  
Objective: `min Σ_A ENS(A)² / DENS(A)` (+ small hurdle-cost terms).  
The core structural constraint is the **area energy balance** (energy conservation):

```
ENS(A)  +  net_position_type2(A)  −  Spillage(A)
    =  ENS_init(A)  +  net_position_type2_init(A)  −  Spillage_init(A)
```

Two auxiliary constraints bound the search space:

| Constraint | Formula | Implementation file |
|---|---|---|
| Fictitious load (`Spillage ≤ ?`) | `STt − (1−BT)·STmint + BH·Ht + BF·(Ft−Lt) + BH·STS_net` | `construct_problem_constraints_RHS.cpp:59–152` |
| Max ENS (`ENS ≤ ?`) | `ConsommationsAbattues + max(0, AllMustRunGeneration) + ε` | `construct_problem_constraints_RHS.cpp:154–186` |

### The problem: Modeler (hybrid) objects are invisible to LMR/CSR

The **Modeler** (GEMS) allows users to define custom physics components that connect to
standard Antares areas through *area connections* (see
`from-GEMS-to-legacy-linear-problem.md`). A Modeler component connected to area A can:

1. Inject or absorb energy into the area balance via the `inject_to_balance` port field.
2. Expand the spillage upper bound via the `spillage_bound` port field.
3. Expand the ENS upper bound via the `unsupplied_energy_bound` port field.

In the main weekly LP, `ComponentToAreaConnectionFiller::addConstraints()` translates these
port-field expressions into LP constraint modifications (variable terms added with negative
coefficients; constants added positively to bounds). This is documented in
`from-GEMS-to-legacy-linear-problem.md`.

**The CSR post-processing is currently blind to these contributions.** Its auxiliary bound
formulas (`setRHSfictitiousLoadValue`, `setRHSMaxEnsLoadValue`) hard-code specific legacy
object types (thermal paliers, hydro, STS withdrawal, generation flags) and completely ignore
any Modeler component connected to the area. This produces incorrect CSR bounds — and
therefore incorrect ENS redistribution — whenever a hybrid (area + Modeler) configuration
is in use.

### Objectives

This document describes the two-step strategy to resolve this:

1. **Step 1 — Equivalent reformulation for legacy objects**: replace the
   generation-structure-specific bound formulas with formulas derived purely from the
   observable triple `{ENS_init, Spillage_init, all link flows}` via the energy conservation
   equation. The result is mathematically equivalent for legacy-only areas and is
   structure-agnostic (does not enumerate object types).

2. **Step 2 — Extension for Modeler objects**: add Modeler-component contributions to the
   reformulated bounds by evaluating each component's `spillage_bound` and
   `unsupplied_energy_bound` port-field expressions at the main LP solution values,
   following the same pattern as `ComponentToAreaConnectionFiller`.

---

## Step 1 — Equivalent Reformulation for Legacy Objects

### The energy conservation identity

After the main weekly LP is solved, the full energy balance at each area A holds for every
hour h:

```
Gen_init(A,h)  +  net_position_ALL_init(A,h)  +  ENS_init(A,h)  −  Spillage_init(A,h)
    =  Load(A,h)
```

where `net_position_ALL_init(A,h)` is the algebraic sum of **all** link flows at area A
(both type-1/type-2 and type-2/type-2 links), and `Load(A,h) = ConsommationsAbattues(A,h) +
AllMustRunGeneration(A,h)`.

Rearranging:

```
Gen_init(A,h)  =  Load(A,h)  −  net_position_ALL_init(A,h)  −  ENS_init(A,h)  +  Spillage_init(A,h)
```

This recovers total generation at the node purely from `{ENS_init, Spillage_init, all link
flows, Load}` — **without reading individual thermal paliers, hydro, or STS values**.

Computing `net_position_ALL_init(A,h)`:

```cpp
double netPosAll = 0.;
int Interco = problemeHebdo_->IndexDebutIntercoOrigine[Area];
while (Interco >= 0) {
    netPosAll -= problemeHebdo_->ValeursDeNTC[hour].ValeurDuFlux[Interco];
    Interco = problemeHebdo_->IndexSuivantIntercoOrigine[Interco];
}
Interco = problemeHebdo_->IndexDebutIntercoExtremite[Area];
while (Interco >= 0) {
    netPosAll += problemeHebdo_->ValeursDeNTC[hour].ValeurDuFlux[Interco];
    Interco = problemeHebdo_->IndexSuivantIntercoExtremite[Interco];
}
```

Note: the existing `calculateAreaFlowBalance()` (`adq_patch_curtailment_sharing.cpp:38`) only
sums type-2/type-2 links (for DENS). The new computation must sum **all** link types.

### New fictitious load bound

The fictitious load constraint prevents CSR from claiming spillage that is not physically
backed by generation. Since CSR does **not** re-dispatch generation (only type-2 flows, ENS,
and Spillage are decision variables), the physical maximum spillage at area A is exactly
`Gen_init(A)`:

```
Spillage_bound_legacy(A,h)  =  Spillage_init(A,h)  −  ENS_init(A,h)
                              −  net_position_ALL_init(A,h)  +  Load(A,h)
```

This single formula replaces the entire flag-driven computation
`STt − (1−BT)·STmint + BH·Ht + BF·(Ft−Lt) + BH·STS_net`.

**Equivalence for legacy objects**: the current formula approximates `Gen_init` with
flag-adjusted components. Since in CSR generation is fixed (Pmin constraints are not active
— Pmin limits how much generation can be *reduced*, not how much can be *spilled*), the full
`Gen_init` is the tightest valid upper bound. The new formula is therefore equivalent or
slightly less restrictive, which is correct.

### Max ENS bound (unchanged)

The current formula:

```
ENS_bound(A,h)  =  ConsommationsAbattues(A,h)  +  max(0, AllMustRunGeneration(A,h))  +  ε
```

is already concise, uses only input parameters (not generation dispatch details), and holds
under energy conservation for legacy-only areas. No change is needed in Step 1.

### CSR area balance RHS (unchanged)

```
RHS_balance(A,h)  =  ENS_init(A,h)  +  net_position_type2_init(A,h)  −  Spillage_init(A,h)
```

This is already expressed in the observable triple and is not affected by Modeler components
(whose energy injection is absorbed into ENS_init/Spillage_init through the main LP solution).
No change is needed.

---

## Step 2 — Adaptation for Modeler (Hybrid) Objects

### How Modeler objects enter the main LP bounds

In the main LP, `ComponentToAreaConnectionFiller::addConstraints()`
(`ComponentToAreaConnectionFiller.cpp:183–218`) loops over every component and its
port-to-area connections. For each connection it calls `addPortContributionToLinearPb()` on
three port fields, modifying three LP constraints:

```
inject_to_balance       →  NumeroDeContrainteDesBilansPays
spillage_bound          →  NumeroDeContraintePourEviterLesChargesFictives
unsupplied_energy_bound →  NumeroDeContraintePourBornerLaDefaillance
```

Each port-field expression is evaluated into a `TimeDependentLinearExpression` via
`ReadLinearExpressionVisitor`. The expression is added to the constraint with the convention
(from `from-GEMS-to-legacy-linear-problem.md`):
- Variable terms → negative coefficient on the LHS
- Constant terms → positive addition to both bounds

So after `ComponentToAreaConnectionFiller` runs, the fictitious load constraint is:

```
Spillage  −  Gen_legacy_vars  −  Σ_Modeler coef_i·var_i  ≤  Fatal  +  Σ_Modeler constant_j
```

i.e. `Spillage ≤ Gen_legacy + eval(spillage_bound expression)`.

### Why Step 1 alone is insufficient for hybrid areas

The Step 1 formula recovers `Gen_init_total = Gen_legacy + Modeler_inject` from conservation.
But the correct spillage bound is `Gen_legacy + eval(spillage_bound)`, which differs from
`Gen_init_total` by `eval(spillage_bound) − eval(inject_to_balance)`.

This difference is non-zero whenever the Modeler component's spillage absorption capacity
(`spillage_bound`) differs from its energy injection into the balance
(`inject_to_balance`) — which is the general case (e.g. a battery with remaining injection
capacity, or a flexible load that reduces effective demand).

### Corrected CSR bound formulas for hybrid areas

**Fictitious load (Spillage upper bound):**

```
Spillage_bound(A,h)  =  [Spillage_init  −  ENS_init  −  net_position_ALL_init  +  Load](A,h)
                       +  Σ_{components at A}  [eval(spillage_bound, h)  −  eval(inject_to_balance, h)]
```

Equivalently, separating the two contributions:

```
Spillage_bound(A,h)  =  Gen_legacy_init(A,h)  +  Σ_{components at A}  eval(spillage_bound, h)
```

where `Gen_legacy_init = Gen_init_total − Σ eval(inject_to_balance)`.

**Max ENS (ENS upper bound):**

```
ENS_bound(A,h)  =  ConsommationsAbattues(A,h)  +  max(0, AllMustRunGeneration(A,h))  +  ε
                +  Σ_{components at A}  eval(unsupplied_energy_bound, h)
```

**CSR area balance RHS**: unchanged (see Step 1).

### Sanity check — canonical Modeler types

| Component type | `inject_to_balance` | `spillage_bound` | Net correction `spill − inject` | Physical interpretation |
|---|---|---|---|---|
| Pure generator (`var_1`) | `+var_1` | `+var_1` | 0 | Conservation already captures it |
| Flexible load (`load_var`) | `−load_var` | `0` | `+load_var` | Reduced demand frees surplus → bound increases |
| Battery (partial charge) | `withdrawal − injection` | `inj_cap − injection` | `inj_cap − withdrawal` | Remaining injection capacity available for spillage |
| RES curtailment (`curtail_var`) | `+P_max − curtail_var` | `+curtail_var` | `2·curtail_var − P_max` | Curtailed energy available as spillage |

### Implementation pattern

The implementation of Step 2 follows the same architecture as
`ComponentToAreaConnectionFiller`. In `setRHSfictitiousLoadValue()` and
`setRHSMaxEnsLoadValue()`, after computing the legacy-based RHS, add a loop:

```cpp
// Pseudocode — mirrors ComponentToAreaConnectionFiller::addConstraints()
if (problemeHebdo_->modelerData) {
    for (const auto& component : modelerData->system->Components()) {
        for (auto [portId, areaId] : component.portToAreaConnections()) {
            if (areaIndices.at(areaId) != Area) continue;

            const auto& portType = component.findPort(portId).Type().areaConnection();

            // Evaluate port-field expressions at main LP solution for hour h
            double injectVal = evaluateAtSolution(
                portType->inject_to_balance, component, hour, solution);
            double spillVal  = evaluateAtSolution(
                portType->spillage_bound, component, hour, solution);
            double ensVal    = evaluateAtSolution(
                portType->unsupplied_energy_bound, component, hour, solution);

            rhs_fictitious_load += spillVal - injectVal;
            rhs_max_ens         += ensVal;
        }
    }
}
```

`evaluateAtSolution()` reads the variable values from the LP solution via
`OptimEntityContainer` (the same container used by `ComponentToAreaConnectionFiller` to
build the LP), then evaluates the `TimeDependentLinearExpression` at hour h:

```
eval(expr, h)  =  expr[h].constant()  +  Σ_i  expr[h].coefficient(i) * solution[varIndex(i)]
```

---

## Summary

```
After main LP solve:
  ┌──────────────────────────────────────────────────────────────────┐
  │  Observable triple for each type-2 area A, hour h:              │
  │    ENS_init(A,h)   Spillage_init(A,h)   ValeurDuFlux (all links) │
  └───────────────────────────┬──────────────────────────────────────┘
                               │  energy conservation
                               ▼
  Gen_init_total(A,h) = Load(A,h) − net_position_ALL_init(A,h)
                       − ENS_init(A,h) + Spillage_init(A,h)
                               │
              ┌────────────────┴────────────────────────┐
              │ Step 1 (legacy only)                     │ Step 2 (add for each Modeler component at A)
              ▼                                          ▼
  Spillage_bound = Gen_init_total              Spillage_bound += eval(spillage_bound)
                                                              −  eval(inject_to_balance)
  ENS_bound = Load + max(0, MustRun)           ENS_bound      += eval(unsupplied_energy_bound)

  CSR area balance RHS = ENS_init + net_position_type2_init − Spillage_init  [no change]
  DENS(A) = max(0, ENS_init + net_position_type2_init)                       [no change]
```

### Files to modify

| File | Change |
|---|---|
| `adequacy_patch_csr/construct_problem_constraints_RHS.cpp` | Replace `setRHSfictitiousLoadValue()` with the Step 1 formula; add Modeler loop (Step 2) |
| `adequacy_patch_csr/construct_problem_constraints_RHS.cpp` | Add Modeler loop to `setRHSMaxEnsLoadValue()` (Step 2) |
| `adequacy_patch_csr/hourly_csr_problem.h` | Add `modelerData` pointer and solution accessor to `HourlyCSRProblem` if not already available |

The DENS computation (`calculateAreaFlowBalance` in `adq_patch_curtailment_sharing.cpp`) and
the CSR area balance RHS (`setRHSnodeBalanceValue`) require **no changes**.
