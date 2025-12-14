Feature: 10 - Modeler extra outputs

  @fast
  Scenario: 10.1: Extra-outputs, basic features
    Given the modeler study path is "modeler/10_1"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component      | output                  | timestep | scenario | value  |
      | 1     | base_zone      | spill_cost_contribution | 1-5      | 0        | 0      |
      | 1     | base_zone      | ens_cost_contribution   | 1-4      | 0        | 0      |
      | 1     | base_zone      | ens_cost_contribution   | 5        | 0        | 200000 |
      | 1     | load_base_zone | load_parameter          | 1        | 0        | 20     |
      | 1     | load_base_zone | load_parameter          | 2        | 0        | 40     |
      | 1     | load_base_zone | load_parameter          | 3        | 0        | 60     |
      | 1     | load_base_zone | load_parameter          | 4        | 0        | 80     |
      | 1     | load_base_zone | load_parameter          | 5        | 0        | 110    |
      | 1     | gas_base_zone  | generation_cost         | 1        | 0        | 40     |
      | 1     | gas_base_zone  | generation_cost         | 2        | 0        | 80     |
      | 1     | gas_base_zone  | generation_cost         | 3        | 0        | 120    |
      | 1     | gas_base_zone  | generation_cost         | 4        | 0        | 160    |
      | 1     | gas_base_zone  | generation_cost         | 5        | 0        | 200    |
      | 1     | gas_base_zone  | squared_generation_cost | 1        | 0        | 1600   |
      | 1     | gas_base_zone  | squared_generation_cost | 2        | 0        | 6400   |
      | 1     | gas_base_zone  | squared_generation_cost | 3        | 0        | 14400  |
      | 1     | gas_base_zone  | squared_generation_cost | 4        | 0        | 25600  |
      | 1     | gas_base_zone  | squared_generation_cost | 5        | 0        | 40000  |
      | 1     | gas_base_zone  | total_generation_cost   |          | 0        | 600    |

  @fast
  Scenario: 10.2: Extra-outputs, basic features
    Given the modeler study path is "modeler/10_2"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 1     | base_zone         | spill_cost_contribution | 1-5      | 0        | 0     |
      | 1     | base_zone         | ens_cost_contribution   | 1-5      | 0        | 0     |
      | 1     | load_base_zone    | load_parameter          | 1-4      | 0        | 120   |
      | 1     | load_base_zone    | load_parameter          | 5        | 0        | 20    |
      | 1     | gas_base_zone     | generation_cost         | 1-5      | 0        | 200   |
      | 1     | gas_base_zone     | squared_generation_cost | 1-5      | 0        | 40000 |
      | 1     | gas_base_zone     | total_generation_cost   |          | 0        | 1000  |
      | 1     | storage_base_zone | soc_rate                | 1        | 0        | 1     |
      | 1     | storage_base_zone | soc_rate                | 2        | 0        | 0.8   |
      | 1     | storage_base_zone | soc_rate                | 3        | 0        | 0.6   |
      | 1     | storage_base_zone | soc_rate                | 4        | 0        | 0.4   |
      | 1     | storage_base_zone | soc_rate                | 5        | 0        | 0.2   |
      | 1     | storage_base_zone | cumulative_withdrawal   |          | 0        | 80    |
      | 1     | storage_base_zone | constant_expression     |          | 0        | 1     |

  @fast
  Scenario: 10.3: Extra-outputs, with timeshift operator
    Given the modeler study path is "modeler/10_3"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 1     | base_zone         | spill_cost_contribution | 1-5      | 0        | 0     |
      | 1     | base_zone         | ens_cost_contribution   | 1-5      | 0        | 0     |
      | 1     | load_base_zone    | load_parameter          | 1-4      | 0        | 120   |
      | 1     | load_base_zone    | load_parameter          | 5        | 0        | 20    |
      | 1     | gas_base_zone     | generation_cost         | 1-5      | 0        | 200   |
      | 1     | gas_base_zone     | squared_generation_cost | 1-5      | 0        | 40000 |
      | 1     | gas_base_zone     | total_generation_cost   |          | 0        | 1000  |
      | 1     | storage_base_zone | soc_rate                | 1        | 0        | 1     |
      | 1     | storage_base_zone | soc_rate                | 2        | 0        | 0.8   |
      | 1     | storage_base_zone | soc_rate                | 3        | 0        | 0.6   |
      | 1     | storage_base_zone | soc_rate                | 4        | 0        | 0.4   |
      | 1     | storage_base_zone | soc_rate                | 5        | 0        | 0.2   |
      | 1     | storage_base_zone | soc_variation           | 1-4      | 0        | -20   |
      | 1     | storage_base_zone | soc_variation           | 5        | 0        | 80    |
      | 1     | storage_base_zone | cumulative_withdrawal   |          | 0        | 80    |
      | 1     | storage_base_zone | constant_expression     |          | 0        | 1     |

  @fast
  Scenario: 10.4: Extra-outputs, with comparison operator
    Given the modeler study path is "modeler/10_4"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 1     | base_zone         | unsupplied_energy       | 1-4      | 0        | 0     |
      | 1     | base_zone         | unsupplied_energy       | 5        | 0        | 10    |
      | 1     | base_zone         | loss_of_load            | 1-4      | 0        | 0     |
      | 1     | base_zone         | loss_of_load            | 5        | 0        | 1     |

  @fast
  Scenario: 10.5: Extra-outputs, with dual and reduced_cost, single timestep
    Given the modeler study path is "modeler/10_5"
    When I run antares modeler
    Then the simulation succeeds
    # price is dual of balance
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 1     | base_zone         | price                   | 1        | 0        | 10    |
      | 1     | gas_base_zone     | generation_reduced_cost | 1        | 0        | 0     |
      | 1     | oil_base_zone     | generation_reduced_cost | 1        | 0        | 40    |

  @fast
  Scenario: 10.5-1: Extra-outputs, with dual and reduced_cost, single timestep
    Given the modeler study path is "modeler/10_5_1"
    When I run antares modeler
    Then the simulation succeeds
    # price is dual of balance
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 1     | base_zone         | price                   | 1        | 0        | 50    |
      | 1     | gas_base_zone     | generation_reduced_cost | 1        | 0        | -40   |
      | 1     | oil_base_zone     | generation_reduced_cost | 1        | 0        | 0     |

  @fast
  Scenario: 10.5-2: Extra-outputs, with dual and reduced_cost
    Given the modeler study path is "modeler/10_5_2"
    When I run antares modeler
    Then the simulation succeeds
    # price is dual of balance
    And the modeler outputs contain the following entries
      | block | component     | output                  | timestep | scenario | value  |
      | 1     | base_zone     | price                   | 1        | 0        | 10     |
      | 1     | base_zone     | price                   | 2        | 0        | 15     |
      | 1     | base_zone     | price                   | 3        | 0        | 20000  |
      | 1     | gas_base_zone | generation_reduced_cost | 1        | 0        | 0      |
      | 1     | gas_base_zone | generation_reduced_cost | 2        | 0        | 0      |
      | 1     | gas_base_zone | generation_reduced_cost | 3        | 0        | -19960 |
      | 1     | oil_base_zone | generation_reduced_cost | 1        | 0        | 20     |
      | 1     | oil_base_zone | generation_reduced_cost | 2        | 0        | -5     |
      | 1     | oil_base_zone | generation_reduced_cost | 3        | 0        | -19990 |
