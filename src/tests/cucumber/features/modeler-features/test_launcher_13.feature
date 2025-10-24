Feature: 13 - Invest in object


    # Simple generation expansion problem on one node, one timestep and one scenario with one candidate.

    # Demand = 400
    # Generator : P_max : 200, Cost : 45
    # Unsupplied energy : Cost : 501

    # -> 200 of unsupplied energy
    # -> Total cost without investment = 45 * 200 + 501 * 200 = 109_200

    # Continuous candidate  : Invest cost : 400 / MW; Prod cost : 10

    # Optimal investment : 200 MW

    # -> Optimal cost = 400 * 200 + 10 * 200 (Invest cost + prod cost of new generator)
    #                             + 45 * 200 (Generator)
    #                 =    80_000 +   11_000
    #                 = 91_000
  @fast
  Scenario: 13.1: One system with one electric node (1 thermal cluster, 1 load) and one candidate with continuous invest option
    Given the modeler study path is "modeler/13_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is greater than 90999 and lower than 91001
    And the master problem contains the following variables
      | name              | objective coef |
      | CAND_p_max        | 490            |
      | DISCRETE_p_max    | 200            |
      | DISCRETE_nb_units |                |

    # Study case 13_2 : to test investment problems
    # Simple generation expansion problem on one node, one timestep and one scenario with two candidates: one continuous and one discrete.

    # Demand = 400
    # Generator : P_max : 200, Cost : 45
    # Unsupplied energy : Cost : 501

    # -> 200 of unsupplied energy
    # -> Total cost without investment = 45 * 200 + 501 * 200 = 109_200

    # Continuous candidate  : Invest cost : 490 / MW; Prod cost : 10
    # Discrete candidate : Invest cost : 200 / MW; Prod cost : 10; Nb of units: 10; Prod per unit: 10

    # Optimal investment : 100 MW (Discrete) + 100 MW (Continuous)

    # -> Optimal cost = 490 * 100 + 10 * 100 (Continuous)
    #                 + 200 * 100 + 10 * 100 (Discrete)
    #                             + 45 * 200 (Generator)
    #                 =    69_000 +   11_000
    #                 = 80_000
  @fast
  Scenario: 13.2: One system with one electric node (1 thermal cluster, 1 load) and two candidates : one with continuous invest option and one with discrete invest option
    Given the modeler study path is "modeler/13_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is greater than 79999 and lower than 80001