Feature: Legacy mc-ind <-> simulation table equivalence
  # The legacy solver run with --output=all writes both the historical
  # mc-ind/<year>/... text tree and the flat simulation table. For every
  # quantity the legacy weekly problem feeds into the table (raw variables via
  # LegacyNameMapper, derived values via LegacyExtraOutputs) the two outputs
  # must carry the same numbers.
  #
  # The step below walks a declarative mapping (see
  # features/steps/common_steps/simulation_table_equivalence.py) over every
  # area / thermal cluster / short-term-storage cluster / link of the study and
  # compares the simulation table value against its mc-ind counterpart at every
  # timestep, within a per-quantity tolerance. It reads the final optimisation
  # pass (optim-nb-2 when present, else optim-nb-1) and never compares against a
  # frozen reference file, so it keeps working while the table format evolves.
  #
  # Quantities currently cross-checked: unsupplied_energy, spilled_energy,
  # price, actual_load, thermal generation_power, thermal actual_num_units_on,
  # short-term-storage injection/withdrawal/level, link flow / abs_flow /
  # minus_flow / actual_loop_flow. Gaps (hydro level, MIP-week duals,
  # adequacy-patch rows) are listed in
  # docs/developer-guide/simulation-table-e2e-coverage.md

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
