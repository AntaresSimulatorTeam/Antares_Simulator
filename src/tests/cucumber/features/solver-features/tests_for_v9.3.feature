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