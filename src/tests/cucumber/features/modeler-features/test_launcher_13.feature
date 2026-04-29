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

  @fast
  Scenario: 13_2: Simple generation expansion problem on one node, one timestep and one scenario with two candidates: one continuous and one discrete.
    Given the modeler study path is "modeler/13_2"
    When I run antares modeler
    Then the simulation succeeds
    And the master problem contains the following variables
      | name                                  | xmin | xmax | cost |
      | continuous_generator_candidate.p_max  | 0    | 1000 | 490  |
      | discrete_generator_candidate.p_max    | 0    | inf  | 200  |
      | discrete_generator_candidate.nb_units | 0    | 10   | 0    |
    And the subproblem contains the following variables
      | name                                         | xmin | xmax | cost |
      | my_node.spillage_s0_t0                       | 0    | 1e6  | 1    |
      | my_node.unsupplied_energy_s0_t0              | 0    | 1e6  | 501  |
      | already_installed_generator.generation_s0_t0 | 0    | 200  | 45   |
    And the structure file contains the following entries
      | problem | variable                             | index |
      | master  | continuous_generator_candidate.p_max | 0     |
      | master  | discrete_generator_candidate.p_max   | 1     |
      | 1-1     | continuous_generator_candidate.p_max | 3     |
      | 1-1     | discrete_generator_candidate.p_max   | 5     |
