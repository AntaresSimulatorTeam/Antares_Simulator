# Legacy Extra-Outputs — Spec Checklist (ANT-5006 §2.4 / §2.5)

Status of every extra-output (and port field) the GEMS views need from the legacy
solver's simulation table. See `legacy-extra-outputs.md` for the design of the
mechanism itself.

**Legend**
- `[x]` implemented (formula + unit test, cucumber where derivable)
- `[ ]` not implemented yet
- **Design** — the capability that still has to be built before the row can be
  emitted (see §"Design building blocks still needed" at the bottom for the
  shared pieces).

All "anchored on" references are to the recorded legacy variable/constraint that
supplies the row's component and time index.

---

## 2.4.2.1 — Model: area  ✅ complete

| Done | Output | Formula | Anchor |
|------|--------|---------|--------|
| [x] | `price` | `dual(balance)` (stored as `-dual`) | `AreaBalance` constraint |
| [x] | `imbalance_cost` | `spillage_cost·spilled + unsupplied_energy_cost·unsupplied` (user costs, un-noised: `CoutDeDefaillance*SansBruit`) | `UnsuppliedEnergy` |
| [x] | `is_loss_of_load` | `unsupplied_energy > 0` | `UnsuppliedEnergy` |
| [x] | `is_significant_loss_of_load` | `unsupplied_energy > 0.5` | `UnsuppliedEnergy` |
| [x] | `is_near_loss_of_load` | `dual(balance) > unsupplied_energy_cost - 5` | `AreaBalance` constraint |

Adequacy-patch outputs (`domestic_unsupplied_energy` / DENS,
`is_local_matching_rule_violated` / LMR.VIOL) are **blocked on ANT-5240** (spec
still being written); CSR outputs are explicitly out of scope per the spec.

---

## 2.4.2.2 — Model: renewable  ✅ complete

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `generation_power` | `available_power` | `InputGenerationOfArea` series (B) |
| [x] | `minus_generation` | `-available_power` | `InputGenerationOfArea` series (B) |

`available_power` is input data (cluster TS in cluster mode; `area.wind/solar.series`
or `area.hydro.series->ror` in aggregated mode). Renewables are **not** LP variables
in the legacy weekly problem, so `SIM_RenseignementProblemeHebdo` copies the week's
series into `PROBLEME_HEBDO::InputGenerationOfArea` and the emitter reads them
per component/hour. Component naming: `{area}_wind`, `{area}_solar`,
`{area}_run_of_river` (aggregated) or the cluster name (clusters mode; enabled
clusters only, matching the must-run aggregation).

---

## 2.4.2.3 — Model: misc_gen  ✅ complete

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `generation_power` | `available_power` | `InputGenerationOfArea` series (B) |
| [x] | `minus_generation` | `-available_power` | `InputGenerationOfArea` series (B) |

`available_power = miscGen.entry[MiscGenIndex]`. Same input-series path as
renewable. Component → `miscGen.entry` index mapping:

| Component | MiscGenIndex |
|-----------|--------------|
| `{area}_combined_heat_power` | `fhhCHP` |
| `{area}_biomass` | `fhhBioMass` |
| `{area}_biogas` | `fhhBioGaz` |
| `{area}_waste` | `fhhWaste` |
| `{area}_geothermal` | `fhhGeoThermal` |
| `{area}_other` | `fhhOther` |
| `{area}_pumped_storage_power` | `fhhPSP` |
| `{area}_rest_world` | `fhhRowBalance` |

---

## 2.4.2.4 — Model: thermal  ✅ complete

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `actual_num_units_on` | `ceil(num_units_on)` | `NODU` |
| [x] | `prop_cost` | `market_bid_cost · generation_power` (user cost, un-noised: `CoutHoraireDeProductionDuPalierThermiqueSansBruit`) | `DispatchableProduction` |
| [x] | `non_prop_cost` | `startup_cost·max(0, ceil(N) - ceil(N)[t-1]) + fixed_cost·ceil(N)` | `NODU` (+ `NumberStartingDispatchableUnits` via view) |
| [x] | `cluster_availability` | `max(cmg·(1-s), mpu·ceil(cmg/Mpu))` | `DispatchableProduction` (spinning cancels) |
| [x] | `min_gen_power` | `min(generation_power, min_gen_mod·num_units·Mpu)` | `DispatchableProduction` |
| [x] | `up_margin` | `cluster_availability - generation_power` | `DispatchableProduction` |
| [x] | `down_margin` | `generation_power - min(cluster_availability, min_gen_mod·num_units·Mpu)` | `DispatchableProduction` |
| [x] | `co2/nh3/so2/nox/pm2_5/pm5/pm10/nmvoc/op1..op5_emissions` (13) | `generation_power · emission_rate` | `DispatchableProduction` (factors in context, keyed `area$$cluster`) |
| [x] | `profit` | `(area_price - generation_cost)·max(generation_power - min_gen_mod·num_units·Mpu, 0)` | **Area-price map** (A); cluster→area via `info.area` |

