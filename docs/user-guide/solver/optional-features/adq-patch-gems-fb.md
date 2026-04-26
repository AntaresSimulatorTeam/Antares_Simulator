# GEMS flow-based constraints in the adequacy-patch CSR

_Available since v9.3.11_

## Overview

When a study is both GEMS-enabled and uses the adequacy patch, the same GEMS system
(`input/system.yml` + `input/model-libraries/*.yml`) that drives the main optimization
problem can also contribute **additional rows and columns** to the Curtailment Sharing
Rule (CSR) quadratic sub-problem that is solved per triggered hour.

This lets you encode network constraints — such as Cross-Zonal Capacity (CZC) or
RAM limits from a Capacity Calculation Region (CCR) model — in one place and have
them applied consistently in both the full-horizon problem and the adequacy-patch
post-processing step.

## How it works

1. **Shared model definition.** The GEMS library defines a model with one or more
   constraints whose ids begin with `flow_based_constraint_` (or match a custom regex).
   Parameters may be time-dependent and/or scenario-dependent, read from
   `input/data-series/*.csv` time-series files.

2. **Variable mapping.** If a model variable is exposed through a port connected to an
   adequacy-patch area, it is mapped to the corresponding CSR column (ENS / flow
   variables). Variables with no area connection receive a new dedicated LP column.

3. **Per-hour injection.** For each CSR-triggered hour and Monte Carlo year the adapter
   evaluates the constraint expression — substituting parameter values for that
   (hour, scenario) pair — and injects the resulting linear row into the CSR matrix.

4. **RHS update.** The right-hand side is re-evaluated from the same expression and
   stored in the CSR `SecondMembre` array before the solver is called.

## Enabling the feature

In `settings/generaldata.ini`, section `[adequacy patch]`:

```ini
[adequacy patch]
include-adq-patch = true
enable-gems-fb-constraints = true
# optional — override the default constraint-id filter:
# gems-fb-constraint-filter = ^flow_based_constraint_
```

The study must already contain `input/system.yml` referencing at least one model library.

## Default constraint filter and why zone-balance constraints are excluded

### What the default regex matches

The default pattern `^flow_based_constraint_` matches any constraint whose id **starts
with** that literal string. It deliberately **does not** match:

- `ccr_exchange_balance` — the canonical example of a CCR zone-balance constraint
  (`Σ exchange = 0` across the CCR perimeter)
- any other structural balance identifiers (`zone_balance_*`, `net_position_*`, …)

The narrow prefix ensures that only explicitly-labelled flow-based constraints enter the
CSR, leaving all zone-balance and structural rows out by default.

### Why the CSR already enforces nodal energy balance

For every area marked **physicalAreaInsideAdqPatch**, `CsrAreaBalance::add()`
(`constraints/CsrAreaBalance.cpp:8–71`) emits one equality row:

```
ENS(A) + net_position(A) − spillage(A)
    = ENS_init(A) + net_position_init(A) − spillage_init(A)
```

The right-hand side is computed in `adq_patch_curtailment_sharing.cpp:148` and written to
`SecondMembre` in `construct_problem_constraints_RHS.cpp:32–57`.

This identity holds for **every** inside area simultaneously. Summing it over all inside
areas gives a system-wide net-position relation:

```
Σ_A net_position(A) = Σ_A net_position_init(A)
```

An additional GEMS constraint `Σ net_position(A) = 0` (or `≤ 0`) is therefore either:

- **Redundant** — if the CCR perimeter exactly matches the set of inside areas and flows,
  or
- **In conflict** — if the GEMS perimeter differs (e.g. it includes or excludes areas
  that the CSR does not).

### Failure modes when the filter is widened naively

1. **Redundant rows → rank-deficient constraint matrix.** Adding a row that is an exact
   linear combination of existing CSR rows produces a rank-deficient matrix. Most LP
   solvers presolve this away, but it widens dual degeneracy, slows the QP, and can
   produce unpredictable dual variables.

2. **Perimeter mismatch → infeasibility at scarcity hours.** The GEMS CCR model may
   include pseudo-areas (e.g. `ccr_flow_alegro`, `ccr_flow_frejus`) that appear in the
   GEMS zone-balance but are not Antares `physicalAreaInsideAdqPatch` areas. The CSR's
   implicit balance sums over a different set than the GEMS constraint. The QP can become
   infeasible at scarcity hours because the GEMS RHS is inconsistent with the per-area
   balances the CSR has already fixed.

3. **Re-constraining what the CSR is designed to redistribute.** The CSR quadratic
   objective is precisely designed to adjust net positions across inside areas to share
   curtailment equitably. Injecting a structural balance that freezes `Σ net_position = 0`
   defeats that redistribution — it either collapses the feasible set to a single point or
   renders it infeasible, making the CSR objective meaningless.

### Legitimate reasons to widen the filter

**Diagnostic mode — verify that the two balance formulations agree:**

