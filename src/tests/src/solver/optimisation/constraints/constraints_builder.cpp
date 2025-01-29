/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define BOOST_TEST_MODULE "constraints_builder"
#define WIN32_LEAN_AND_MEAN

#include <numeric>

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

/*
 * this code is designed to:
 * Validate the addition of withdrawalSum, injectionSum, and netting constraints for various
 * countries (CountryA, CountryB, CountryC). Ensure that the cumulative constraints are correctly
 * applied and stored.
 *
 */

struct BB
{
    int nombreDePasDeTempsPourUneOptimisation = 50;

    std::vector<double> Pi = std::vector(2 * nombreDePasDeTempsPourUneOptimisation, 0.0);
    // Placeholder for coefficients
    std::vector<int> Colonne = std::vector(2 * nombreDePasDeTempsPourUneOptimisation, 0);
    // Placeholder for column indices
    int nombreDeContraintes = 0;
    int nombreDeTermesDansLaMatriceDeContrainte = 0;
    std::vector<int> IndicesDebutDeLigne = std::vector(4, 0);
    SparseVector<double> CoefficientsDeLaMatriceDesContraintes;
    //= std::vector(100, 0.0); // Plain vector
    SparseVector<int> IndicesColonnes;
    //= std::vector<int> (100, 0); // Plain vector for column indices
    std::vector<int> NombreDeTermesDesLignes = std::vector<int>(4, 0);
    std::string Sens = std::string(4, '='); // Placeholder for constraint senses
    int IncrementDAllocationMatriceDesContraintes = 10;
    std::vector<CORRESPONDANCES_DES_VARIABLES> CorrespondanceVarNativesVarOptim;
    ;

    void set_correspondances_des_variables()
    {
        CorrespondanceVarNativesVarOptim.resize(nombreDePasDeTempsPourUneOptimisation);
        // for every timestep the same indices
        // .InjectionVariable = {0, 1} :
        // --> storage 1 --> injection index = 0
        // --> storage 2 --> injection index = 1
        // --> storage 3 --> injection index = 1

        for (auto i = 0; i < nombreDePasDeTempsPourUneOptimisation; i++)
        {
            CorrespondanceVarNativesVarOptim[i].SIM_ShortTermStorage = {
              .InjectionVariable = {0, 1, 4},
              .WithdrawalVariable = {2, 3, 5}};
        }
    }

    //.S {{.SIM_ShortTermStorage = {.InjectionVariable = {0,1}, .WithdrawalVariable = {2,3} } }};
    const int32_t NombreDePasDeTempsPourUneOptimisation = nombreDePasDeTempsPourUneOptimisation;
    // Example value
    std::vector<int> NumeroDeVariableStockFinal = std::vector<int>(10, -1);
    std::vector<std::vector<int>> NumeroDeVariableDeTrancheDeStock = std::vector<std::vector<int>>(
      10,
      std::vector<int>(5, -1));
    std::vector<std::string> NomDesContraintes = std::vector<std::string>(100, "");
    const bool NamedProblems = true;
    const std::vector<const char*> NomsDesPays = {"CountryA", "CountryB", "CountryC"};
    const uint32_t weekInTheYear = 1;        // Example week
    const uint32_t NombreDePasDeTemps = 168; // Example number of time steps in a week

    // Mock data storage

    std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>
      addc1_withdrawal_constraints = {{.hours = {1, 2}, .globalIndex = 0, .localIndex = 0},
                                      {.hours = {3, 4}, .globalIndex = 1, .localIndex = 1}};

    std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>
      addc2_injection_constraints = {{.hours = {5, 6}, .globalIndex = 2, .localIndex = 0},
                                     {.hours = {7, 8}, .globalIndex = 3, .localIndex = 1}};
    std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>
      addc3_netting_constraints = {{.hours = {9, 10}, .globalIndex = 4, .localIndex = 0},
                                   {.hours = {11, 12}, .globalIndex = 5, .localIndex = 1}};

