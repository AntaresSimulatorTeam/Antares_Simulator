Feature: 6 - Simple end-to-end tests to test port management

  @fast
  Scenario: 6.1: One system with three components: one balance node, one load and one 2 gen-component
    Given the modeler study path is "modeler/6_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the modeler outputs contain the following entries
      | block | component | output              | timestep | scenario | value |
      | 1     | NG        | gen1_p              | 1        | 0        | 80    |
      | 1     | NG        | gen2_p              | 1        | 0        | 20    |
      | 1     | NG        | injection_port.flow | 1        | 0        | 100   |
      | 1     | NL        | injection_port.flow |          |          | -100  |

  @fast
  Scenario: 6.2: test system with 3 components : one balance node, one load (100 MW) and one component with two generators (with Pmax_1 = 80 MW & Pmax_2 = 200 MW) ; gen1 has a fixed cost. Two time steps.
    Given the modeler study path is "modeler/6_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 765
    And the modeler outputs contain the following entries
      | component | output              | timestep | scenario | value |
      | NG        | gen1_p              | 1        | 0        | 80    |
      | NG        | gen2_p              | 1        | 0        | 20    |
      | NG        | gen1_p              | 2        | 0        | 0     |
      | NG        | gen2_p              | 2        | 0        | 100   |
      | NG        | on_off_gen1_p       | 1        | 0        | 1     |
      | NG        | on_off_gen1_p       | 2        | 0        | 0     |
      | NG        | injection_port.flow | 1-2      | 0        | 100   |
      | NL        | injection_port.flow |          |          | -100  |

  @fast
  Scenario: 6.3: test system with 4 components : one balance node, one load (100 MW), two components of generators (with Pmax_1 = 80 MW and Pmax_2 = 200 MW). Two time steps.
    Given the modeler study path is "modeler/6_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 765
    And the modeler outputs contain the following entries
      | component | output              | timestep | scenario | value |
      | NG1       | gen                 | 1        | 0        | 80    |
      | NG2       | gen                 | 1        | 0        | 20    |
      | NG1       | gen                 | 2        | 0        | 0     |
      | NG2       | gen                 | 2        | 0        | 100   |
      | NG1       | on_off_gen          | 1        | 0        | 1     |
      | NG1       | on_off_gen          | 2        | 0        | 0     |
      | NG1       | injection_port.flow | 1        | 0        | 80    |
      | NG1       | injection_port.flow | 2        | 0        | 0     |
      | NG2       | injection_port.flow | 1        | 0        | 20    |
      | NG2       | injection_port.flow | 2        | 0        | 100   |
      | NL        | injection_port.flow |          |          | -100  |

  @fast
  Scenario: 6.1 - with scenario builder: Same as 6.1, but the load's first MC year is linked to 2nd timeseries in load.tsv
    Given the modeler study path is "modeler/6_1_scenariobuilder"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the modeler outputs contain the following entries
      | component | output              | timestep | scenario | value |
      | NG        | gen1_p              | 1        | 0        | 80    |
      | NG        | gen2_p              | 1        | 0        | 20    |
      | NG        | injection_port.flow | 1        | 0        | 100   |
      | NL        | injection_port.flow | 1        | 0        | -100  |
