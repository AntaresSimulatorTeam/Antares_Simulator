Feature: 10 - Modeler extra outputs

  @short
  Scenario: 10.1: Extra-outputs, basic features
    Given the modeler study path is "modeler/10_1"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component      | output                  | timestep | scenario | value  |
      | 0     | base_zone      | spill_cost_contribution | 0-4      | 0        | 0      |
      | 0     | base_zone      | ens_cost_contribution   | 0-3      | 0        | 0      |
      | 0     | base_zone      | ens_cost_contribution   | 4        | 0        | 200000 |
      | 0     | load_base_zone | load_parameter          | 0        | 0        | 20     |
      | 0     | load_base_zone | load_parameter          | 1        | 0        | 40     |
      | 0     | load_base_zone | load_parameter          | 2        | 0        | 60     |
      | 0     | load_base_zone | load_parameter          | 3        | 0        | 80     |
      | 0     | load_base_zone | load_parameter          | 4        | 0        | 110    |
      | 0     | gas_base_zone  | generation_cost         | 0        | 0        | 40     |
      | 0     | gas_base_zone  | generation_cost         | 1        | 0        | 80     |
      | 0     | gas_base_zone  | generation_cost         | 2        | 0        | 120    |
      | 0     | gas_base_zone  | generation_cost         | 3        | 0        | 160    |
      | 0     | gas_base_zone  | generation_cost         | 4        | 0        | 200    |
      | 0     | gas_base_zone  | squared_generation_cost | 0        | 0        | 1600   |
      | 0     | gas_base_zone  | squared_generation_cost | 1        | 0        | 6400   |
      | 0     | gas_base_zone  | squared_generation_cost | 2        | 0        | 14400  |
      | 0     | gas_base_zone  | squared_generation_cost | 3        | 0        | 25600  |
      | 0     | gas_base_zone  | squared_generation_cost | 4        | 0        | 40000  |
      | 0     | gas_base_zone  | total_generation_cost   |          | 0        | 600    |

  @short
  Scenario: 10.2: Extra-outputs, basic features
    Given the modeler study path is "modeler/10_2"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 0     | base_zone         | spill_cost_contribution | 0-4      | 0        | 0     |
      | 0     | base_zone         | ens_cost_contribution   | 0-4      | 0        | 0     |
      | 0     | load_base_zone    | load_parameter          | 0-3      | 0        | 120   |
      | 0     | load_base_zone    | load_parameter          | 4        | 0        | 20    |
      | 0     | gas_base_zone     | generation_cost         | 0-4      | 0        | 200   |
      | 0     | gas_base_zone     | squared_generation_cost | 0-4      | 0        | 40000 |
      | 0     | gas_base_zone     | total_generation_cost   |          | 0        | 1000  |
      | 0     | storage_base_zone | soc_rate                | 0        | 0        | 1     |
      | 0     | storage_base_zone | soc_rate                | 1        | 0        | 0.8   |
      | 0     | storage_base_zone | soc_rate                | 2        | 0        | 0.6   |
      | 0     | storage_base_zone | soc_rate                | 3        | 0        | 0.4   |
      | 0     | storage_base_zone | soc_rate                | 4        | 0        | 0.2   |
      | 0     | storage_base_zone | cumulative_withdrawal   |          | 0        | 80    |
      | 0     | storage_base_zone | constant_expression     |          | 0        | 1     |

  @short
  Scenario: 10.3: Extra-outputs, with timeshift operator
    Given the modeler study path is "modeler/10_3"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 0     | base_zone         | spill_cost_contribution | 0-4      | 0        | 0     |
      | 0     | base_zone         | ens_cost_contribution   | 0-4      | 0        | 0     |
      | 0     | load_base_zone    | load_parameter          | 0-3      | 0        | 120   |
      | 0     | load_base_zone    | load_parameter          | 4        | 0        | 20    |
      | 0     | gas_base_zone     | generation_cost         | 0-4      | 0        | 200   |
      | 0     | gas_base_zone     | squared_generation_cost | 0-4      | 0        | 40000 |
      | 0     | gas_base_zone     | total_generation_cost   |          | 0        | 1000  |
      | 0     | storage_base_zone | soc_rate                | 0        | 0        | 1     |
      | 0     | storage_base_zone | soc_rate                | 1        | 0        | 0.8   |
      | 0     | storage_base_zone | soc_rate                | 2        | 0        | 0.6   |
      | 0     | storage_base_zone | soc_rate                | 3        | 0        | 0.4   |
      | 0     | storage_base_zone | soc_rate                | 4        | 0        | 0.2   |
      | 0     | storage_base_zone | soc_variation           | 0-3      | 0        | -20   |
      | 0     | storage_base_zone | soc_variation           | 4        | 0        | 80    |
      | 0     | storage_base_zone | cumulative_withdrawal   |          | 0        | 80    |
      | 0     | storage_base_zone | constant_expression     |          | 0        | 1     |

  @short
  Scenario: 10.4: Extra-outputs, with comparison operator
    Given the modeler study path is "modeler/10_4"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 0     | base_zone         | unsupplied_energy       | 0-3      | 0        | 0     |
      | 0     | base_zone         | unsupplied_energy       | 4        | 0        | 10    |
      | 0     | base_zone         | loss_of_load            | 0-3      | 0        | 0     |
      | 0     | base_zone         | loss_of_load            | 4        | 0        | 1     |

  @short
  Scenario: 10.5: Extra-outputs, with dual and reduced_cost, single timestep
    Given the modeler study path is "modeler/10_5"
    When I run antares modeler
    Then the simulation succeeds
    # price is dual of balance
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 0     | base_zone         | price                   | 0        | 0        | 10    |
      | 0     | gas_base_zone     | generation_reduced_cost | 0        | 0        | 0     |
      | 0     | oil_base_zone     | generation_reduced_cost | 0        | 0        | 40    |

  @short
  Scenario: 10.5-1: Extra-outputs, with dual and reduced_cost, single timestep
    Given the modeler study path is "modeler/10_5_1"
    When I run antares modeler
    Then the simulation succeeds
    # price is dual of balance
    And the modeler outputs contain the following entries
      | block | component         | output                  | timestep | scenario | value |
      | 0     | base_zone         | price                   | 0        | 0        | 50    |
      | 0     | gas_base_zone     | generation_reduced_cost | 0        | 0        | -40   |
      | 0     | oil_base_zone     | generation_reduced_cost | 0        | 0        | 0     |

  @short
  Scenario: 10.5-2: Extra-outputs, with dual and reduced_cost
    Given the modeler study path is "modeler/10_5_2"
    When I run antares modeler
    Then the simulation succeeds
    # price is dual of balance
    And the modeler outputs contain the following entries
      | block | component     | output                  | timestep | scenario | value  |
      | 0     | base_zone     | price                   | 0        | 0        | 10     |
      | 0     | base_zone     | price                   | 1        | 0        | 15     |
      | 0     | base_zone     | price                   | 2        | 0        | 20000  |
      | 0     | gas_base_zone | generation_reduced_cost | 0        | 0        | 0      |
      | 0     | gas_base_zone | generation_reduced_cost | 1        | 0        | 0      |
      | 0     | gas_base_zone | generation_reduced_cost | 2        | 0        | -19960 |
      | 0     | oil_base_zone | generation_reduced_cost | 0        | 0        | 20     |
      | 0     | oil_base_zone | generation_reduced_cost | 1        | 0        | -5     |
      | 0     | oil_base_zone | generation_reduced_cost | 2        | 0        | -19990 |

  @short
  Scenario: 10.6: be able to compute the income of a thermal cluster, through a port.
    Given the modeler study path is "modeler/10_6"
    When I run antares modeler
    Then the simulation succeeds
    And the modeler outputs contain the following entries
      | block | component         | output                    | timestep | scenario | value |
      | 0     | base_zone         | balance_port.price        | 0        | 0        | 10    |
      | 0     | gas_base_zone     | dual(balance) from area   | 0        | 0        | 10    |
	  | 0     | gas_base_zone     | income                    | 0        | 0        | 0     |