    std::vector<double> fill_rhs()
    {
        std::vector<double> ret(HOURS_PER_YEAR);
        std::iota(ret.begin(), ret.end(), 0);
        return ret;
    }

    Antares::Data::ShortTermStorage::AdditionalConstraints addc1_withdrawal = {
      .name = "addc1_withdrawal",
      .cluster_id = "cluster_1",
      .variable = "withdrawal",
      .operatorType = "less",
      .rhs = fill_rhs(),
      .constraints = addc1_withdrawal_constraints};
    Antares::Data::ShortTermStorage::AdditionalConstraints addc2_injection = {
      .name = "addc2_injection",
      .cluster_id = "cluster_2",
      .variable = "injection",
      .operatorType = "greater",
      .rhs = fill_rhs(),
      .constraints = addc2_injection_constraints};
    Antares::Data::ShortTermStorage::AdditionalConstraints addc3_netting = {
      .name = "addc3_netting",
      .cluster_id = "cluster_3",
      .variable = "netting",
      .operatorType = "equal",
      .rhs = fill_rhs(),
      .constraints = addc3_netting_constraints};

    ::ShortTermStorage::PROPERTIES storage1 = {.additionalConstraints = {addc1_withdrawal},
                                               .clusterGlobalIndex = 0,
                                               .name = "cluster_1"};
    ::ShortTermStorage::PROPERTIES storage2 = {.additionalConstraints = {addc2_injection},
                                               .clusterGlobalIndex = 1,
                                               .name = "cluster_2"};
    ::ShortTermStorage::PROPERTIES storage3 = {.injectionEfficiency = 45,
                                               .withdrawalEfficiency = 2025,
                                               .additionalConstraints = {addc3_netting},
                                               .clusterGlobalIndex = 2,
                                               .name = "cluster_3"};

    std::vector<CORRESPONDANCES_DES_CONTRAINTES> CorrespondanceCntNativesCntOptim;
    std::vector<::ShortTermStorage::AREA_INPUT> shortTermStorage = InitializeShortTermStorageData();
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES CorrespondanceCntNativesCntOptimHebdomadaires{
      {},
      std::vector<int>(20, 0)};

    std::vector<ShortTermStorage::AREA_INPUT> InitializeShortTermStorageData()
    {
        return {{storage1}, {storage2}, {storage3}};
    }

    ShortTermStorageCumulativeConstraintData shorttermstoragecumulativeconstraintdata
      = InitializeShortTermStorageCumulativeConstraintData();

    ShortTermStorageCumulativeConstraintData InitializeShortTermStorageCumulativeConstraintData()
    {
        return {CorrespondanceCntNativesCntOptim,
                shortTermStorage,
                CorrespondanceCntNativesCntOptimHebdomadaires};
    }

    ConstraintBuilderData constraint_builder_data = InitializeConstraintBuilderData();

    ConstraintBuilderData InitializeConstraintBuilder()
    {
        return ConstraintBuilderData(constraint_builder_data);
    }

    ConstraintBuilderData InitializeConstraintBuilderData()

    {
        set_correspondances_des_variables();

        // Create the mock ConstraintBuilderData object
        return {Pi,
                Colonne,
                nombreDeContraintes,
                nombreDeTermesDansLaMatriceDeContrainte,
                IndicesDebutDeLigne,
                CoefficientsDeLaMatriceDesContraintes,
                IndicesColonnes,
                NombreDeTermesDesLignes,
                Sens,
                IncrementDAllocationMatriceDesContraintes,
                CorrespondanceVarNativesVarOptim,
                NombreDePasDeTempsPourUneOptimisation,
                NumeroDeVariableStockFinal,
                NumeroDeVariableDeTrancheDeStock,
                NomDesContraintes,
                NamedProblems,
                NomsDesPays,
                weekInTheYear,
                NombreDePasDeTemps};
    }
};

