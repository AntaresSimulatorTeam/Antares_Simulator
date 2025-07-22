Feature: tests for v9.2
  
  @fast @short
  Scenario: min gen for thermal clusters
  Given the solver study path is "Antares_Simulator_Tests_NR/valid-v920/min-gen-for-thermal"
  When I run antares simulator
  Then the simulation succeeds
  And the simulation takes less than 5 seconds
  # area : AREA
  And in area "AREA", week 1, year 1, daily mingens for cluster "some-cluster" are 10, 30, 50, 50, 40, 30, 20
  # area : AREA 2
  And in area "AREA 2", week 1, year 1, daily mingens for cluster "cluster-1" are 10, 30, 50, 50, 40, 30, 20
  And in area "AREA 2", week 1, year 1, daily mingens for cluster "cluster-2" are 10, 30, 50, 50, 40, 30, 20
  And in area "AREA 2", week 1, year 1, daily mingens for cluster "cluster-3" are 1, 3, 5, 5, 4, 3, 2
  # area : AREA 3
  And in area "AREA 3", year 1, no mingens for cluster "cluster-must-run"
  