Feature: reserves tests

@fast @short
# Lot 1 : Intégration de la participation du thermique allumé à des réserves à la hausse et baisse
# Le test vise a vérifier que la desactivation des réserves dans le general.data fonctionne bien
# On ajoute donc dans l'étude des réserves et des clusters qui peuvent y participer, mais on désactive les réserves. Ces dernières ne doivent pas être prises en compte dans l'optimisation
  Scenario: lot_1_disabled
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_1_disabled"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 100 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And the annual system cost is 6.7872e+07

@fast @short
# Lot 1 : Intégration de la participation du thermique allumé à des réserves à la hausse
# Le test vise à vérifier la fonctionnalité de participation des clusters thermique allumés aux réserves à la hausse, en prenant en compte les surcoûts de participation.
# les surcoûts de participation doivent permettre de hierarchiser l'odre de participation des clusters aux réserves et doivent apparaître dans le coût total
# Les réserves sont activées, on a thermal_all_cheap (production et réserves cheap) qui participe a la réserve UP et thermal_expensive_res_part (production cheap mais réserves chères) qui produit,
# thermal_expensive_prod (production chère mais réserves cheap) qui participe a la réserve sans produire
  Scenario: lot_1_simple_up
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_1_simple_up"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 80 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_1", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_1", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_1", reserve participation power is always equal to 0 MWh
    And the annual system cost is 7.04928e+07

@fast @short
# Lot 1 : Intégration de la participation du thermique allumé à des réserves à la baisse
# Le test vise à vérifier la fonctionnalité de participation des clusters thermique allumés aux réserves à la baisse, en prenant en compte les surcoûts de participation.
# les surcoûts de participation doivent permettre de hierarchiser l'odre de participation des clusters aux réserves et doivent apparaître dans le coût total
# Les réserves sont activées, on a thermal_all_cheap (production et réserves cheap) qui participe a la réserve DOWN, thermal_expensive_res_part (production cheap mais réserves chères) qui produit mais ne participe pas,
# thermal_expensive_prod (production chère mais réserves cheap) qui ne produit rien et ne peut donc pas participer à la baisse
  Scenario: lot_1_simple_down
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_1_simple_down"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 100 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_1", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_1", reserve participation power is always equal to 0 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_1", reserve participation power is always equal to 0 MWh
    And the annual system cost is 7.0896e+07

@fast @short
# Lot 1 : Intégration de la participation du thermique allumé à des réserves à la hausse et baisse
# Le test vise à vérifier la fonctionnalité de participation des clusters thermique allumés aux réserves à la baisse et la hausse, en prenant en compte les surcoûts de participation.
# les surcoûts de participation doivent permettre de hierarchiser l'odre de participation des clusters aux réserves et doivent apparaître dans le coût total
  Scenario: lot_1_simple_up_and_down
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_1_simple_up_and_down"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 80 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_up", reserve participation power is always equal to 20 MWh
	  And in area "AREA", during year 1, for group "GAS" and reserve "Res_up", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_up", reserve participation power is always equal to 20 MWh
	  And in area "AREA", during year 1, for group "NUCLEAR" and reserve "Res_up", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_up", reserve participation power is always equal to 0 MWh
	  And in area "AREA", during year 1, for group "COAL" and reserve "Res_up", reserve participation power is always equal to 0 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_down", reserve participation power is always equal to 20 MWh
	  And in area "AREA", during year 1, for group "GAS" and reserve "Res_down", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_down", reserve participation power is always equal to 0 MWh
	  And in area "AREA", during year 1, for group "NUCLEAR" and reserve "Res_down", reserve participation power is always equal to 0 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_down", reserve participation power is always equal to 0 MWh
	  And in area "AREA", during year 1, for group "COAL" and reserve "Res_down", reserve participation power is always equal to 0 MWh

    And the annual system cost is 7.35168e+07

@fast @short
# Lot 2 : Intégration de la participation du thermique éteint et des stockage CT et LT
# Un cluster qui participe avec à une réserve à la hausse avec des unités off
  Scenario: lot_2_off_cluster_participation
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_2_off_cluster_participation"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", total reserve participation power is 0 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", participation of off units to the reserve is always equal to 40 MWh
    # OV. Cost = 20 *50+1*40+500*50+3000*10 euros (Prod cluster + Surcoûts réserves + défaillance EOD + défaillance réserves)
    And in area "AREA", overall cost on "1 JAN 06:00" of year 1 is of 56040 Euro 
    And the annual system cost is 9.408e+06

