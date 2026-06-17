Feature: Legacy variables in simulation table

  # The legacy solver populates PROBLEME_ANTARES_A_RESOUDRE::NomDesVariables
  # with names of the form `<Output>::<location>::hour<N>`. After the per-week
  # solve, FillLegacySimulationTable parses those names, applies LegacyNameMapper
  # to the output, and pushes one row per parsed variable into the per-numSpace
  # SimulationTable. This scenario checks that:
  #   - the simulation table file is produced for a pure-legacy study
  #   - the LegacyNameMapper transform (UnsuppliedEnergy -> unsupplied_energy)
  #     is visible in the CSV output
  #   - the row carries the value the legacy solver computed for that hour

  @fast @short
  Scenario: Legacy UnsuppliedEnergy is mapped to unsupplied_energy and carries the solver value
    # "002 Thermal fleet - Base" has a single area (id "area") with a known
    # shortfall of 52 MW on "2 JAN 09:00" of year 1, i.e. absolute hour 34
    # (1-based), which lives in week 0 -> block 1. Antares lowercases area
    # ids, so the legacy variable produced by the solver is
    # `UnsuppliedEnergy::area<area>::hour<33>` (0-based legacy hour); is stored as
    # component=area, output=unsupplied_energy, absolute_time_index=34.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation succeeds
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And the modeler outputs contain the following entries
      | block | component  | output            | timestep | scenario | value |
      | 1     | area | unsupplied_energy | 34       | 0        | 52    |

  @fast @short
  Scenario: Extra outputs prop_cost and imbalance_cost are derived from the legacy solution
    # Same study, same shortfall hour (absolute hour 34, block 1). With 52 MW
    # of unsupplied energy every thermal cluster is at its maximum, so:
    #   - prop_cost = marginal_cost * generated_power per cluster:
    #       base:      35 * (4 * 900) = 126000
    #       semi base: 50 * (5 * 300) =  75000
    #       peak:      80 * (8 * 100) =  64000
    #   - imbalance_cost = unsupplied_energy_cost * unsupplied_energy
    #     + spilled_energy_cost * spilled_energy = 10000 * 52 + 0 * 0 = 520000
    #   - is_loss_of_load = 1 since unsupplied energy (52) exceeds the 0.5 MW
    #     threshold
    #   - price = -dual(balance) = 10000: with unsupplied energy strictly
    #     between its bounds, the marginal MW is served at the unsupplied
    #     energy cost
    #   - is_near_loss_of_load = 1 since price (10000) > 10000 - 5

    # The derived costs are computed from the objective coefficients, which
    # carry the anti-degeneracy noise the legacy solver always adds to costs
    # (PrepareRandomNumbers in common-eco-adq.cpp: |noise| is forced into
    # [5e-4, 6e-4] per thermal cluster cost even with spread-cost = 0, and
    # this study sets spread-unsupplied-energy-cost = 0.01). Hence the relaxed
    # tolerance: relative 1e-4 covers a noise of 6e-4 on the cheapest cost
    # (35) while still pinning the value to the theoretical formula.
    Given the solver study path is "Antares_Simulator_Tests_NR/hybrid/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation succeeds
    And the modeler outputs contain the following entries with relative tolerance 1e-4
      | block | component | output         | timestep | scenario | value  |
      | 1     | base      | prop_cost      | 34       | 0        | 126000 |
      | 1     | semi base | prop_cost      | 34       | 0        | 75000  |
      | 1     | peak      | prop_cost      | 34       | 0        | 64000  |
      | 1     | area      | imbalance_cost | 34       | 0        | 520000 |
      | 1     | area      | is_loss_of_load | 34      | 0        | 1      |
      | 1     | area      | price          | 34       | 0        | 10000  |
      | 1     | area      | is_near_loss_of_load | 34  | 0        | 1      |
