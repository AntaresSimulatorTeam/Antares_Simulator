Feature: short tests

  @fast @short
  Scenario: 001 One node - passive
    Given the study path is "short-tests/001 One node - passive"
    When I run antares simulator
    Then the simulation takes less than 1 seconds
    And the simulation succeeds
    And the annual system cost is
      | EXP | STD | MIN | MAX |
      | 0   | 0   | 0   | 0   |

  @fast @short
  Scenario: 002 Thermal fleet - Base
    Given the study path is "short-tests/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation takes less than 1 seconds
    And the simulation succeeds
    And the annual system cost is
      | EXP      | STD | MIN      | MAX      |
      | 2.729e+7 | 0   | 2.729e+7 | 2.729e+7 |
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "02 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: 003 Thermal fleet - Must-run
    Given the study path is "short-tests/003 Thermal fleet - Must-run"
    When I run antares simulator
    Then the simulation takes less than 1 seconds
    And the simulation succeeds
    And the annual system cost is
      | EXP      | STD | MIN      | MAX      |
      | 2.751e+7 | 0   | 2.751e+7 | 2.751e+7 |
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "02 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: 021 Four areas - DC law
    Given the study path is "short-tests/021 Four areas - DC law"
    When I run antares simulator
    Then the simulation takes less than 15 seconds
    And the simulation succeeds
    And the annual system cost is
      | EXP       | STD       | MIN       | MAX       |
      | 7.972e+10 | 2.258e+10 | 5.613e+10 | 1.082e+11 |