Feature: Investment add-on plugged on an operational model

  # The operational model caps its generation with
  # sum_connections(capacity_port.capacity, no_connection_expr = p_max_cluster)

  @short
  Scenario: Operational model without add-on uses its own capacity
    Given the modeler study path is "modeler/investment_addon_not_connected"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 121800

  @short
  Scenario: Operational model with investment add-on uses the invested capacity
    Given the modeler study path is "modeler/investment_addon_connected"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 8000