BOOST_FIXTURE_TEST_CASE(AddWithdrawalConstraint, BB)
{
    ConstraintBuilder builder(constraint_builder_data);

    ShortTermStorageCumulation cumulation(builder, shorttermstoragecumulativeconstraintdata);

    // Call the add method for "CountryA" (index 0)
    cumulation.add(0);
    // Assert that the number of constraints has increased by the expected amount
    // Assuming 2 additional constraints (from addc1_withdrawal) should be added
    BOOST_CHECK_EQUAL(builder.data.nombreDeContraintes, 2);

    // Verify that the constraint names are correctly generated and stored
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[0],
                      "WithdrawalSum::area<CountryA>::ShortTermStorage<cluster_1>::Constraint<"
                      "addc1_withdrawal_0>"); // Assuming this is the generated name
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[1],
                      "WithdrawalSum::area<CountryA>::ShortTermStorage<cluster_1>::Constraint<"
                      "addc1_withdrawal_1>"); // Check the second constraint
    //
    // // Verify that the correct number of terms have been added to the matrix
    BOOST_CHECK_EQUAL(builder.data.nombreDeTermesDansLaMatriceDeContrainte, 4);

    // Verify that the correct indices for constraints have been set
    BOOST_CHECK_EQUAL(builder.data.IndicesDebutDeLigne[0], 0);
    // Assuming this is the starting index
    BOOST_CHECK_EQUAL(builder.data.IndicesDebutDeLigne[1], 2); // Check next line index

    // Verify that the correct variables and values were updated in the matrix
    BOOST_CHECK_EQUAL(builder.data.Pi[0], 1.0);
    // Verify the first term's coefficient (adjust based on actual expected values)
    BOOST_CHECK_EQUAL(builder.data.Pi[1], 1.0);
    // Verify the first term's coefficient (adjust based on actual expected values)
    BOOST_CHECK_EQUAL(builder.data.Colonne[0], 2); // Verify the first term's column index
    BOOST_CHECK_EQUAL(builder.data.Colonne[1], 2); // Verify the first term's column index

    // Check if the sense of constraints was updated correctly
    BOOST_CHECK_EQUAL(builder.data.Sens[0], '<');
    BOOST_CHECK_EQUAL(builder.data.Sens[1], '<');

    // 4. Validate correspondence mapping
    BOOST_CHECK_EQUAL(
      shorttermstoragecumulativeconstraintdata.CorrespondanceCntNativesCntOptimHebdomadaires
        .ShortTermStorageCumulation[addc1_withdrawal_constraints[0].globalIndex],
      0);
    BOOST_CHECK_EQUAL(
      shorttermstoragecumulativeconstraintdata.CorrespondanceCntNativesCntOptimHebdomadaires
        .ShortTermStorageCumulation[addc1_withdrawal_constraints[1].globalIndex],
      1);
}

