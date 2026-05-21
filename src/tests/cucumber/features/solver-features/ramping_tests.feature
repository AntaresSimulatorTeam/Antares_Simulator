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
	
@fast @short
# Le test vise a vérifier que les couts des rampes a la hausse et à la baisse ont bien un impact
# On a donc dans l'étude des clusters d'une seule unité avec un ramping limité à 10 et différents couts à la hausse et à la baisse, si les couts de ramping sont supérieurs aux couts de unsupplied/spilled on on vérifie que le cluster ne rampe pas pour les satisfaire
# On note que les clusters qui le peuvent modulent à la hausse ou à la baisse pour compenser les extinctions/allumages des autres et coller au load
# The productions prices are set so that thermal_all_cheap will start first, followed by thermal_all_expensive, thermal_decrease_expensive and thermal_increase_expensive
  Scenario: 3_cluster_ramp_cost
    Given the solver study path is "Antares_Simulator_Tests_NR/ramping_tests/3_cluster_ramp_cost"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "area", during year 1, hourly production of "thermal_all_cheap" for hour 0 is equal to 0 MWh
	And in area "area", during year 1, hourly production of "thermal_all_cheap" for hour 1 is equal to 10 MWh
	And in area "area", during year 1, hourly production of "thermal_all_cheap" for hour 5 is equal to 50 MWh
	And in area "area", during year 1, hourly production of "thermal_all_cheap" for hour 10 is equal to 40 MWh
    And in area "area", during year 1, hourly production of "thermal_all_expensive" for hour 15 is equal to 0 MWh
	And in area "area", during year 1, hourly production of "thermal_all_expensive" for hour 16 is equal to 50 MWh
	And in area "area", during year 1, hourly production of "thermal_all_expensive" for hour 41 is equal to 50 MWh
	And in area "area", during year 1, hourly production of "thermal_all_expensive" for hour 42 is equal to 0 MWh
    And in area "area", during year 1, hourly production of "thermal_decrease_expensive" for hour 5 is equal to 0 MWh
    And in area "area", during year 1, hourly production of "thermal_decrease_expensive" for hour 6 is equal to 10 MWh
    And in area "area", during year 1, hourly production of "thermal_decrease_expensive" for hour 7 is equal to 20 MWh
	And in area "area", during year 1, hourly production of "thermal_decrease_expensive" for hour 20 is equal to 50 MWh
    And in area "area", during year 1, hourly production of "thermal_decrease_expensive" for hour 51 is equal to 50 MWh
	And in area "area", during year 1, hourly production of "thermal_decrease_expensive" for hour 52 is equal to 0 MWh
    And in area "area", during year 1, hourly production of "thermal_increase_expensive" for hour 10 is equal to 0 MWh
    And in area "area", during year 1, hourly production of "thermal_increase_expensive" for hour 11 is equal to 50 MWh
    And in area "area", during year 1, hourly production of "thermal_increase_expensive" for hour 37 is equal to 50 MWh
    And in area "area", during year 1, hourly production of "thermal_increase_expensive" for hour 38 is equal to 40 MWh
	And in area "area", during year 1, hourly production of "thermal_increase_expensive" for hour 44 is equal to 30 MWh
    And the annual system cost is 4.2564e+07