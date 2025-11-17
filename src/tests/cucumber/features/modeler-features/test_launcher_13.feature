Feature: 13 - Investment problems

  @fast @run
  Scenario: 13_1: XXX
    Given the modeler study path is "modeler/13_1"
    When I run antares modeler
    Then the simulation succeeds
    And the master problem contains the following variables
    | name | xmin | xmax | cost |
    | CAND.p_max | None | 1000 | 400 |
    And the subproblem contains the following variables
    | name|xmin|xmax|cost|
    | N.spillage|None|None|1|
    | N.unsupplied_energy|None|None| 501|
    And the structure file contains the following entries
    | problem | variable | index |
    | master  |CAND.p_max | 0 |
    | 1-1  |CAND.p_max | 4 |