```ini
gems-fb-constraint-filter = .
```

This matches every GEMS constraint, including zone-balance rows. Use on a small study to
check that GEMS and CSR produce identical RHS values. Remove or narrow before production
runs.

**Anchoring internal GEMS variables not bound to any area-connection:**

Some CCR models introduce purely-internal flow variables (e.g. `ccr_flow_alegro`,
`ccr_flow_frejus`) with no Antares area port. The adapter allocates these as extra LP
columns, and a GEMS constraint is the only way to couple them to the rest of the CSR
decision space. For such models the recommended override is:

```ini
gems-fb-constraint-filter = ^(flow_based_constraint_|ccr_exchange_balance)$
```

The inside-area redundancy this introduces is benign for solvers that presolve
(XPRESS, CPLEX, COIN, SCIP) — they detect the linearly-dependent row and drop it
silently. Verify with solver log level `debug` that no infeasible status appears.

### How to choose

If your GEMS CCR model connects exclusively to Antares `physicalAreaInsideAdqPatch`
areas, leave `gems-fb-constraint-filter` unset — the default `^flow_based_constraint_`
prefix is sufficient and the CSR's own area-balance rows handle the zone balance.

If your model contains purely-internal variables that appear only in zone-balance
constraints (no area port), add those constraint ids explicitly to the filter regex and
verify the solver log. If a scarcity hour triggers an infeasibility, the most likely
cause is a perimeter mismatch: check that every area referenced in the GEMS balance is
marked `physicalAreaInsideAdqPatch` in your study.

## Worked example — CCR flow-based constraints

### Library definition (`input/model-libraries/ccr-lib.yml`)

```yaml
library:
  id: ccr_lib
  description: CCR flow-based model
  port-types:
    - id: area_port
      description: Connection point to an adequacy-patch area
      fields:
        - id: ens
  models:
    - id: ccr_model
      description: Injects one RAM constraint per scenario into the CSR
      parameters:
        - id: ram_direct_fbc_0001
          time-dependent: false
          scenario-dependent: true
      variables:
        - id: flow_hub
          lower-bound: -1e20
          upper-bound: 1e20
          variable-type: continuous
      constraints:
        - id: flow_based_constraint_0001
          expression: flow_hub <= ram_direct_fbc_0001
      ports:
        - id: p_north
          type: area_port
      port-field-definitions:
        - port: p_north
          field: ens
          definition: flow_hub
```

### System definition (`input/system.yml`)

```yaml
system:
  id: my_ccr_system
  model-libraries: ccr_lib
  components:
    - id: ccr
      model: ccr_lib.ccr_model
      parameters:
        - id: ram_direct_fbc_0001
          time-dependent: false
          scenario-dependent: true
          value: ram_direct_fbc_0001   # dataset id in input/data-series/
      port-connections:
        - component-port: p_north
          area: north
```

### Time-series data (`input/data-series/ram_direct_fbc_0001.csv`)

Tab-separated, one column per scenario (tsNumber = MC year + 1):

```
500	350
```

MC year 0 reads column 1 → RAM = 500 MW; MC year 1 reads column 2 → RAM = 350 MW.

## INI key reference

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable-gems-fb-constraints` | bool | `false` | Inject matching GEMS constraints into the CSR. |
| `gems-fb-constraint-filter` | string | _(empty)_ | ECMAScript regex on constraint ids. Empty → `^flow_based_constraint_`. |

See [Adequacy-patch parameters](../04-parameters.md#adequacy-patch-parameters) for the
full parameter list.

## Constraint filter — avoiding double-counting

The default filter `^flow_based_constraint_` is deliberately narrow. For a full
explanation of why zone-balance constraints like `ccr_exchange_balance` must be
excluded, the failure modes when the filter is widened naively, and the two legitimate
reasons to override it, see
[Default constraint filter and why zone-balance constraints are excluded](#default-constraint-filter-and-why-zone-balance-constraints-are-excluded)
above.

Example — restrict injection to a specific CCR by prefix:

```ini
gems-fb-constraint-filter = ^flow_based_constraint_FR_BE_
```

## Scenario-dependent parameters

Parameters declared `scenario-dependent: true` in the GEMS model are read from a
multi-column CSV where column _n_ holds the values for MC year _n − 1_
(the adapter uses `tsNumber = mcYear + 1`, following the 1-based Antares convention).

This means different Monte Carlo years automatically receive different RAM values
without any extra configuration — the same mechanism used by all GEMS time-series.

## Limitations

- The CSR objective function is **not** modified; GEMS constraints affect feasibility
  and the admissible region only.
- The scenario-builder's TS-number mapping is not yet wired for GEMS parameters:
  the adapter currently uses `tsNumber = mcYear + 1` directly.
- Constraints that reference only internal variables (no area port) introduce extra
  LP columns. These columns contribute no objective cost; ensure the constraints are
  sufficiently tight to make them meaningful.