BOOST_FIXTURE_TEST_CASE(AddInjectionConstraint, BB)
{
    ConstraintBuilder builder(constraint_builder_data);

    ShortTermStorageCumulation cumulation(builder, shorttermstoragecumulativeconstraintdata);

    // Call the add method for "CountryB" (index 1)
    cumulation.add(1);
    // Assert that the number of constraints has increased by the expected amount
    // Assuming 2 additional constraints (from addc1_withdrawal) should be added
    BOOST_CHECK_EQUAL(builder.data.nombreDeContraintes, 2);

    // Verify that the constraint names are correctly generated and stored
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[0],
                      "InjectionSum::area<CountryB>::ShortTermStorage<cluster_2>::Constraint<addc2_"
                      "injection_0>"); // Assuming this is the generated name
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[1],
                      "InjectionSum::area<CountryB>::ShortTermStorage<cluster_2>::Constraint<addc2_"
                      "injection_1>"); // Check the second constraint
    //
    // // Verify that the correct number of terms have been added to the matrix
    BOOST_CHECK_EQUAL(builder.data.nombreDeTermesDansLaMatriceDeContrainte, 4);

    // Verify that the correct indices for constraints have been set
    BOOST_CHECK_EQUAL(builder.data.IndicesDebutDeLigne[0], 0);
    // Assuming this is the starting index
    BOOST_CHECK_EQUAL(builder.data.IndicesDebutDeLigne[1], 2); // Check next line index

    // Verify that the correct variables and values were updated in the matrix
    BOOST_CHECK_EQUAL(builder.data.Pi[0], 1.0);
    // Verify the first term's coefficient (adjust based on actual expected values)
    BOOST_CHECK_EQUAL(builder.data.Pi[1], 1.0);
    // Verify the first term's coefficient (adjust based on actual expected values)
    BOOST_CHECK_EQUAL(builder.data.Colonne[0], 1); // Verify the first term's column index
    BOOST_CHECK_EQUAL(builder.data.Colonne[1], 1); // Verify the first term's column index

    // Check if the sense of constraints was updated correctly
    BOOST_CHECK_EQUAL(builder.data.Sens[0], '>');
    BOOST_CHECK_EQUAL(builder.data.Sens[1], '>');

    // 4. Validate correspondence mapping
    BOOST_CHECK_EQUAL(
      shorttermstoragecumulativeconstraintdata.CorrespondanceCntNativesCntOptimHebdomadaires
        .ShortTermStorageCumulation[addc2_injection_constraints[0].globalIndex],
      0);
    BOOST_CHECK_EQUAL(
      shorttermstoragecumulativeconstraintdata.CorrespondanceCntNativesCntOptimHebdomadaires
        .ShortTermStorageCumulation[addc2_injection_constraints[1].globalIndex],
      1);
}

BOOST_FIXTURE_TEST_CASE(AddNettingConstraint, BB)
{
    ConstraintBuilder builder(constraint_builder_data);

    ShortTermStorageCumulation cumulation(builder, shorttermstoragecumulativeconstraintdata);

    // Call the add method for "CountryC" (index 2)
    cumulation.add(2);
    // Assert that the number of constraints has increased by the expected amount
    // Assuming 2 additional constraints (from addc1_withdrawal) should be added
    BOOST_CHECK_EQUAL(builder.data.nombreDeContraintes, 2);

    // Verify that the constraint names are correctly generated and stored
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[0],
                      "NettingSum::area<CountryC>::ShortTermStorage<cluster_3>::Constraint<addc3_"
                      "netting_0>"); // Assuming this is the generated name
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[1],
                      "NettingSum::area<CountryC>::ShortTermStorage<cluster_3>::Constraint<addc3_"
                      "netting_1>"); // Check the second constraint
    //
    // // Verify that the correct number of terms have been added to the matrix
    BOOST_CHECK_EQUAL(builder.data.nombreDeTermesDansLaMatriceDeContrainte, 8);

    // Verify that the correct indices for constraints have been set
    BOOST_CHECK_EQUAL(builder.data.IndicesDebutDeLigne[0], 0);
    // Assuming this is the starting index
    BOOST_CHECK_EQUAL(builder.data.IndicesDebutDeLigne[1], 4); // Check next line index

    // Verify that the correct variables and values were updated in the matrix
    BOOST_CHECK_EQUAL(builder.data.Pi[0], storage3.injectionEfficiency);
    // Verify the first term's coefficient (adjust based on actual expected values)
    BOOST_CHECK_EQUAL(builder.data.Pi[1], -storage3.withdrawalEfficiency);
    // Verify the first term's coefficient (adjust based on actual expected values)
    BOOST_CHECK_EQUAL(builder.data.Colonne[0], 4); // Verify the first term's column index
    BOOST_CHECK_EQUAL(builder.data.Colonne[1], 5); // Verify the first term's column index

    // Check if the sense of constraints was updated correctly
    BOOST_CHECK_EQUAL(builder.data.Sens[0], '=');
    BOOST_CHECK_EQUAL(builder.data.Sens[1], '=');

    // 4. Validate correspondence mapping
    BOOST_CHECK_EQUAL(
      shorttermstoragecumulativeconstraintdata.CorrespondanceCntNativesCntOptimHebdomadaires
        .ShortTermStorageCumulation[addc3_netting_constraints[0].globalIndex],
      0);
    BOOST_CHECK_EQUAL(
      shorttermstoragecumulativeconstraintdata.CorrespondanceCntNativesCntOptimHebdomadaires
        .ShortTermStorageCumulation[addc3_netting_constraints[1].globalIndex],
      1);
}

