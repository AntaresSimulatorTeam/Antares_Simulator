Feature: Simple modeler systems

  @short
  Scenario: cyclic
    Given the modeler study path is "modeler/simple_system_cyclic"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 1500

  @short
  Scenario: drop
    Given the modeler study path is "modeler/simple_system_drop"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 300
