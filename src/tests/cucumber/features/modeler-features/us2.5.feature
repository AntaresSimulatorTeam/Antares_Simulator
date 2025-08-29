Feature: 2.5 - Pure modeler simple studies, with no ports and no timeseries

  @fast
  Scenario: 2.5.1: One model with one load and two generators, one timestep
    Given the modeler study path is "modeler/epic2/us2.5/study_2.5.1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | node1     | gen1_p | 1        | 80    |
      | node1     | gen2_p | 1        | 20    |

  @fast
  Scenario: 2.5.2: One model with one load and two generators (minP), three timesteps
    Given the modeler study path is "modeler/epic2/us2.5/study_2.5.2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 810
    And the modeler outputs contain the following entries
      | component | output  | timestep | value |
      | node1     | gen1_up | 1-3      | 1     |
      | node1     | gen1_p  | 1-3      | 60    |
      | node1     | gen1_up | 1-3      | 1     |
      | node1     | gen2_p  | 1-3      | 40    |

  @fast
  Scenario: 2.5.3: Two libs, one timestep
    Given the modeler study path is "modeler/epic2/us2.5/study_2.5.3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 15600
    And the modeler outputs contain the following entries
      | component | output  | timestep | value |
      | node1     | gen1_p  | 1        | 0     |
      | node1     | gen2_p  | 1        | 100   |
      | node2     | gen1_p  | 1        | 500   |
      | node2     | gen1_up | 1        | 1     |
      | node2     | gen2_p  | 1        | 500   |
      | node2     | gen2_up | 1        | 1     |

  @fast
  Scenario: 2.5.4: Test with integer variable
    Given the modeler study path is "modeler/epic2/us2.5/study_2.5.4"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 540
    And the modeler outputs contain the following entries
      | component | output      | timestep | value |
      | node1     | gen_total_p | 1        | 1000  |
      | node1     | gen_n_on    | 1        | 4     |