BOOST_FIXTURE_TEST_CASE(MultipleAreasTest, BB)
{
    ConstraintBuilder builder(constraint_builder_data);
    ShortTermStorageCumulation cumulation(builder, shorttermstoragecumulativeconstraintdata);

    // Add constraints for multiple areas
    cumulation.add(0); // CountryA
    cumulation.add(1); // CountryB

    // Check if the number of constraints increased correctly
    BOOST_CHECK_EQUAL(builder.data.nombreDeContraintes, 4);

    // Verify the names of the constraints for both countries
    BOOST_CHECK_EQUAL(
      builder.data.NomDesContraintes[0],
      "WithdrawalSum::area<CountryA>::ShortTermStorage<cluster_1>::Constraint<addc1_withdrawal_0>");
    BOOST_CHECK_EQUAL(
      builder.data.NomDesContraintes[1],
      "WithdrawalSum::area<CountryA>::ShortTermStorage<cluster_1>::Constraint<addc1_withdrawal_1>");
    BOOST_CHECK_EQUAL(
      builder.data.NomDesContraintes[2],
      "InjectionSum::area<CountryB>::ShortTermStorage<cluster_2>::Constraint<addc2_injection_0>");
    BOOST_CHECK_EQUAL(
      builder.data.NomDesContraintes[3],
      "InjectionSum::area<CountryB>::ShortTermStorage<cluster_2>::Constraint<addc2_injection_1>");

    // Check if the sense of constraints was updated correctly for all areas
    BOOST_CHECK_EQUAL(builder.data.Sens[0], '<');
    BOOST_CHECK_EQUAL(builder.data.Sens[1], '<');
    BOOST_CHECK_EQUAL(builder.data.Sens[2], '>');
    BOOST_CHECK_EQUAL(builder.data.Sens[3], '>');
}

