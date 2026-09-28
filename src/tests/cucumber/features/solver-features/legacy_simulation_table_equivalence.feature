Feature: Legacy mc-ind <-> simulation table equivalence
  # The legacy solver run with --output=all writes both the historical
  # mc-ind/<year>/... text tree and the flat simulation table. For every
  # quantity the legacy weekly problem feeds into the table (raw variables via
  # LegacyNameMapper, derived values via LegacyExtraOutputs) the two outputs
  # must carry the same numbers.
  #
  # Quantities currently cross-checked: unsupplied_energy, spilled_energy,
  # price, actual_load, thermal generation_power, thermal actual_num_units_on,
  # short-term-storage injection/withdrawal/level, link flow / abs_flow /
  # minus_flow / actual_loop_flow, link abs_congestion_fee / alg_congestion_fee,
  # area reserve spilled/unsupplied energy, thermal/STS/hydro reserve
  # participation. Gaps (hydro level, MIP-week duals, adequacy-patch DENS /
  # LMR.VIOL rows) are listed in
  # docs/developer-guide/simulation-table-e2e-coverage.md
  #
  # By default the check reads whichever simulation-table stage
  # (optim-nb-1/optim-nb-2) mc-ind's own numbers come from; on a study where a
  # later stage (peak-shaving, adq-patch) changes the published numbers, use
  # "the simulation table for stage "<stage>" matches ..." to point the check
  # at that stage instead (see the adequacy-patch scenario below).

  @short
  Scenario: Single legacy area with a thermal fleet (fast UC)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Two legacy areas joined by a hurdle-cost link
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/Hurdle-cost link"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Legacy area with a generator component (max_p above load)
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_1"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Legacy thermal cluster with a constant load component
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/3_6_3"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Accurate unit commitment - actual_num_units_on and second-pass generation
    # optim-nb-1 is the LP relaxation; the equivalence step reads optim-nb-2, so
    # generation_power and actual_num_units_on line up with the mc-ind NODU /
    # MWh columns produced after the unit-commitment heuristic.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/008 Thermal fleet - Accurate unit commitment"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @fast @short
  Scenario: Hybrid GEMS + legacy thermal, MILP heuristic (two optimisation passes)
    Given the solver study path is "Antares_Simulator_Tests_NR/thermal_milp_gems_and_thermal_legacy"
    And the linear solver is highs
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Thermal cluster reserve participation and area reserve imbalance
    # This study's accurate-UC actual_num_units_on does not yet line up with
    # mc-ind on optim-nb-1 (unlike the accurate-UC scenario above, which reads
    # optim-nb-2); scope the check to the reserve-specific mappings only.
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_1_simple_up"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for "spilled_energy_reserve" in year 1
    And the simulation table matches the legacy mc-ind output for "unsupplied_energy_reserve" in year 1
    And the simulation table matches the legacy mc-ind output for "units_on_reserve_power" in year 1
    And the simulation table matches the legacy mc-ind output for "units_off_reserve_power" in year 1

  @short
  Scenario: Short-term storage with withdrawal efficiency (injection/withdrawal/level)
    # Withdrawal efficiency < 1 on several storages means injection and
    # withdrawal power genuinely differ (not just sign), which is the case the
    # STS mapping needs to be exercised against.
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-v920/st-storage-withdrawal-efficiency"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Short-term storage reserve participation
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Hydro reserve participation
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/LT_1_reserves"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table matches the legacy mc-ind output for year 1

  @short
  Scenario: Hydro remix (peak-shaving) - mc-ind reflects the post-remix stage, not optim-nb-2
    # "hydro preference 1" is a single-area, no-thermal, managed-hydro study
    # where the weekly LP alone cannot avoid unsupplied energy on some hours
    # while running short of hydro on others; RemixHydroPostProcessCmd then
    # reshuffles hydro generation across the week to shave the peaks, moving
    # unsupplied_energy, spilled_energy and price on many hours (e.g. hour 961
    # goes from 5000 MWh unsupplied after optim-nb-2 down to 3586 MWh after
    # remix). mc-ind prints the post-remix numbers, so the plain "for year N"
    # step (which reads optim-nb-2) would compare mc-ind against the wrong
    # stage here.
    Given the solver study path is "Antares_Simulator_Tests_NR/short-tests/hydro preference 1"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table for stage "peak-shaving" matches the legacy mc-ind output for year 1

  @short
  Scenario: Adequacy patch (CSR) - mc-ind reflects the adq-patch stage, not optim-nb-2
    # include-adq-patch = true here, so the CSR post-treatment is the last
    # thing to touch this week's results (see the worked example in
    # legacy_simulation_table.feature, "Per-stage simulation tables show what
    # the adequacy patch moved"): mc-ind prints the post-CSR numbers, which
    # differ from optim-nb-2's on the areas inside the patch (unsupplied
    # energy is redistributed, price loses its anti-degeneracy noise and is
    # overwritten with the un-noised unserverdenergycost). The plain "for year
    # N" step reads optim-nb-2 and would compare mc-ind against the wrong
    # stage on this study, so use the stage-scoped step instead.
    Given the solver study path is "Antares_Simulator_Tests_NR/adequacy-patch-CSR/adq-patch-CSR-test-case-v02"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the simulation table for stage "adq-patch" matches the legacy mc-ind output for year 1