@fast @short
# Lot 2
# Un cluster qui participe avec à deux réserves à la hausse avec des unités off, on vérifié qu'il priviligie la plus rentable
  Scenario: lot_2_off_cluster_participation_multiple_res
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/lot_2_off_cluster_participation_multiple_res"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", total reserve participation power is 0 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_2", total reserve participation power is 0 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", participation of off units to the reserve is always equal to 30 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_2", participation of off units to the reserve is always equal to 20 MWh
    And in area "AREA", during year 1, for reserve "Res_1", reserve unsupplied power is always equal to 10 MWh
    And in area "AREA", during year 1, for reserve "Res_2", reserve unsupplied power is always equal to 0 MWh
    And in area "AREA", overall cost on "1 JAN 06:00" of year 1 is of 80050 Euro 
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 100 MW
    And the annual system cost is 1.344e+07

#
# Pour les tests du lot 3 : se référer au document "Schema_de_tests_des_réserves_3_nov_2024.pptx"
#

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce premier test, on ne considère pas les contraintes associées aux paramètres de contraintes de stocks
  Scenario: ST_1UP_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 2nd test, on ne considère que la contrainte de power activation
  Scenario: ST_1UP_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 3ème test, on ne considère que la contrainte de power activation ainsi que ref activation duration et energy activation ratio mais les deux derniers paramètres ne doivent pas impacter
  Scenario: ST_1UP_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 4ème test, on ne considère que la contrainte de power activation ainsi que ref activation duration et energy activation ratio avec les deux derniers paramètres qui doivent impacter
  Scenario: ST_1UP_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 5ème test, on ne considère pas la contrainte de power activation mais ref activation duration et energy activation ratio avec les deux derniers paramètres qui doivent impacter
  Scenario: ST_1UP_reserves_test5
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 6ème test, on considère la contrainte de power activation et ref activation duration mais pas energy activation ratio 

  Scenario: ST_1UP_reserves_test6
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 7ème test, on se concentre sur le energy activation ratio pour une réserve spécifique

  Scenario: ST_1UP_reserves_test7
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 8ème test, on se concentre sur le power activation ratio pour une réserve spécifique

  Scenario: ST_1UP_reserves_test8
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur le power activation ratio pour une réserve spécifique
  Scenario: ST_1UP_reserves_test9
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce premier test, on ne considère pas les contraintes associées aux paramètres de contraintes de stocks
  Scenario: ST_1DOWN_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 2nd test, on ne considère que la contrainte de power activation
  Scenario: ST_1DOWN_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 3ème test, on ne considère que la contrainte de power activation ainsi que ref activation duration et energy activation ratio mais les deux derniers paramètres ne doivent pas impacter
  Scenario: ST_1DOWN_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 4ème test, on ne considère que la contrainte de power activation ainsi que ref activation duration et energy activation ratio avec les deux derniers paramètres qui doivent impacter
  Scenario: ST_1DOWN_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 5ème test, on ne considère pas la contrainte de power activation mais ref activation duration et energy activation ratio avec les deux derniers paramètres qui doivent impacter
  Scenario: ST_1DOWN_reserves_test5
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 6ème test, on considère la contrainte de power activation et ref activation duration mais pas energy activation ratio 
  Scenario: ST_1DOWN_reserves_test6
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 7ème test, on se concentre sur le energy activation ratio pour une réserve spécifique
  Scenario: ST_1DOWN_reserves_test7
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 8ème test, on se concentre sur le power activation ratio pour une réserve spécifique
  Scenario: ST_1DOWN_reserves_test8
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_1DOWN_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio
# On considère 1 réserve down
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur le power activation ratio pour une réserve spécifique
  Scenario: ST_1DOWN_reserves_test9
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test1
  Scenario: ST_2UP_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	  And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh
	
@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test2
  Scenario: ST_2UP_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 50 MWh
	
@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test3
  Scenario: ST_2UP_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 50 MWh
	
@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test4
  Scenario: ST_2UP_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", the sum over two hours of reserve participation power is always equal to 100 MWh
	