`profit` uses the price of the cluster's area (the `AreaBalance` dual) via the
area-price map (A), `generation_cost`/`generation_power` read by index off the
`DispatchableProduction` anchor, and the `min_gen_mod·num_units·Mpu` floor reused
from the context's `minGenPower` (same term as `min_gen_power`/`down_margin`).
Skipped when the area has no recorded price or the cluster is absent from the
margin context.

---

## 2.4.2.5 — Model: load  ✅ complete

| Done | Output | Formula | Anchor |
|------|--------|---------|--------|
| [x] | `actual_load` | `load` (raw = residual + must-run) | `UnsuppliedEnergy` (load series in context) |

---

## 2.4.2.6 — Model: link  ✅ complete

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `abs_flow` | `max(flow, -flow)` | `DirectFlow` |
| [x] | `minus_flow` | `-flow` | `DirectFlow` |
| [x] | `prop_cost` | `direct_hurdle·direct_flow + indirect_hurdle·indirect_flow` | `PositiveDirectFlow` (+ `PositiveIndirectFlow` via view) |
| [x] | `capacity_shadow_price` | `max(dual(flow_dissociation), -dual(...))` | `FlowDissociation` constraint |
| [x] | `is_directly_congested` | `flow >= direct_capacity` | `DirectFlow` (capacity in context) |
| [x] | `is_indirectly_congested` | `flow <= -indirect_capacity` | `DirectFlow` (capacity in context) |
| [x] | `actual_loop_flow` | `loop_flow` | `DirectFlow` (loop flow in context) |
| [x] | `abs_congestion_fee` | `max(flow,-flow)·\|price_out - price_in\|` | **Area-price map** (A) + link→endpoint-area resolution |
| [x] | `alg_congestion_fee` | `flow·(price_out - price_in)` | **Area-price map** (A) + link→endpoint-area resolution |

`price_out`/`price_in` are the balance duals of the link's extremity / origin areas,
looked up in the area-price map (A) after splitting the `DirectFlow` anchor's
`origin$$destination` component into its two endpoint area names. Skipped when
either endpoint area has no recorded price.

---

## 2.4.2.7 — Model: short_term_storage  ✅ complete

| Done | Output | Formula | Anchor |
|------|--------|---------|--------|
| [x] | `profit` | `floor((withdrawal_power - injection_power)·area_price + 0.5)` | `ShortTermStorageWithdrawal`/`Injection` via `problemeHebdo.ShortTermStorage[pays]` (price = balance dual) |

Emitted per storage of each area inside the hourly loop; the (area, storage)
pair comes from the study-structure iteration, so no view-side area
qualification is needed. Dual-derived, hence zero on MIP weeks (same
limitation as `price`).

---

## 2.4.2.8 — Model: hydro  ✅ complete

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `level_percentage` | `level / reservoir_capacity · 100` | `HydroLevel` (capacity in context) |
| [x] | `actual_inflows` | `round(inflows)` | `HydroLevel` (inflows in context) |
| [x] | `hydro_shadow_price` | `dual(FinalStockExpression)` | `FinalStockExpression` constraint |
| [x] | `bellman_value` | `−sum(cost_layer · LayerStorage)` | `LayerStorage` variables of the area (weekly, accurate water value only) |

`bellman_value` sums `−CoutLineaire · X` over the area's `LayerStorage` variables
(the coefficient is `-WaterLayerValues[layer]`, written by the cost-assignment
site, so the value is `Σ WaterLayerValues · X`) and is emitted once per area on
the interval's last hour, like `hydro_shadow_price`.

