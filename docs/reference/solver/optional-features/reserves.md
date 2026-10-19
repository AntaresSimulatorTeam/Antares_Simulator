# File format for reserves
## Reserves definition

For each area, a new file **input/reserves/&lt;area id&gt;/reserves.yml** defines the area's reserves:

```yaml
global-parameters:
  energy-activation-ratio-up: 0.9
  energy-activation-ratio-down: 0.9
  reference-activation-duration-up: 2
  reference-activation-duration-down: 2

reserves:
  - name: primary-up
    type: up
    failure-cost: 150
    spillage-cost: 100
    power-activation-ratio: 1
    energy-activation-ratio: 0.8
    reference-activation-duration: 2
  - name: primary-down
    type: down
    failure-cost: 150
    spillage-cost: 100
    power-activation-ratio: 1
    energy-activation-ratio: 0.8
    reference-activation-duration: 2
```

Properties:

- `global-parameters` (optional), global parameters per area:
    - `energy-activation-ratio-up` / `energy-activation-ratio-down`: maximum global energy activation ratio
      for up / down reserves
    - `reference-activation-duration-up` / `reference-activation-duration-down`: reference activation
      duration for up / down reserves
- `reserves` (optional), list of reserves. Each entry contains:
    - `name` (string, required, must be unique within the area)
    - `type` (string, required): `up` or `down`
    - `failure-cost` (double): cost of failure for the reserve
    - `spillage-cost` (double): cost of spillage for the reserve
    - `power-activation-ratio` (double): maximum activation ratio
    - `energy-activation-ratio` (double): energy activation ratio
    - `reference-activation-duration` (double): reference activation duration

For each defined reserve, the corresponding hourly need time-series must be located at
**input/reserves/&lt;area id&gt;/&lt;reserve id&gt;.txt** (the reserve id is the reserve name lower-cased,
characters outside of `[a-z0-9_-(),& ]` being replaced by spaces). The file contains 8760 hourly values
(whitespace separated).

## Reserve participations

Reserve participations declare which components (hydro, thermal clusters, short-term storages) participate in
the reserves of their area. These files are only read if `include-reserves = true` **and**
`other preferences/unit-commitment-mode` is not `fast`. If a file is absent, it is simply ignored.

For each area, the hydro participation file **input/hydro/common/&lt;area id&gt;/reserve-participations.yml**:

```yaml
participations:
  certifications:
    - reserve: primary-up
      participation-cost: 10
      max-release: 500
      max-store: 300
  symmetries:
    - reserves: [primary-up, primary-down]
```

For each area, the thermal participation file **input/thermal/clusters/&lt;area id&gt;/reserve-participations.yml**:

```yaml
participations:
  - cluster: my-cluster
    certifications:
      - reserve: primary-up
        participation-cost: 10
        max-power: 100
        max-power-off: 20
        participation-cost-off: 5
    symmetries:
      - reserves: [primary-up, primary-down]
```

For each area, the short-term storage participation file
**input/st-storage/clusters/&lt;area id&gt;/reserve-participations.yml**:

```yaml
participations:
  - storage: my-storage
    certifications:
      - reserve: primary-up
        participation-cost: 10
        max-release: 500
        max-store: 300
    symmetries:
      - reserves: [primary-up, primary-down]
```

Notes:

- Every `reserve` name must match a reserve defined in **input/reserves/&lt;area id&gt;/reserves.yml** for the
  same area.
- For thermal clusters, the certification properties are `participation-cost`, `max-power`, `max-power-off` and
  `participation-cost-off`. For hydro and short-term storages, they are `participation-cost`, `max-release` and
  `max-store`. All values must be non-negative.
- Each `symmetries.reserves` group must contain at least two reserves. A cluster/storage/hydro can only declare
  symmetries if it has at least one certification (participation).
