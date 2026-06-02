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