---

## 2.5 — Port fields written to the ST  ✅ complete

Same emission mechanism as extra-outputs; the port field name is the ST `output`.
Component naming: models sharing the area's identity get their own suffixed
component so `balance_port.flow` rows cannot collide — `{area}_load` (load) and
`{area}_hydro_storage` (long_term_storage); everything else uses the component of
its extra-outputs (area, cluster, storage `{area}_short_term_storage_{name}`,
`origin$$destination`, input-generation names).

| Done | Model | Component | Port field | Definition |
|------|-------|-----------|-----------|------------|
| [x] | area | `{area}` | `balance_port.price` | `dual(balance)` (same value as `price`) |
| [x] | load | `{area}_load` | `balance_port.flow` | `-load` (raw load, as `actual_load`) |
| [x] | link | `origin$$destination` | `out_port.flow` | `flow` |
| [x] | link | `origin$$destination` | `in_port.flow` | `-flow` |
| [x] | renewable | input-generation names | `balance_port.flow` | `available_power` |
| [x] | miscellaneous_generation | input-generation names | `balance_port.flow` | `available_power` |
| [x] | thermal | cluster name | `balance_port.flow` | `generation_power` |
| [x] | short_term_storage | `{area}_short_term_storage_{name}` | `balance_port.flow` | `withdrawal_power - injection_power` |
| [x] | long_term_storage | `{area}_hydro_storage` | `balance_port.flow` | `HydProd - Pumping` (skipped without hydro production; pumping term dropped when absent) |

---

## Progress summary

| Model | Implemented | Remaining |
|-------|-------------|-----------|
| area | 5 / 5 | — (adequacy-patch rows blocked on ANT-5240) |
| renewable | 2 / 2 | — |
| misc_gen | 2 / 2 | — |
| thermal | 23 / 23 | — |
| load | 1 / 1 | — |
| link | 9 / 9 | — |
| short_term_storage | 1 / 1 | — |
| hydro | 4 / 4 | — |
| ports (2.5) | 9 / 9 | — |

---

## Design building blocks still needed

Since the derived outputs are computed by iterating the study structure and
reading the solved problem through the correspondence tables (see
`legacy-extra-outputs.md`), most former blockers are gone: any area price is one
dual read away, and cluster- or storage-level operands are unambiguous because
the loops carry the (area, cluster) pair. The remaining work:

### (A) STS outputs — ✅ done
The per-area STS loop now runs in `AddLegacyExtraOutputs` and emits **STS
profit** = `floor(price(area) × (withdrawal − injection) + 0.5)`.
- Caveat: dual-derived values are zero on MIP weeks (duals not extracted) — same
  limitation as `price`.

### (B) Anchorless study-data emission — ✅ done
✅ done — `SIM_RenseignementProblemeHebdo` copies the week's input-only series
(renewables, misc gen) into `PROBLEME_HEBDO::InputGenerationOfArea`
(`fillInputGenerationSeries` in `sim_calcul_economique.cpp`), and the emitter's
`inputGenerationOutputs` walks them per component/hour. Provides **renewable** +
**misc_gen** `generation_power` / `minus_generation`; their `balance_port.flow`
fields (§2.5) can reuse the same series.

### (C) Per-area layer aggregation — ✅ done
`−cost_layer · LayerStorage` is accumulated across the `LayerStorage` variables of
one area (`vm.LayerStorage(pays, layer)`) and emitted as a single per-area
**bellman_value** row in `weeklyHydroOutputs`.

### (D) Port-field emission layer (§2.5) — ✅ done
The port fields are emitted alongside the extra-outputs they reuse
(`balance_port.price` = `price`, thermal `balance_port.flow` =
`generation_power`, link `out/in_port.flow` = `±flow`, storage and
input-generation flows from their existing operands, `{area}_hydro_storage` from
`HydProd`/`Pumping`).

---

## Suggested order

1. ~~**(A) STS outputs** → STS profit; one more entity loop.~~ done
2. ~~**(C) Per-area layer aggregation** → bellman_value; self-contained.~~ done
3. ~~**(B) Anchorless emission path** → renewable + misc_gen; distinct loop.~~ done
4. ~~**(D) Port fields** → §2.5; mostly reuses prior values.~~ done
