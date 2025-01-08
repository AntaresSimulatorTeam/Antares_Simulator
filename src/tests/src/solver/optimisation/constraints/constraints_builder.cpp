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

#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/antares/constants.h"


struct BB
{
    int nombreDePasDeTempsPourUneOptimisation = 10;

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
    std::vector<CORRESPONDANCES_DES_VARIABLES> CorrespondanceVarNativesVarOptim;;

    void set_correspondances_des_variables()
    {
        CorrespondanceVarNativesVarOptim.resize(nombreDePasDeTempsPourUneOptimisation);
        // for every timestep the same indices
        // .InjectionVariable = {0, 1} :
        // --> storage 1 --> injection index = 0
        // --> storage 2 --> injection index = 1

        for (auto i = 0; i < nombreDePasDeTempsPourUneOptimisation; i++)
        {
            CorrespondanceVarNativesVarOptim[i].SIM_ShortTermStorage = {
                    .InjectionVariable = {0, 1}, .WithdrawalVariable = {2, 3}};
        }
    }

    //.S {{.SIM_ShortTermStorage = {.InjectionVariable = {0,1}, .WithdrawalVariable = {2,3} } }};
    const int32_t NombreDePasDeTempsPourUneOptimisation = nombreDePasDeTempsPourUneOptimisation;
    // Example value
    std::vector<int> NumeroDeVariableStockFinal = std::vector<int>(10, -1);
    std::vector<std::vector<int> > NumeroDeVariableDeTrancheDeStock = std::vector<std::vector<
        int> >(10, std::vector<int>(5, -1));
    std::vector<std::string> NomDesContraintes = std::vector<std::string>(100, "");
    const bool NamedProblems = true;
    const std::vector<const char*> NomsDesPays = {"CountryA", "CountryB"};
    const uint32_t weekInTheYear = 1;        // Example week
    const uint32_t NombreDePasDeTemps = 168; // Example number of time steps in a week

    // Mock data storage


    std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>
    addc1_withdrawal_constraints = {
            {.hours = {1, 2},
             .globalIndex = 0,
             .localIndex = 0},
            {.hours = {3, 4},
             .globalIndex = 1,
             .localIndex = 1}};

    std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>
    addc2_injection_constraints = {
            {.hours = {5, 6},
             .globalIndex = 2,
             .localIndex = 0},
            {.hours = {7, 8},
             .globalIndex = 3,
             .localIndex = 1}};

    std::vector<double> fill_rhs()
    {
        std::vector<double> ret(HOURS_PER_YEAR);
        std::iota(ret.begin(), ret.end(), 0);
        return ret;
    };
    Antares::Data::ShortTermStorage::AdditionalConstraints addc1_withdrawal = {
            .name = "addc1_withdrawal", .cluster_id = "cluster_1", .variable = "withdrawal",
            .operatorType = "less", .rhs = fill_rhs(), .constraints = addc1_withdrawal_constraints};
    Antares::Data::ShortTermStorage::AdditionalConstraints addc2_injection = {
            .name = "addc2_injection", .cluster_id = "cluster_2", .variable = "injection",
            .operatorType = "greater", .rhs = fill_rhs(),
            .constraints = addc2_injection_constraints};


    ::ShortTermStorage::PROPERTIES storage1 = {.additionalConstraints = {addc1_withdrawal},
                                               .clusterGlobalIndex = 0, .name = "cluster_1"};
    ::ShortTermStorage::PROPERTIES storage2 = {.additionalConstraints = {addc2_injection},
                                               .clusterGlobalIndex = 1, .name = "cluster_2"};

    std::vector<CORRESPONDANCES_DES_CONTRAINTES> CorrespondanceCntNativesCntOptim;
    std::vector<::ShortTermStorage::AREA_INPUT> shortTermStorage = InitializeShortTermStorageData();
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES CorrespondanceCntNativesCntOptimHebdomadaires{
            {}, std::vector<int>(20, 0)};


    std::vector<ShortTermStorage::AREA_INPUT> InitializeShortTermStorageData()
    {
        return {{storage1}, {storage2}};
    }


    ShortTermStorageCumulativeConstraintData shorttermstoragecumulativeconstraintdata =
            InitializeShortTermStorageCumulativeConstraintData();


    ShortTermStorageCumulativeConstraintData InitializeShortTermStorageCumulativeConstraintData()
    {
        return {CorrespondanceCntNativesCntOptim, shortTermStorage,
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
        return {
                Pi,
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
                      "WithdrawalSum::area<CountryA>::ShortTermStorage<cluster_1>::Constraint<addc1_withdrawal_0>")
    ; // Assuming this is the generated name
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[1],
                      "WithdrawalSum::area<CountryA>::ShortTermStorage<cluster_1>::Constraint<addc1_withdrawal_1>")
    ; // Check the second constraint
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
            shorttermstoragecumulativeconstraintdata
            .CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation[
                addc1_withdrawal_constraints[0].globalIndex],
            0);
    BOOST_CHECK_EQUAL(
            shorttermstoragecumulativeconstraintdata.
            CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation[
                addc1_withdrawal_constraints[1].globalIndex],
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
                      "InjectionSum::area<CountryB>::ShortTermStorage<cluster_2>::Constraint<addc2_injection_0>")
    ; // Assuming this is the generated name
    BOOST_CHECK_EQUAL(builder.data.NomDesContraintes[1],
                      "InjectionSum::area<CountryB>::ShortTermStorage<cluster_2>::Constraint<addc2_injection_1>")
    ; // Check the second constraint
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
            shorttermstoragecumulativeconstraintdata
            .CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation[
                addc2_injection_constraints[0].globalIndex],
            0);
    BOOST_CHECK_EQUAL(
            shorttermstoragecumulativeconstraintdata.
            CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation[
                addc2_injection_constraints[1].globalIndex],
            1);
}
