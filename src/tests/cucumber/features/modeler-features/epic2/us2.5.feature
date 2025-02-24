Feature: 2.5 - Pure modeler simple studies, with no ports and no timeseries

  Scenario: 2.5.1: One model with one load and two generators, one timestep
    Given the study path is "modeler/epic2/us2.5/study_2.5.1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 160
    And the optimal value of variable node1.gen1_p_0 is 80
    And the optimal value of variable node1.gen2_p_0 is 20

  Scenario: 2.5.2: One model with one load and two generators (minP), three timesteps
    Given the study path is "modeler/epic2/us2.5/study_2.5.2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 810
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_up  | 0-2       | 1     |
      | node1     | gen1_p   | 0-2       | 60    |
      | node1     | gen1_up  | 0-2       | 1     |
      | node1     | gen2_p   | 0-2       | 40    |

  Scenario: 2.5.3: Two libs, one timestep
    Given the study path is "modeler/epic2/us2.5/study_2.5.3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 15600
    And the optimal values of the variables are
      | component | variable | timestep | value |
      | node1     | gen1_p   | 0         | 0     |
      | node1     | gen2_p   | 0         | 100   |
      | node2     | gen1_p   | 0         | 500   |
      | node2     | gen1_up  | 0         | 1     |
      | node2     | gen2_p   | 0         | 500   |
      | node2     | gen2_up  | 0         | 1     |

  Scenario: 2.5.4: Test with integer variable
    Given the study path is "modeler/epic2/us2.5/study_2.5.4"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 540
    And the optimal value of variable node1.gen_total_p_0 is 1000
    And the optimal value of variable node1.gen_n_on_0 is 4