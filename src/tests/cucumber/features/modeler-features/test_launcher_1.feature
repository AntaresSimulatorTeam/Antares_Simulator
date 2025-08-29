Feature: 1 - Simple end-to-end tests to test temporal expression management

  @fast
  Scenario: 1.1: One model with one load and two generators, two timesteps
    Given the modeler study path is "modeler/1_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 320
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1-2      | 80    |
      | node1     | gen2_p | 1-2      | 20    |

  @fast
  Scenario: 1.2: One model with one load and two generators, two timesteps ; mix of constant parameters and timeseries
    Given the modeler study path is "modeler/1_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 320
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1-2      | 80    |
      | node1     | gen2_p | 1-2      | 20    |

  @fast
  Scenario: 1.3: One model with one load and two generators, one timestep ; fails because of timeseries of different lengths
    Given the modeler study path is "modeler/1_3"
    When I run antares modeler
    Then the simulation fails


  @fast
  Scenario: 1.4: test system with one load (100 MW) and two generators in the same node (with Pmax_1 = 80 MW & Pmax_2 = 200 MW) ; gen1 has a fixed switch on cost. Two timesteps.
    Given the modeler study path is "modeler/1_4"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 765
    And the modeler outputs contain the following entries
      | component | output        | timestep | value |
      | node1     | gen1_p        | 1        | 80    |
      | node1     | gen2_p        | 1        | 20    |
      | node1     | gen1_p        | 2        | 0     |
      | node1     | gen2_p        | 2        | 100   |
      | node1     | on_off_gen1_p | 1        | 1     |
      | node1     | on_off_gen1_p | 2        | 0     |


  @fast
  Scenario: 1.5: One model with one load and two generators, one timestep ; fails because of timeseries given for parameter with time_dependant = false
    Given the modeler study path is "modeler/1_5"
    When I run antares modeler
    Then the simulation fails

  @fast
  Scenario: 1.6: One model with one load and two generators, 100 timesteps
    Given the modeler study path is "modeler/1_6"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 16000
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1-100    | 80    |
      | node1     | gen2_p | 1-100    | 20    |