void SetupProblemHebdo(PROBLEME_HEBDO& problemeHebdo,
                       int numberOfAreas,
                       int numberOfConstraints,

                       int numberOfTimeSteps)
{
    problemeHebdo.ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();

    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre = *problemeHebdo.ProblemeAResoudre;

    // Setup second member vector
    problemeAResoudre.SecondMembre.resize(numberOfConstraints, 0.0);
    problemeAResoudre.AdresseOuPlacerLaValeurDesCoutsMarginaux.resize(numberOfConstraints, nullptr);
    problemeAResoudre.NombreDeContraintes = numberOfConstraints;

    // Setup storage constraints mapping
    problemeHebdo.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation = {1,
                                                                                              2,
                                                                                              3};

    problemeHebdo.CorrespondanceCntNativesCntOptimJournalieres.resize(1);

    // Initialize CorrespondanceCntNativesCntOptim for each time step
    problemeHebdo.CorrespondanceCntNativesCntOptim.resize(numberOfTimeSteps);
    for (auto& corresp: problemeHebdo.CorrespondanceCntNativesCntOptim)
    {
        corresp.NumeroDeContrainteDesBilansPays.resize(numberOfAreas, 0);
        corresp.NumeroDeContraintePourEviterLesChargesFictives.resize(numberOfAreas, 0);
        corresp.NumeroDeContrainteDesNiveauxPays.resize(numberOfAreas, 0);
        corresp.NumeroDeContrainteDeDissociationDeFlux.resize(1, 0);
        corresp.NumeroDeContrainteDesContraintesCouplantes.resize(1, 0);
        corresp.ShortTermStorageLevelConstraint.resize(numberOfAreas, 0);
    }
    problemeHebdo.NumeroDeContrainteEnergieHydraulique.resize(numberOfAreas, -1);
    problemeHebdo.CaracteristiquesHydrauliques.resize(numberOfAreas,
                                                      {.TurbinageEntreBornes = false,
                                                       .SansHeuristique = false,
                                                       .SuiviNiveauHoraire = false});

    // Initialize other required vectors
    problemeHebdo.ConsommationsAbattues.resize(numberOfTimeSteps);
    for (auto& cons: problemeHebdo.ConsommationsAbattues)
    {
        cons.ConsommationAbattueDuPays.resize(numberOfAreas, 0.0);
    }

    problemeHebdo.AllMustRunGeneration.resize(numberOfTimeSteps);
    for (auto& gen: problemeHebdo.AllMustRunGeneration)
    {
        gen.AllMustRunGenerationOfArea.resize(numberOfAreas, 0.0);
    }

    problemeHebdo.NumeroDeJourDuPasDeTemps.resize(numberOfTimeSteps, 0);
    problemeHebdo.DefaillanceNegativeUtiliserConsoAbattue.resize(numberOfAreas, false);
    problemeHebdo.DefaillanceNegativeUtiliserPMinThermique.resize(numberOfAreas, true);

    // Initialize ResultatsHoraires
    problemeHebdo.ResultatsHoraires.resize(numberOfAreas);
    for (auto& resultats: problemeHebdo.ResultatsHoraires)
    {
        resultats.CoutsMarginauxHoraires.resize(numberOfTimeSteps, 0.0);
    }

    // Setup short term storage data
    problemeHebdo.ShortTermStorage.resize(numberOfAreas);
    problemeHebdo.NombreDePays = numberOfAreas;
    problemeHebdo.OptimisationAvecCoutsDeDemarrage = false;
    problemeHebdo.NombreDePasDeTempsDUneJournee = 24;
    problemeHebdo.NombreDePasDeTempsPourUneOptimisation = 24;
    problemeHebdo.NombreDeContraintesCouplantes = 0;
    problemeHebdo.NombreDInterconnexions = 0;
    problemeHebdo.OptimisationAuPasHebdomadaire = false;
    problemeHebdo.YaDeLaReserveJmoins1 = false;
    problemeHebdo.weekInTheYear = 0;
}

struct ExpectedResult
{
    int constraint_index;
    double rhs;
};

ExpectedResult SetupSingleStorageOneArea(PROBLEME_HEBDO& problemeHebdo)
{
    // Setup a single storage in one area
    ShortTermStorage::AREA_INPUT& area0 = problemeHebdo.ShortTermStorage[0];
    area0.resize(1);

    Antares::Data::ShortTermStorage::AdditionalConstraints additionalConstraint;
    additionalConstraint.rhs = {12.0, 18.0, 24.0}; // RHS values for first hours

    Antares::Data::ShortTermStorage::SingleAdditionalConstraint constraint;
    constraint.globalIndex = 0;
    constraint.hours = {1, 2, 3}; // First three hours
    additionalConstraint.constraints.push_back(constraint);

    auto& storage_area0 = area0[0];
    storage_area0.series = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    storage_area0.series->inflows.resize(HOURS_PER_YEAR, 5.0); // Default inflow
    storage_area0.additionalConstraints.push_back(additionalConstraint);

    // Expected result: Sum of 12.0 + 18.0 + 24.0 = 54.0
    return {.constraint_index = problemeHebdo.CorrespondanceCntNativesCntOptimHebdomadaires
                                  .ShortTermStorageCumulation[constraint.globalIndex],
            .rhs = 54.0};
}

BOOST_AUTO_TEST_CASE(TestSingleStorageOneArea)
{
    PROBLEME_HEBDO problemeHebdo;
    const int numberOfAreas = 1;
    const int numberOfConstraints = 5;
    const int numberOfTimeSteps = 24;

    SetupProblemHebdo(problemeHebdo, numberOfAreas, numberOfConstraints, numberOfTimeSteps);
    const auto [constraint_index, expected_rhs] = SetupSingleStorageOneArea(problemeHebdo);
    // Call function
    OPT_InitialiserLeSecondMembreDuProblemeLineaire(&problemeHebdo, 0, 24, 0, 1);

    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre = *problemeHebdo.ProblemeAResoudre;

    BOOST_CHECK_CLOSE(problemeAResoudre.SecondMembre[constraint_index], expected_rhs, 0.001);
}

