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

  @fast
  Scenario: 1.7: One variable bounded below by a time sum with fixed lower bound and t+ upper bound
    Given the modeler study path is "modeler/14_1"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 50
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1        | 3     |
      | c1        | x      | 2        | 6     |
      | c1        | x      | 3        | 10    |
      | c1        | x      | 4        | 15    |
      | c1        | x      | 5        | 16    |

  @fast
  Scenario: 1.8: Both bounds fixed - sum always accesses the same indices regardless of current timestep
    Given the modeler study path is "modeler/14_2"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 90
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1-5      | 18    |

  @fast
  Scenario: 1.9: Both bounds t-relative - symmetric two-element sliding window (regression test)
    Given the modeler study path is "modeler/14_3"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 30
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1        | 3     |
      | c1        | x      | 2        | 5     |
      | c1        | x      | 3        | 7     |
      | c1        | x      | 4        | 9     |
      | c1        | x      | 5        | 6     |

  @fast
  Scenario: 1.10: Inverted t+ bounds (from > to) with constant parameter - detects constant-branch bug
    Given the modeler study path is "modeler/14_4"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 100
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1-5      | 20    |

  @fast
  Scenario: 1.11: Fixed bounds with cyclic wrap-around - upper index exceeds window size
    Given the modeler study path is "modeler/14_5"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 60
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1-5      | 12    |

  @fast
  Scenario: 1.12: Negative t+ lower bound - three-element lookback window with cyclic wrap at start
    Given the modeler study path is "modeler/14_6"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 45
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1        | 10    |
      | c1        | x      | 2        | 8     |
      | c1        | x      | 3        | 6     |
      | c1        | x      | 4        | 9     |
      | c1        | x      | 5        | 12    |

  @fast
  Scenario: 1.13: Inverted fixed bounds (from > to) with constant parameter - empty sum should give zero
    Given the modeler study path is "modeler/14_7"
    When I run antares modeler
    Then the simulation succeeds
    And the objective value is 100
    And the modeler outputs contain the following entries
      | component | output | timestep | value |
      | c1        | x      | 1-5      | 20    |
