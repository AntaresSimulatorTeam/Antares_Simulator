Feature: 13 - Investment problems

  @fast
  Scenario: 13_1: Simple investment - variables & objective constributions
    Given the modeler study path is "modeler/13_1"
    When I run antares modeler
    Then the simulation succeeds
    And the master problem contains the following variables
      | name                                 | xmin | xmax | cost |
      | continuous_generator_candidate.p_max | 0    | 1000 | 400  |
    And the subproblem contains the following variables
      | name                            | xmin | xmax | cost |
      | my_node.spillage_s0_t0          | 0    | 1e6  | 1    |
      | my_node.unsupplied_energy_s0_t0 | 0    | 1e6  | 501  |
    And the structure file contains the following entries
      | problem | variable                             | index |
      | master  | continuous_generator_candidate.p_max | 0     |
      | 1-1     | continuous_generator_candidate.p_max | 3     |