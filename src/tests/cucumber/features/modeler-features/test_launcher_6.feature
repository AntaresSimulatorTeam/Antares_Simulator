Feature: 6 - Simple end-to-end tests to test port management

  @fast
  Scenario: 6.1: One system with three components: one balance node, one load and one 2 gen-component
    Given the modeler study path is "modeler/6_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | NG        | gen1_p   | 0        | 80    |
      | NG        | gen2_p   | 0        | 20    |

  @fast
  Scenario: 6.2: test system with 3 components : one balance node, one load (100 MW) and one component with two generators (with Pmax_1 = 80 MW & Pmax_2 = 200 MW) ; gen1 has a fixed cost. Two time steps.
    Given the modeler study path is "modeler/6_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 765
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | NG     | gen1_p      | 0     | 80    |
      | NG     | gen2_p      | 0     | 20    |
      | NG     | gen1_p      | 1     | 0    |
      | NG     | gen2_p      | 1     | 100    |
      | NG     | on_off_gen1_p   | 0   | 1    |
      | NG     | on_off_gen1_p   | 1     | 0    |

  @fast
  Scenario: 6.3: test system with 4 components : one balance node, one load (100 MW), two components of generators (with Pmax_1 = 80 MW and Pmax_2 = 200 MW). Two time steps.
    Given the modeler study path is "modeler/6_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 765
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | NG1     | gen        | 0     | 80    |
      | NG2     | gen        | 0     | 20    |
      | NG1     | gen         | 1     | 0    |
      | NG2     | gen         | 1     | 100    |
      | NG1     | on_off_gen   | 0   | 1    |
      | NG1     | on_off_gen   | 1     | 0    |