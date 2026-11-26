# View builder

The view builder prepares YAML files that describe a study's system and the views to apply to its
results. The system file supplies components, properties and connections; a view configuration
selects locations, aggregation patterns and metrics. The numerical results themselves are written
separately, for example in the [simulation table](modeler/03-outputs.md#simulation-table).

## Generated files

The solver writes these files under `<timestamp output>/views/input/` before running the simulation:

| File | Purpose |
|---|---|
| `system-for-views.yml` | System description for views, including legacy and, in hybrid studies, modeler components. |
| `view_config_area.yml` | Area metrics. |
| `view_config_link.yml` | Link metrics. |
| `view_config_thermal.yml` | Thermal cluster metrics. |
| `view_config_sts.yml` | Short-term storage cluster metrics. |
| `view_config_renewable.yml` | Renewable cluster metrics. |

A view configuration is omitted if it has no selected metrics or no aggregation patterns. In a
modeler-only run, `system-for-views.yml` is a copy of `input/system.yml`; the modeler does not
generate the five solver view configurations. For the input system format, see the
[modeler system file](modeler/02-inputs.md#system-file).

## System for views

For a solver run, `system-for-views.yml` contains a `system` mapping with:

- `id`: the study folder path.
- `components`: the system elements, each with an `id`, a `model`, an empty `parameters` list,
  and, when available, `properties` as `id`/`value` pairs.
- `connections`: pairs of component ports, expressed as `component1`, `port1`, `component2` and
  `port2`.

For example, the following is an excerpt for an area and its load:

```yaml
system:
  id: /path/to/study
  components:
    - id: france
      model: antares_legacy_models.area
      parameters: []
      properties:
        - id: carrier
          value: electricity
        - id: district_eu
          value: eu
    - id: france_load
      model: antares_legacy_models.load
      parameters: []
      properties:
        - id: carrier
          value: electricity
  connections:
    - component1: france_load
      port1: balance_port
      component2: france
      port2: balance_port
```

The solver represents each area as a component and adds the applicable load, renewable, thermal,
storage, miscellaneous generation and link components. Their IDs follow the same conventions as
the corresponding simulation table components. Legacy components use models from
`antares_legacy_models` and properties such as `carrier`, `technology`, `name` or `group`.
Area properties also record membership in sets of areas as `district_<set-id>` with the set ID as
the value.

Disabled thermal, renewable and short-term storage clusters are omitted. Load, run-of-river and
miscellaneous generation components are omitted when their relevant time series are all zero. Links
are omitted when capacity is ignored or both directions have all-zero capacity. Aggregated wind and
solar are included only in aggregated renewable mode and when their series are not all zero;
renewable clusters are included only when enabled in cluster mode. A hydro storage component is
included when reservoir management is enabled or hydro inflow is not all zero. For time series
with multiple chronicles, the inactivity check considers chronicles selected for the study's
Monte-Carlo years when those selections are available.

Area-connected legacy components use `balance_port` at both ends. Links connect their `in_port`
and `out_port` to the two areas' `balance_port`, in lexicographical order of area IDs.

In a [hybrid study](solver/hybrid-studies.md), the solver also appends modeler components and
their properties, component-to-component connections and area connections. An area connection is
represented as a connection to the area's `balance_port`. Modeler component IDs must not collide
with generated legacy component IDs; a collision causes an input error. The generated system is a
description for views, so its empty parameter lists do not reproduce the modeler's input parameter
values.

## View configuration files

Each solver-generated configuration has a root `view` mapping. This excerpt illustrates an area
view for a district and selected metrics:

```yaml
view:
  id: view_area
  taxonomy: antares_legacy_taxonomy
  scope:
    location:
      taxonomy-category: balance
    extra-locations:
      - id: district_eu
  aggregations-patterns:
    - id: hour_ind
      time-granularity: hour
      scenario: false
      location-filter: [france, eu]
    - id: day_all
      time-granularity: day
      scenario: true
  metrics:
    - id: antares_legacy_area_catalog.LOAD
    - id: antares_legacy_area_catalog.OV_COST
```

The example shows only some patterns and metrics. Fields are:

- `id`: the view identifier. The generated identifiers are `view_area`, `view_link`,
  `view_thermal`, `view_storage` and `view_renewable`.
- `taxonomy`: `antares_legacy_taxonomy`. `scope.location.taxonomy-category` is `link` for the
  link view and `balance` for the other views.
- `scope.extra-locations`: for non-link views, nonempty sets of areas enabled for output are
  added as `district_<set-id>`. Link views do not include districts.
- `aggregations-patterns`: combinations of `hour`, `day`, `week`, `month` and `year` with
  individual scenarios (`<granularity>_ind`, `scenario: false`) or synthesis across scenarios
  (`<granularity>_all`, `scenario: true`). Without geographic trimming there are ten patterns.
- `metrics`: catalog-qualified metric IDs. The catalog depends on the view type:

| View | Catalog |
|---|---|
| Area | `antares_legacy_area_catalog` |
| Link | `antares_legacy_link_catalog` |
| Thermal | `antares_legacy_thermal_cluster_catalog` |
| Short-term storage | `antares_legacy_short_term_storage_catalog` |
| Renewable | `antares_legacy_renewable_cluster_catalog` |

### Trimming

With geographic trimming enabled, the solver applies the relevant year-by-year or synthesis
output filter for each time granularity. Area and cluster views use area and eligible district
filters; the link view uses link filters. A pattern with no selected locations is omitted. When
only some locations are selected, the pattern contains a `location-filter` list; it is omitted
when all locations are selected. Area and link entries use component IDs, while district entries
use the set ID (for example `eu`, rather than `district_eu`).

With thematic trimming enabled, a metric is included when at least one of its mapped legacy output
variables is selected. For example, `RES_GENERATION` is selected by any of `Renewable Gen.`,
`WIND`, `SOLAR` or `H. ROR`; `STS by group` selects the area storage injection, withdrawal and
level metrics. Without thematic trimming, all metrics defined for the view are included.