std::vector<ExpectedResult> SetupMultipleStoragesDifferentAreas(PROBLEME_HEBDO& problemeHebdo)
{
    // Area 0 setup
    ShortTermStorage::AREA_INPUT& area0 = problemeHebdo.ShortTermStorage[0];
    area0.resize(1);
    Antares::Data::ShortTermStorage::AdditionalConstraints additionalConstraint0;
    additionalConstraint0.rhs = {10.0, 15.0, 20.0, 25.0}; // RHS values for the first few hours

    Antares::Data::ShortTermStorage::SingleAdditionalConstraint constraint0;
    constraint0.globalIndex = 1;
    constraint0.hours = {1, 2, 3}; // First three hours
    additionalConstraint0.constraints.push_back(constraint0);
    auto& storage0_area0 = area0[0];
    storage0_area0.series = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    // Initialize series data for the full year
    storage0_area0.series->inflows.resize(HOURS_PER_YEAR, 10.0); // Default inflow value
    storage0_area0.series->maxInjectionModulation.resize(HOURS_PER_YEAR, 100.0);
    storage0_area0.series->maxWithdrawalModulation.resize(HOURS_PER_YEAR, 100.0);
    storage0_area0.additionalConstraints.push_back(additionalConstraint0);

    // Area 1 setup
    ShortTermStorage::AREA_INPUT& area1 = problemeHebdo.ShortTermStorage[1];
    area1.resize(1);
    Data::ShortTermStorage::AdditionalConstraints additionalConstraint1;
    additionalConstraint1.rhs = {5.0, 8.0, 12.0, 15.0}; // RHS values for the first few hours

    Data::ShortTermStorage::SingleAdditionalConstraint constraint1;
    constraint1.globalIndex = 2;
    constraint1.hours = {1, 2}; // First two hours
    additionalConstraint1.constraints.push_back(constraint1);

    auto& storage1_area1 = area1[0];
    storage1_area1.series = std::make_shared<Data::ShortTermStorage::Series>();

    // Initialize series data for the full year
    storage1_area1.series->inflows.resize(HOURS_PER_YEAR, 0.0); // Default inflow value
    storage1_area1.series->maxInjectionModulation.resize(HOURS_PER_YEAR, 0.0);
    storage1_area1.series->maxWithdrawalModulation.resize(HOURS_PER_YEAR, 0.0);
    storage1_area1.additionalConstraints.push_back(additionalConstraint1);
    // For area 0, sum should be 10.0 + 15.0 + 20.0 = 45.0
    // For area 1, sum should be 5.0 + 8.0 = 13.0
    return {{.constraint_index = problemeHebdo.CorrespondanceCntNativesCntOptimHebdomadaires
                                   .ShortTermStorageCumulation[constraint0.globalIndex] /*= 2*/,
             .rhs = 45.0},
            {.constraint_index = problemeHebdo.CorrespondanceCntNativesCntOptimHebdomadaires
                                   .ShortTermStorageCumulation[constraint1.globalIndex] /* =3 */,
             .rhs = 13.0}};
}

