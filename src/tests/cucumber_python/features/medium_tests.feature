Feature: medium tests

  @fast
  Scenario: 035 Mixed Expansion - Smart grid model 2
    Given the study path is "medium-tests/035 Mixed Expansion - Smart grid model 2"
    When I run antares simulator
    Then the simulation succeeds
    And the expected value of the annual system cost is 3.725e+10
    And the minimum annual system cost is 3.642e+10
    And the maximum annual system cost is 4.011e+10
    And the annual system cost is
        | EXP       | MIN       | MAX       |
        | 3.725e+10 | 3.642e+10 | 4.011e+10 |