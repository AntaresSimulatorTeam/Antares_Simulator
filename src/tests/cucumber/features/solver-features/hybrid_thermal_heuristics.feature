Feature: hybrid studies - integer strategy of GEMS thermal components

  # GEMS thermal components can be solved with their integer variables (integer-strategy "exact"),
  # linearly relaxed ("relaxed"), or relaxed with a legacy heuristic run between two optimisations
  # ("heuristic" with heuristic-id "fast" or "accurate"). The heuristics are declared per model in
  # optim-config.yml.
  #
  # The studies come from the GemsPy thermal heuristic tests, converted into hybrid studies. Each
  # study declares both heuristics in optim-config.yml:
  #  - fast on antares_legacy_models.thermal_fast (no unit commitment variables), whose output is the
  #    lower bound of generation_power
  #  - accurate on antares_legacy_models.thermal, whose output is the lower bound of num_units_on
  # Every scenario then changes the integer strategy (and the model for "fast") of the components.
  # The expected values are the GemsPy ones; the "num_units_on" and non-proportional costs are not
  # checked in fast, as the thermal_fast model has no such variables.
  #
  # Both optimisations are always run (unit-commitment-mode = fast in the legacy part), so the
  # results are read from the second one.

  Background:
    Given the linear solver is coin

  # ---------------------------------------------------------------------------------------------
  # One cluster: 3 units of 1000 MW (pmin 700 MW), min up 3 h, min down 10 h. The demand is
  # 2000 MW, except 2050 MW at hour 12.
  # ---------------------------------------------------------------------------------------------

  @short
  Scenario: One cluster - exact (MILP): a third unit is started at hour 12 only
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster"
    And in system.yml, component "G" has integer strategy "exact"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 16805387
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 0-11     | 0        | 2000  |
      | 0     | G         | generation_power | 12       | 0        | 2100  |
      | 0     | G         | generation_power | 13-167   | 0        | 2000  |
      | 0     | G         | num_units_on     | 0-11     | 0        | 2     |
      | 0     | G         | num_units_on     | 12       | 0        | 3     |
      | 0     | G         | num_units_on     | 13-167   | 0        | 2     |
      | 0     | G         | non_prop_cost    | 12       | 0        | 53    |
      | 0     | node      | spilled_energy   | 12       | 0        | 50    |
      | 0     | node      | spilled_energy   | 13-167   | 0        | 0     |
      | 0     | node      | unsupplied_energy| 0-167    | 0        | 0     |

  @short
  Scenario: One cluster - relaxed: 2.05 units at hour 12, no spillage
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster"
    And in system.yml, component "G" has integer strategy "relaxed"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 16802838.55
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 12       | 0        | 2050  |
      | 0     | G         | num_units_on     | 12       | 0        | 2.05  |
      | 0     | G         | non_prop_cost    | 12       | 0        | 4.55  |
      | 0     | node      | spilled_energy   | 0-167    | 0        | 0     |

  @short
  Scenario: One cluster - heuristic accurate: same solution as MILP
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster"
    And in system.yml, component "G" has integer strategy "heuristic accurate"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    # First optimisation: relaxed, as in the "relaxed" scenario
    And the modeler outputs from stage "optim-nb-1" contain the following entries with relative tolerance 1e-6
      | block | component | output       | timestep | scenario | value |
      | 0     | G         | num_units_on | 12       | 0        | 2.05  |
    # Second optimisation: num_units_on lower bound = ceil(2.05)
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 16805387
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 12       | 0        | 2100  |
      | 0     | G         | num_units_on     | 0-11     | 0        | 2     |
      | 0     | G         | num_units_on     | 12       | 0        | 3     |
      | 0     | G         | num_units_on     | 13-167   | 0        | 2     |
      | 0     | G         | non_prop_cost    | 12       | 0        | 53    |
      | 0     | node      | spilled_energy   | 12       | 0        | 50    |

  @short
  Scenario: One cluster - heuristic fast: the third unit runs for a whole block of 10 hours
    # Blocks of max(min up, min down) = 10 hours: the third unit is on from hour 10 to 19, its
    # pmin (700 MW) forces 100 MW of spillage at each of these hours (50 MW at hour 12).
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster"
    And in system.yml, component "G" uses model "antares_legacy_models.thermal_fast" with integer strategy "heuristic fast"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 16850000
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 0-9      | 0        | 2000  |
      | 0     | G         | generation_power | 10-19    | 0        | 2100  |
      | 0     | G         | generation_power | 20-167   | 0        | 2000  |
      | 0     | node      | spilled_energy   | 0-9      | 0        | 0     |
      | 0     | node      | spilled_energy   | 10-11    | 0        | 100   |
      | 0     | node      | spilled_energy   | 12       | 0        | 50    |
      | 0     | node      | spilled_energy   | 13-19    | 0        | 100   |
      | 0     | node      | spilled_energy   | 20-167   | 0        | 0     |
    And the modeler outputs contain no "num_units_on" entries for component "G"

  # ---------------------------------------------------------------------------------------------
  # One cluster without minimum generation: the demand is 2050 MW at hour 12 only.
  # ---------------------------------------------------------------------------------------------

  @short
  Scenario: One cluster without pmin - exact (MILP)
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster_no_pmin"
    And in system.yml, component "G" has integer strategy "exact"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 102659
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 12       | 0        | 2050  |
      | 0     | G         | num_units_on     | 12-14    | 0        | 3     |
      | 0     | G         | non_prop_cost    | 12       | 0        | 153   |

  @short
  Scenario: One cluster without pmin - relaxed
    # The hours at which the 2.05 units are on (min up 3 h) are not unique: only hour 12 is checked
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster_no_pmin"
    And in system.yml, component "G" has integer strategy "relaxed"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 102608.65
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 12       | 0        | 2050  |
      | 0     | G         | num_units_on     | 12       | 0        | 2.05  |

  @short
  Scenario: One cluster without pmin - heuristic accurate: same cost as MILP
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster_no_pmin"
    And in system.yml, component "G" has integer strategy "heuristic accurate"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 102659
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 12       | 0        | 2050  |
      | 0     | G         | num_units_on     | 12       | 0        | 3     |

  @short
  Scenario: One cluster without pmin - heuristic fast: no start-up nor fixed cost in the model
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster_no_pmin"
    And in system.yml, component "G" uses model "antares_legacy_models.thermal_fast" with integer strategy "heuristic fast"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 102500
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G         | generation_power | 0-11     | 0        | 0     |
      | 0     | G         | generation_power | 12       | 0        | 2050  |
      | 0     | G         | generation_power | 13-167   | 0        | 0     |

  # ---------------------------------------------------------------------------------------------
  # Two clusters with a low load: the heuristics lead to more spillage than MILP (accurate a bit
  # more, fast much more), visible from the first hour of the week.
  # ---------------------------------------------------------------------------------------------

  @short
  Scenario: Two clusters with a low load - exact (MILP)
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/two_clusters_low_load"
    And in system.yml, component "G1" has integer strategy "exact"
    And in system.yml, component "G2" has integer strategy "exact"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 36036414
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G1        | generation_power | 0        | 0        | 21000 |
      | 0     | G1        | num_units_on     | 0        | 0        | 35    |
      | 0     | G2        | generation_power | 12       | 0        | 7177  |
      | 0     | G2        | num_units_on     | 12       | 0        | 8     |
      | 0     | node      | spilled_energy   | 0        | 0        | 1211  |

  @short
  Scenario: Two clusters with a low load - heuristic accurate on both clusters
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/two_clusters_low_load"
    And in system.yml, component "G1" has integer strategy "heuristic accurate"
    And in system.yml, component "G2" has integer strategy "heuristic accurate"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 36060641
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G1        | generation_power | 0        | 0        | 21600 |
      | 0     | G1        | num_units_on     | 0        | 0        | 36    |
      | 0     | G2        | generation_power | 12       | 0        | 6277  |
      | 0     | G2        | num_units_on     | 12       | 0        | 8     |
      | 0     | node      | spilled_energy   | 0        | 0        | 1811  |

  @short
  Scenario: Two clusters with a low load - heuristic fast on both clusters
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/two_clusters_low_load"
    And in system.yml, component "G1" uses model "antares_legacy_models.thermal_fast" with integer strategy "heuristic fast"
    And in system.yml, component "G2" uses model "antares_legacy_models.thermal_fast" with integer strategy "heuristic fast"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    And the modeler outputs are read from stage "optim-nb-2"
    And the objective value is 35774633
    And the modeler outputs contain the following entries
      | block | component | output           | timestep | scenario | value |
      | 0     | G1        | generation_power | 0        | 0        | 24600 |
      | 0     | G2        | generation_power | 12       | 0        | 0     |
      | 0     | node      | spilled_energy   | 0        | 0        | 4811  |

  # ---------------------------------------------------------------------------------------------
  # Four clusters, all integer strategies in the same study: G1 relaxed, G2 heuristic fast,
  # G3 exact and G4 heuristic accurate. The GemsPy test lasts 24 hours: here the same day is
  # simulated 7 times.
  # ---------------------------------------------------------------------------------------------

  @short
  Scenario: Four clusters with mixed integer strategies
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/four_clusters_mixed_strategies"
    When I run antares simulator with --output=all
    Then the simulation succeeds
    # First optimisation: G4 (accurate) is relaxed
    And the modeler outputs from stage "optim-nb-1" contain the following entries with relative tolerance 1e-6
      | block | component | output       | timestep | scenario | value      |
      | 0     | G4        | num_units_on | 14       | 0        | 0.24666667 |
      | 0     | G4        | num_units_on | 15       | 0        | 0.33333333 |
    And the modeler outputs are read from stage "optim-nb-2"
    # 7 times the objective of the 24 hour GemsPy test (537 680)
    And the objective value is 3763760
    And the modeler outputs contain the following entries
      | block | component | output            | timestep | scenario | value |
      | 0     | G1        | num_units_on      | 0-167    | 0        | 4     |
      | 0     | G1        | generation_power  | 3        | 0        | 400   |
      | 0     | G1        | generation_power  | 12       | 0        | 1200  |
      | 0     | G2        | generation_power  | 6        | 0        | 80    |
      | 0     | G2        | generation_power  | 12       | 0        | 750   |
      | 0     | G3        | generation_power  | 15       | 0        | 600   |
      | 0     | G3        | num_units_on      | 13-17    | 0        | 3     |
      | 0     | G4        | generation_power  | 14-16    | 0        | 50    |
      | 0     | G4        | num_units_on      | 13       | 0        | 0     |
      | 0     | G4        | num_units_on      | 14-16    | 0        | 1     |
      | 0     | G4        | num_units_on      | 17       | 0        | 0     |
      | 0     | node      | spilled_energy    | 0-167    | 0        | 0     |
      | 0     | node      | unsupplied_energy | 0-167    | 0        | 0     |

  # ---------------------------------------------------------------------------------------------
  # Configuration errors
  # ---------------------------------------------------------------------------------------------

  @short
  Scenario: A heuristic must be declared in optim-config.yml for the model of the component
    # optim-config.yml declares "fast" for thermal_fast only
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster"
    And in system.yml, component "G" has integer strategy "heuristic fast"
    When I run antares simulator
    Then the simulation fails
    And the message "Component 'G': heuristic 'fast' is not declared in optim-config.yml for model 'antares_legacy_models.thermal'" is reported in the logs

  @short
  Scenario: heuristic-id is mandatory with the integer strategy heuristic
    Given the solver study path is a copy of "Antares_Simulator_Tests_NR/hybrid/thermal_heuristics/one_cluster"
    And in system.yml, component "G" has integer strategy "heuristic"
    When I run antares simulator
    Then the simulation fails
    And the message "Component 'G': 'heuristic-id' is mandatory when integer strategy is 'heuristic'" is reported in the logs