BOOST_AUTO_TEST_CASE(TestMultipleStoragesDifferentAreas)
{
    // Setup test data
    PROBLEME_HEBDO problemeHebdo;
    // Initialize problem size
    const int numberOfAreas = 2;
    const int numberOfConstraints = 10;

    const int numberOfTimeSteps = 24;

    SetupProblemHebdo(problemeHebdo, numberOfAreas, numberOfConstraints, numberOfTimeSteps);
    const auto expected_results = SetupMultipleStoragesDifferentAreas(problemeHebdo);

    // Call the function
    OPT_InitialiserLeSecondMembreDuProblemeLineaire(&problemeHebdo,
                                                    0,  // PremierPdtDeLIntervalle
                                                    24, // DernierPdtDeLIntervalle
                                                    0,  // NumeroDeLIntervalle
                                                    1   // optimizationNumber
    );

    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre = *problemeHebdo.ProblemeAResoudre;
    std::vector<int> not_affected_constraints_indices;
    std::iota(not_affected_constraints_indices.begin(), not_affected_constraints_indices.end(), 0);
    // Verify the results
    for (const auto& [constraint_index, expected_rhs]: expected_results)
    {
        BOOST_CHECK_CLOSE(problemeAResoudre.SecondMembre[constraint_index], expected_rhs, 0.001);

        // remove this constraint from the list
        erase_if(not_affected_constraints_indices,
                 [&constraint_index](const int index) { return index == constraint_index; });
    }

    // Check that other constraints weren't affected
    for (auto index: not_affected_constraints_indices)
    {
        BOOST_CHECK_SMALL(problemeAResoudre.SecondMembre[index], 0.001);
    }
}

std::vector<ExpectedResult> SetupMultipleStoragesSameArea(PROBLEME_HEBDO& problemeHebdo)
{
    // Setup two storage units in the same area
    ShortTermStorage::AREA_INPUT& area0 = problemeHebdo.ShortTermStorage[0];
    area0.resize(2);

    // First storage
    Antares::Data::ShortTermStorage::AdditionalConstraints additionalConstraint1;
    additionalConstraint1.rhs = {10.0, 15.0}; // First two hours
    Antares::Data::ShortTermStorage::SingleAdditionalConstraint constraint1;
    constraint1.globalIndex = 0;
    constraint1.hours = {1, 2};
    additionalConstraint1.constraints.push_back(constraint1);

    auto& storage1 = area0[0];
    storage1.series = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    storage1.series->inflows.resize(HOURS_PER_YEAR, 0.0); // Default inflow
    storage1.additionalConstraints.push_back(additionalConstraint1);

    // Second storage
    Antares::Data::ShortTermStorage::AdditionalConstraints additionalConstraint2;
    additionalConstraint2.rhs = {5.0, 7.0}; // First two hours
    Antares::Data::ShortTermStorage::SingleAdditionalConstraint constraint2;
    constraint2.globalIndex = 1;
    constraint2.hours = {1, 2};
    additionalConstraint2.constraints.push_back(constraint2);

    auto& storage2 = area0[1];
    storage2.series = std::make_shared<Antares::Data::ShortTermStorage::Series>();
    storage2.series->inflows.resize(HOURS_PER_YEAR, 0.0); // Default inflow
    storage2.additionalConstraints.push_back(additionalConstraint2);

    // Expected sum for :
    // constaint1 = (10.0 + 15.0) = 25.0
    //  constaint2 = (5.0 + 7.0) = 12.0
    return {{.constraint_index = problemeHebdo.CorrespondanceCntNativesCntOptimHebdomadaires
                                   .ShortTermStorageCumulation[constraint1.globalIndex],
             .rhs = 25.0},
            {.constraint_index = problemeHebdo.CorrespondanceCntNativesCntOptimHebdomadaires
                                   .ShortTermStorageCumulation[constraint2.globalIndex],
             .rhs = 12.0}};
}

BOOST_AUTO_TEST_CASE(TestMultipleStoragesSameArea)
{
    PROBLEME_HEBDO problemeHebdo;
    const int numberOfAreas = 1;
    const int numberOfConstraints = 5;
    const int numberOfTimeSteps = 24;

    SetupProblemHebdo(problemeHebdo, numberOfAreas, numberOfConstraints, numberOfTimeSteps);
    const auto expected_results = SetupMultipleStoragesSameArea(problemeHebdo);
    // Call function
    OPT_InitialiserLeSecondMembreDuProblemeLineaire(&problemeHebdo, 0, 24, 0, 1);

    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre = *problemeHebdo.ProblemeAResoudre;
    for (const auto& [constraint_index, expected_rhs]: expected_results)
    {
        BOOST_CHECK_CLOSE(problemeAResoudre.SecondMembre[constraint_index], expected_rhs, 0.001);
    }
}
