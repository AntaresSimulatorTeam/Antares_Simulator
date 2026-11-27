# Thermal cluster ramping model — input format spec

This document describes the on-disk input format for the ramping model introduced in
[PR #2684](https://github.com/AntaresSimulatorTeam/Antares_Simulator/pull/2684), based on the
implementation currently on `feature/ramping_model_scalian`.

## 1. Study-level activation switch

File: `settings/generaldata.ini`
Section: `[optimization]`

```ini
[optimization]
include-thermal-cluster-ramping = true
```

- Key: `include-thermal-cluster-ramping`
- Type: boolean (`true` / `false`)
- Default: `false`
- Source: `Parameters::include.thermal_ramping`
  (`src/libs/antares/study/include/antares/study/parameters.h:316`,
  parsed in `src/libs/antares/study/parameters.cpp:628`)

This is the **global gate**: when `false`, the per-cluster `ramping-enabled` and ramping
parameter keys described below are parsed but silently ignored (no `Ramping` struct is ever
attached to a cluster).

## 2. Per-cluster activation and parameters

File: `input/thermal/clusters/<area>/list.ini`
Section: `[<cluster name>]` (same section as all other thermal cluster properties)

```ini
[my cluster]
name = my cluster
group = other
unitcount = 1
nominalcapacity = 100
...
ramping-enabled = true
power-increase-cost = 12.5
power-decrease-cost = 8.0
max-upward-power-ramping-rate = 20
max-downward-power-ramping-rate = 25
```

### 2.1 `ramping-enabled`

- Type: boolean
- Default: absent/false → cluster has no ramping model (`cluster.ramping` stays `std::nullopt`)
- Only has an effect when the study-level `include-thermal-cluster-ramping = true`
  (`src/libs/antares/study/parts/thermal/cluster_list.cpp:358-371`). If the global switch is
  off, this key is read but has no effect.
- When `true` and the global switch is on, a `ThermalCluster::Ramping` object is created with
  all fields defaulted to `0`, then populated by the keys below as they are encountered.
- This key is parsed **before** the other properties are enumerated
  (`ThermalClusterLoadFromSection`), so the four numeric keys below can appear anywhere in the
  section and will still be picked up, but they must be declared in the same section for the
  cluster in question.

### 2.2 Numeric ramping parameters

All four are `double`, ignored (parsed but discarded, returns `true`) if `cluster.ramping` is
not set (i.e. `ramping-enabled` was absent/false, or the global switch is off):

| Key | Field | Unit | Default | Validity constraint |
|---|---|---|---|---|
| `power-increase-cost` | `Ramping::powerIncreaseCost` | €/MW | `0` | must be `>= 0` |
| `power-decrease-cost` | `Ramping::powerDecreaseCost` | €/MW | `0` | must be `>= 0` |
| `max-upward-power-ramping-rate` | `Ramping::maxUpwardPowerRampingRate` | MW/hour | none (internal value `0`, invalid) | **required**, must be `> 0` |
| `max-downward-power-ramping-rate` | `Ramping::maxDownwardPowerRampingRate` | MW/hour | none (internal value `0`, invalid) | **required**, must be `> 0` |

The two ramping rates have no usable default: the internal initial value `0` fails the
validity check (see 2.3), so they must be set explicitly to a strictly positive value.
Otherwise ramping is disabled for the cluster.

Source: `src/libs/antares/study/parts/thermal/cluster_list.cpp:292-313` (parsing),
`src/libs/antares/study/include/antares/study/parts/thermal/cluster.h:244-260` (struct
definition), `src/libs/antares/study/parts/thermal/cluster.cpp:443-492` (defaults/validation).

### 2.3 Validation / integrity check

Run during `ThermalCluster::integrityCheck()`
(`src/libs/antares/study/parts/thermal/cluster.cpp:257-266`):

- If either ramping rate is `<= 0`, or either cost is `< 0`, `checkValidity()` logs an error of
  the form:
  ```
  Thermal cluster: <area>/<cluster>: The maximum upward power ramping rate must be greater
  than zero. Ramping is disabled for this thermal cluster.
  ```
- On any such failure the cluster's `ramping` optional is **reset to `std::nullopt`** — i.e. an
  ill-defined ramping block does not fail the whole study load, it just silently disables
  ramping for that one cluster (with the error logged).

## 3. Summary of parsing/activation logic

```
generaldata.ini [optimization] include-thermal-cluster-ramping = false (default)
        │
        ├─ false → per-cluster ramping-enabled / ramping keys are parsed but never applied
        │
        └─ true  → for each thermal cluster section in list.ini:
                      ramping-enabled = true  →  cluster.ramping = Ramping{} (zeros)
                                              →  power-increase-cost,
                                                 power-decrease-cost,
                                                 max-upward-power-ramping-rate,
                                                 max-downward-power-ramping-rate
                                                 populate the struct
                      integrityCheck()        →  validates ranges;
                                                  invalid ⇒ ramping disabled for that cluster
```

## 4. Minimal example

`settings/generaldata.ini`:
```ini
[optimization]
include-thermal-cluster-ramping = true
```

`input/thermal/clusters/A/list.ini`:
```ini
[gas turbine]
name = gas turbine
group = other
unitcount = 1
nominalcapacity = 200
ramping-enabled = true
power-increase-cost = 5
power-decrease-cost = 5
max-upward-power-ramping-rate = 50
max-downward-power-ramping-rate = 50
```

This enables ramping constraints for cluster `gas turbine` in area `A`, with a maximum
hourly output change of 50 MW in either direction and a linear cost of 5 €/MW applied to
each MW of upward or downward power variation.

## 5. Notes / open points

- Unlike most other numeric cluster properties, these four keys have **no dedicated unit test
  coverage for malformed/non-numeric values** beyond the generic `p->value.to<double>(...)`
  parse-failure path shared by all cluster properties.
- The keys are unconditionally recognized by `ThermalClusterLoadFromProperty` regardless of the
  global `include-thermal-cluster-ramping` switch — they are simply no-ops when ramping isn't
  active for the cluster, so no "unknown property" warning is ever logged for them, even in
  studies that never intend to use ramping.
- This spec covers only the **input parsing / data model** layer
  (`src/libs/antares/study/...`). It does not describe the solver-side constraint generation
  (`src/solver/optimisation/constraints/Ramping*`, `PowerOutputVariation*`) or output variables
  (`src/solver/variable/include/antares/solver/variable/economy/rampingCosts.h`).