@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test5
  Scenario: ST_2UP_reserves_test5
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 0 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test6
  Scenario: ST_2UP_reserves_test6
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 0 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# On considère 2 réserves up
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test7
  Scenario: ST_2UP_reserves_test7
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# On considère 2 réserves up
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test8
  Scenario: ST_2UP_reserves_test8
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Les tests "ST_2UP_reserves_testX" visent à tester les contraintes associées aux paramètres ref activation duration et energy activation ratio dans le cas de 2 besoins de réserves
# On considère 2 réserves up
# Ce test est intéressant car les contraintes précédentes somment la participation par type de réserves (up ou down) dans leur formulation
# Afin de contraindre le problème, le stock CT considéré aura des puissances Turb/Pomp, un stock initial et des certifications de participation aux réserves calibrées pour
# Dans ce 9ème test, on se concentre sur les mêmes critères que ST_1UP_reserves_test9
  Scenario: ST_2UP_reserves_test9
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh






@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Thermal cluster n'est pas diponible au départ : la batterie participe aux réserves, puis se vide pour limiter le loss of load, et se remplit quand le cluster se rallume
# Lot 3 non actif
  Scenario: ST_3UP_unavailable_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_1.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33400 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And the annual system cost is 3.02e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Thermal cluster n'est pas diponible au départ : la batterie participe aux réserves, puis se vide pour limiter le loss of load, et se remplit quand le cluster se rallume
# energy-activation-ratio-up = 1
# energy-activation-ratio = 1
# power-activation-ratio = 1
# reference-activation-duration = 2
  Scenario: ST_3UP_unavailable_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_2.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 32800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 3.08e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Thermal cluster n'est pas diponible au départ : la batterie participe aux réserves, puis se vide pour limiter le loss of load, et se remplit quand le cluster se rallume
# energy-activation-ratio-up = 0.5
# energy-activation-ratio = 0.5
# reference-activation-duration = 2
# power-activation-ratio = 0.5 
Scenario: ST_3UP_unavailable_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_3.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 32800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 3.08e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Thermal cluster n'est pas diponible au départ : la batterie participe aux réserves, puis se vide pour limiter le loss of load, et se remplit quand le cluster se rallume
# energy-activation-ratio-up = 0.5
# reference-activation-duration-up = 2
# energy-activation-ratio = 0.5
# power-activation-ratio = 1
# reference-activation-duration = 2
  Scenario: ST_3UP_unavailable_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_4.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 6.44e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Test des réserves dans l'hydro avec un réservoir vide sans définir de power activation ratio
  Scenario: LT_1_up_down_reserves_without_power_activation_ratio_empty
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/LT_1_reserves"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_1", total reserve participation power is 1008 MWh
	  And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_2", total reserve participation power is 1680 MWh
    And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 6 MWh
	  And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_2", on "1 JAN 06:00", reserve participation power is 10 MWh
    And the annual system cost is 2.1504e+06
	

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Test des réserves dans l'hydro avec un réservoir vide avec power activation ratio : les réserves à la hausse ne peuvent pas participer quand le réservoir est trop bas
  Scenario: LT_2_up_down_reserves_with_power_activation_ratio_empty
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/LT_2_reserves"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_1", total reserve participation power is inferior to 1000 MWh
	  And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_2", total reserve participation power is 1680 MWh
    And the annual system cost is 2.15533e+06


@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
# Test des réserves dans l'hydro avec un réservoir plein avec power activation ratio : les réserves à la baisse ne peuvent pas participer quand le réservoir est trop plein
  Scenario: LT_3_up_down_reserves_with_power_activation_ratio_full
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/LT_3_reserves"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_1", total reserve participation power is 1008 MWh
	  And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_2", total reserve participation power is inferior to 1680 MWh
    And the annual system cost is 2.15718e+06

@fast @short
# Lot 3_1 : intégration des contraintes de symétries
# Test des symmetries avec clusters thermiques
# l'objectif est de tester la symétrie pour un cluster sur deux réserves, pour lesquelles il a une certification de participation différente
  Scenario: Thermal_cluster_symmetry_test_1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/symmetry_test_1_thermal"
    When I run antares simulator 
    Then the simulation succeeds
    And the simulation takes less than 60 seconds
    And in area "FRANCE", during year 1, for cluster "therm" and reserve "Res_1_up", reserve participation power is always equal to 40 MWh
    And in area "FRANCE", during year 1, for cluster "therm" and reserve "Res_1_down", reserve participation power is always equal to 20 MWh
    And in area "FRANCE", during year 1, for reserve "Res_1_up", reserve unsupplied power is always equal to 10 MWh
    And in area "FRANCE", during year 1, for reserve "Res_1_down", reserve unsupplied power is always equal to 30 MWh
    And in area "FRANCE", during year 1, hourly production of "therm" is always equal to 60 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 40 MW

