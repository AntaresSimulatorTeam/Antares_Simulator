Feature: ramping tests

@fast @short
# Le test vise a vérifier que la desactivation des rampes dans le general.data fonctionne bien
# On ajoute donc dans l'étude un cluster qui a des contraintes de rampes thermiques à la hausse et à la baisse, mais on désactive les rampes. Ces dernières ne doivent pas être prises en compte dans l'optimisation
  Scenario: ramping_1_disabled
    Given the solver study path is "Antares_Simulator_Tests_NR/ramping_tests/1_cluster_disabled_ramp"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "area", during year 1, hourly production of "thermal_1" for hour 7 is equal to 10 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 8 is equal to 30 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 15 is equal to 30 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 16 is equal to 80 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 23 is equal to 80 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 24 is equal to 100 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 39 is equal to 100 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 40 is equal to 60 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 47 is equal to 60 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 48 is equal to 20 MWh
	And in area "area", during year 1, hourly production of "thermal_1" for hour 55 is equal to 20 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 56 is equal to 100 MWh
    And the annual system cost is 5.0688e+07

@fast @short
# Le test vise a vérifier que les rampes a la hausse et à la baisse fonctionnent bien
# On a donc dans l'étude un cluster qui a des contraintes de rampes thermiques à la hausse et à la baisse, et on vérifie qu'elles ne sont pas dépassées
  Scenario: ramping_1_enabled
    Given the solver study path is "Antares_Simulator_Tests_NR/ramping_tests/1_cluster_ramp"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "area", during year 1, hourly production of "thermal_1" for hour 7 is equal to 10 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 8 is equal to 12 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 9 is equal to 14 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 15 is equal to 26 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 16 is equal to 78 MWh
	And in area "area", during year 1, hourly production of "thermal_1" for hour 17 is equal to 80 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 23 is equal to 80 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 24 is equal to 84 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 25 is equal to 88 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 31 is equal to 100 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 33 is equal to 100 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 34 is equal to 96 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 39 is equal to 66 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 47 is equal to 60 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 48 is equal to 20 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 55 is equal to 20 MWh
    And in area "area", during year 1, hourly production of "thermal_1" for hour 56 is equal to 72 MWh
    And the annual system cost is 5.09227e+07