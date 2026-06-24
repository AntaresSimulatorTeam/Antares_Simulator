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
| [x] | `imbalance_cost` | `spillage_cost·spilled + unsupplied_energy_cost·unsupplied` | `UnsuppliedEnergy` |
| [x] | `is_loss_of_load` | `unsupplied_energy > 0.5` | `UnsuppliedEnergy` |
| [x] | `is_near_loss_of_load` | `dual(balance) > unsupplied_energy_cost - 5` | `AreaBalance` constraint |

---

## 2.4.2.2 — Model: renewable  ⬜ not started

| Done | Output | Formula | Design |
|------|--------|---------|--------|
| [ ] | `generation_power` | `available_power` | **Anchorless emission path** (B) |
| [ ] | `minus_generation` | `-available_power` | **Anchorless emission path** (B) |

`available_power` is input data (cluster TS in cluster mode; `area.wind/solar.series`
or `area.hydro.series->ror` in aggregated mode). Renewables are **not** LP variables
in the legacy weekly problem, so there is no recorded anchor — these rows need a
new path that emits per component/hour straight from study data. Component naming:
`{area}_wind`, `{area}_solar`, `{area}_run_of_river` (aggregated) or the cluster name.

---

## 2.4.2.3 — Model: misc_gen  ⬜ not started

| Done | Output | Formula | Design |
|------|--------|---------|--------|
| [ ] | `generation_power` | `available_power` | **Anchorless emission path** (B) |
| [ ] | `minus_generation` | `-available_power` | **Anchorless emission path** (B) |

`available_power = miscGen.entry[MiscGenIndex]`. Same anchorless need as renewable.
Component → `miscGen.entry` index mapping:

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

## 2.4.2.4 — Model: thermal  🟨 mostly done (profit left)

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `actual_num_units_on` | `ceil(num_units_on)` | `NODU` |
| [x] | `prop_cost` | `generation_cost · generation_power` | `DispatchableProduction` |
| [x] | `non_prop_cost` | `startup_cost·max(0, ceil(N) - ceil(N)[t-1]) + fixed_cost·ceil(N)` | `NODU` (+ `NumberStartingDispatchableUnits` via view) |
| [x] | `cluster_availability` | `max(cmg·(1-s), mpu·ceil(cmg/Mpu))` | `DispatchableProduction` (spinning cancels) |
| [x] | `min_gen_power` | `min(generation_power, min_gen_mod·num_units·Mpu)` | `DispatchableProduction` |
| [x] | `up_margin` | `cluster_availability - generation_power` | `DispatchableProduction` |
| [x] | `down_margin` | `generation_power - min(cluster_availability, min_gen_mod·num_units·Mpu)` | `DispatchableProduction` |
| [x] | `co2/nh3/so2/nox/pm2_5/pm5/pm10/nmvoc/op1..op5_emissions` (13) | `generation_power · emission_rate` | `DispatchableProduction` (factors in context, keyed `area$$cluster`) |
| [ ] | `profit` | `(area_price - generation_cost)·max(generation_power - min_gen_mod·num_units·Mpu, 0)` | **Area-price map** (A); cluster→area now available via `info.area` |

`profit` needs the price of the cluster's area (the `AreaBalance` dual). The cluster
anchor is now area-qualified (`info.area`), so the only missing piece is an
area→price lookup (A). The `min_gen_mod·num_units·Mpu` floor is the same term used
by `min_gen_power`/`down_margin` (carried in the context as `minGenPower`).

---

## 2.4.2.5 — Model: load  ✅ complete

| Done | Output | Formula | Anchor |
|------|--------|---------|--------|
| [x] | `actual_load` | `load` (raw = residual + must-run) | `UnsuppliedEnergy` (load series in context) |

---

## 2.4.2.6 — Model: link  🟨 mostly done (congestion fees left)

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `abs_flow` | `max(flow, -flow)` | `DirectFlow` |
| [x] | `minus_flow` | `-flow` | `DirectFlow` |
| [x] | `prop_cost` | `direct_hurdle·direct_flow + indirect_hurdle·indirect_flow` | `PositiveDirectFlow` (+ `PositiveIndirectFlow` via view) |
| [x] | `capacity_shadow_price` | `max(dual(flow_dissociation), -dual(...))` | `FlowDissociation` constraint |
| [x] | `is_directly_congested` | `flow >= direct_capacity` | `DirectFlow` (capacity in context) |
| [x] | `is_indirectly_congested` | `flow <= -indirect_capacity` | `DirectFlow` (capacity in context) |
| [x] | `actual_loop_flow` | `loop_flow` | `DirectFlow` (loop flow in context) |
| [ ] | `abs_congestion_fee` | `max(flow,-flow)·\|price_out - price_in\|` | **Area-price map** (A) + **link→endpoint-area** resolution |
| [ ] | `alg_congestion_fee` | `flow·(price_out - price_in)` | **Area-price map** (A) + **link→endpoint-area** resolution |

`price_out`/`price_in` are the balance duals of the link's extremity / origin areas
(spec writes them via `sum_connections(out_port.price)` / `in_port.price`). Both
endpoint area names are known from `problemeHebdo`; the only missing piece is the
area→price lookup (A).

---

## 2.4.2.7 — Model: short_term_storage  ⬜ not started

| Done | Output | Formula | Design |
|------|--------|---------|--------|
| [ ] | `profit` | `floor((withdrawal_power - injection_power)·area_price + 0.5)` | **Area-price map** (A) + **view area-qualification** (C) |