@fast @short
# Lot 3_1 : intégration des contraintes de symétries
# l'objectif de ce test est de tester la symétrie d'un stock CT dans un cas où la symétrie est sur une réserve à la hausse + baisse
# les contraintes physiques sur la participation à la hausse contraignent la participation à la baisse du fait des symétries
# Test des symmetries avec stocks ST
  Scenario: ST_symmetry_test_2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/symmetry_test_2_ST"
    When I run antares simulator 
    Then the simulation succeeds
    And the simulation takes less than 60 seconds
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "Res_1_up", reserve participation power is always equal to 30 MWh
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "Res_2_down", reserve participation power is always equal to 7.5 MWh
    And in area "FRANCE", during year 1, for reserve "Res_1_up", reserve unsupplied power is always equal to 0 MWh
    And in area "FRANCE", during year 1, for reserve "Res_2_down", reserve unsupplied power is always equal to 52.5 MWh
    And in area "FRANCE", year 1 and hour 1, withdrawal for short-term storage "st1" is 10
    And in area "FRANCE", year 1 and hour 1, injection for short-term storage "st1" is 10
 
@fast @short
# Lot 3_1 : intégration des contraintes de symétries
# l'objectif de ce test est de tester la symétrie d'un stock LT dans un cas où la symétrie est sur une réserve à la hausse + baisse
# les contraintes physiques sur la participation à la hausse contraignent la participation à la baisse du fait des symétries
# Test des symmetries avec stocks LT
  Scenario: LT_symmetry_test_2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/symmetry_test_2_LT"
    When I run antares simulator 
    Then the simulation succeeds
    And the simulation takes less than 60 seconds
    And in area "FRANCE", during year 1, for cluster "Hydro" and reserve "Res_1_up", reserve participation power is always equal to 30 MWh
    And in area "FRANCE", during year 1, for cluster "Hydro" and reserve "Res_2_down", reserve participation power is always equal to 7.5 MWh
    And in area "FRANCE", during year 1, for reserve "Res_1_up", reserve unsupplied power is always equal to 0 MWh
    And in area "FRANCE", during year 1, for reserve "Res_2_down", reserve unsupplied power is always equal to 52.5 MWh
    And in area "FRANCE", during year 1, total hydro production is 87360 MWh
    And in area "FRANCE", during year 1, total hydro pumping is 87360 MWh
 
@fast @short
# Lot 3_1 : intégration des contraintes de symétries
# l'objectif de ce test est de tester la symétrie d'un stock CT dans un cas où la symétrie est sur deux réserve à la hausse
# les contraintes physiques sur la participation à la hausse contraignent la participation à la baisse du fait des symétries
# Test des symmetries avec stocks ST
  Scenario: ST_symmetry_test_3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/symmetry_test_3_ST"
    When I run antares simulator 
    Then the simulation succeeds
    And the simulation takes less than 60 seconds
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "Res_1_up", reserve participation power is always equal to 10 MWh
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "Res_2_up", reserve participation power is always equal to 20 MWh
    And in area "FRANCE", during year 1, for reserve "Res_1_up", reserve unsupplied power is always equal to 0 MWh
    And in area "FRANCE", during year 1, for reserve "Res_2_up", reserve unsupplied power is always equal to 10 MWh
    And in area "FRANCE", year 1 and hour 1, withdrawal for short-term storage "st1" is 0
    And in area "FRANCE", year 1 and hour 1, injection for short-term storage "st1" is 0
 
@fast @short
# Lot 3_1 : intégration des contraintes de symétries
# l'objectif de ce test est de tester la symétrie d'un stock LT dans un cas où la symétrie est sur deux réserve à la hausse
# les contraintes physiques sur la participation à la hausse contraignent la participation à la baisse du fait des symétries
# Test des symmetries avec stocks LT
  Scenario: LT_symmetry_test_3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves-tests/symmetry_test_3_LT"
    When I run antares simulator 
    Then the simulation succeeds
    And the simulation takes less than 60 seconds
    And in area "FRANCE", during year 1, for cluster "Hydro" and reserve "Res_1_up", reserve participation power is always equal to 10 MWh
    And in area "FRANCE", during year 1, for cluster "Hydro" and reserve "Res_2_up", reserve participation power is always equal to 20 MWh
    And in area "FRANCE", during year 1, for reserve "Res_1_up", reserve unsupplied power is always equal to 0 MWh
    And in area "FRANCE", during year 1, for reserve "Res_2_up", reserve unsupplied power is always equal to 10 MWh
    And in area "FRANCE", during year 1, total hydro production is 0 MWh
    And in area "FRANCE", during year 1, total hydro pumping is 0 MWh