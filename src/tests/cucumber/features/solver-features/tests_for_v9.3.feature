Feature: tests for v9.3

  @fast @short
  Scenario: scenarized STS additional constraints
  Given the solver study path is "Antares_Simulator_Tests_NR/valid-v930/ST-additional-constraints-scenarized"
  When I run antares simulator
  Then the simulation takes less than 2 seconds
  And the simulation succeeds
  And in area "AREA", year 1 and hour 0, withdrawal for short-term storage "cluster-11" is 1
  And in area "AREA", year 1 and hour 0, injection for short-term storage "cluster-11" is 2
  And in area "AREA", year 1 and hour 0, level for short-term storage "cluster-11" is 50001
# initial lvl - withdrawal + injection + inflows
# 50000 - 1 + 2 + 0

  And in area "AREA", year 2 and hour 0, withdrawal for short-term storage "cluster-11" is 4
  And in area "AREA", year 2 and hour 0, injection for short-term storage "cluster-11" is 1
# initial lvl - withdrawal + injection + inflows
# 50000 - 4 + 1 + 1
  And in area "AREA", year 2 and hour 0, level for short-term storage "cluster-11" is 49998

  @fast @short
  Scenario: near price cap
    Given the solver study path is "Antares_Simulator_Tests_NR/valid-v930/tests-near-price-cap"
    When I run antares simulator
    Then the simulation takes less than 2 seconds
    And the simulation succeeds
    And in area "area", year 1 and hour 0, near price cap is 0 hours
    And in area "area", year 1 and hour 48, near price cap is 1 hours
    And in area "area 2", year 1 and hour 0, near price cap is 0 hours
    And in area "area 2", year 1 and hour 48, near price cap is 1 hours
    And in area "area 2", year 1 and hour 120, near price cap is 0 hours
    And in area "area 2", year 1 and hour 144, near price cap is 1 hours
    And in area "area 3", year 1 and hour 0, near price cap is 0 hours
    And in area "area 3", year 1 and hour 120, near price cap is 1 hours
    And in area "area 4", year 1 and hour 0, near price cap is 0 hours
    And in area "area 4", year 1 and hour 11, near price cap is 1 hours
    And in area "area 4", year 1 and hour 12, near price cap is 1 hours
    And in area "area 4", year 1 and hour 13, near price cap is 0 hours
    And in area "area 4", year 1 and hour 17, near price cap is 1 hours