`ShortTermStorageInjection`/`Withdrawal` are recorded with the storage name as
component; the namer's area is captured (`info.area`), but combining the two
variables of the same storage needs the view to key by area too (storage names can
collide across areas), plus the area price (A).

---

## 2.4.2.8 — Model: hydro  🟨 mostly done (bellman_value left)

| Done | Output | Formula | Anchor / Design |
|------|--------|---------|-----------------|
| [x] | `level_percentage` | `level / reservoir_capacity · 100` | `HydroLevel` (capacity in context) |
| [x] | `actual_inflows` | `round(inflows)` | `HydroLevel` (inflows in context) |
| [x] | `hydro_shadow_price` | `dual(FinalStockExpression)` | `FinalStockExpression` constraint |
| [ ] | `bellman_value` | `sum(cost_layer · LayerStorage)` | **Per-area layer aggregation** (D) |

`LayerStorage` variables are recorded with the layer index as component and are now
area-qualified (`info.area`); `cost_layer` is their objective coefficient
(`CoutLineaire`). The row is per area, so the layers of one area must be summed and
emitted once on the area (D).

---

## 2.5 — Port fields written to the ST  ⬜ not started

Same emission mechanism as extra-outputs ("hard-coded list of extra expressions").
Needed both as standalone ST entries and because the congestion fees read the link
endpoints' `*_port.price`.

| Done | Model | Port field | Definition | Design |
|------|-------|-----------|------------|--------|
| [ ] | area | `balance_port.price` | `dual(balance)` | **Port-field emission** (E); value = `price` (already computed) |
| [ ] | load | `balance_port.flow` | `-load` | (E); load already in context |
| [ ] | link | `out_port.flow` | `flow` | (E) |
| [ ] | link | `in_port.flow` | `-flow` | (E) |
| [ ] | renewable | `balance_port.flow` | `available_power` | (E) + anchorless data (B) |
| [ ] | miscellaneous_generation | `balance_port.flow` | `available_power` | (E) + anchorless data (B) |
| [ ] | thermal | `balance_port.flow` | `generation_power` | (E); raw output exists |
| [ ] | short_term_storage | `balance_port.flow` | `withdrawal_power - injection_power` | (E) + view (C) |
| [ ] | long_term_storage | `balance_port.flow` | `withdrawal_power - injection_power` | (E) |

---

## Progress summary

| Model | Implemented | Remaining |
|-------|-------------|-----------|
| area | 4 / 4 | — |
| renewable | 0 / 2 | generation_power, minus_generation |
| misc_gen | 0 / 2 | generation_power, minus_generation |
| thermal | 22 / 23 | profit |
| load | 1 / 1 | — |
| link | 7 / 9 | abs_congestion_fee, alg_congestion_fee |
| short_term_storage | 0 / 1 | profit |
| hydro | 3 / 4 | bellman_value |
| ports (2.5) | 0 / 9 | all |

---

## Design building blocks still needed

The remaining work clusters around a few reusable capabilities. Implementing each
once unlocks several outputs.

### (A) Area-price map  — *highest leverage*
A lookup `area → price` (i.e. `-dual(AreaBalance)`), built from the recorded
`AreaBalance` constraints + duals already passed to `AddLegacyExtraOutputs`.
Unlocks: **thermal profit**, **STS profit**, **link abs/alg_congestion_fee**, and
the area `balance_port.price` field.
- Note: the price is a constraint dual (computed in the constraints pass); the
  consumers are variable-anchored, so build the map up front (one pass over
  `constraintsInfo`/`duals`) before the variable dispatch loop.
- Caveat: zero on MIP weeks (duals not extracted) — same limitation as `price`.

### (B) Anchorless study-data emission path
A way to emit ST rows for components that are **not** LP variables (renewable
clusters, misc-gen entries). Emits per component/hour from study data carried in
the context. Unlocks: **renewable** + **misc_gen** `generation_power` /
`minus_generation`, and their `balance_port.flow` fields.
- Needs the `available_power` series plumbed into the context, plus the component
  naming conventions (`{area}_wind/_solar/_run_of_river`, the misc-gen table).

### (C) View area-qualification
Teach `LegacySolutionView` to key by area in addition to component, so cluster- and
storage-level companion lookups are unambiguous across areas. Unlocks robust
**STS profit** (combine withdrawal + injection of the same storage). The `area`
field on `LegacyVariableInfo` already exists; this extends the view's key.

### (D) Per-area layer aggregation
Accumulate `cost_layer · LayerStorage` across the `LayerStorage` variables of one
area and emit a single per-area row. Unlocks **bellman_value**.

### (E) Port-field emission layer (§2.5)
A hard-coded table of `port.field → expression` emitted into the ST, reusing
already-computed values where possible (`balance_port.price` = `price`, thermal
`balance_port.flow` = `generation_power`, link `out/in_port.flow` = `±flow`).
Unlocks all of §2.5. Needed in full for the GEMS port views; the congestion fees
(2.4.2.6) only need the *price* port values, which (A) already provides directly.

---

## Suggested order

1. **(A) Area-price map** → unlocks thermal profit, STS profit (with C), and link
   congestion fees in one go. Biggest single unlock, no new study-data plumbing.
2. **(C) View area-qualification** → enables STS profit cleanly; small, additive.
3. **(D) Per-area layer aggregation** → bellman_value; self-contained.
4. **(B) Anchorless emission path** → renewable + misc_gen; distinct mechanism.
5. **(E) Port fields** → §2.5; mostly reuses prior values.
