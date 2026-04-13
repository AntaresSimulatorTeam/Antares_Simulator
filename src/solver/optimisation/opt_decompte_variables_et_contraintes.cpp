// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/antares/fatal-error.h>
#include "antares/solver/optimisation/opt_decompte_variables_et_contraintes_reserves.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares;

void OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(PROBLEME_HEBDO*);

// This estimate is not very accurate, some constraints may not be enabled
// in some cases.
// The counting below should be removed, and we should instead use other methods
// involving dynamic reallocations, etc.
int OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    int nombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    // =============================
    // Counting variables
    // =============================
    int mxPaliers = 0;
    ProblemeAResoudre->NombreDeVariables = problemeHebdo->NombreDInterconnexions;

    ProblemeAResoudre->NombreDeVariables += 2 * problemeHebdo->NombreDInterconnexions;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        ProblemeAResoudre->NombreDeVariables += problemeHebdo->PaliersThermiquesDuPays[pays]
                                                  .NombreDePaliersThermiques;

        mxPaliers += problemeHebdo->PaliersThermiquesDuPays[pays].NombreDePaliersThermiques;

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
        {
            ProblemeAResoudre->NombreDeVariables++;
            ProblemeAResoudre->NombreDeVariables++;
        }

        ProblemeAResoudre->NombreDeVariables += 2;
    }
    ProblemeAResoudre->NombreDeVariables *= nombreDePasDeTempsPourUneOptimisation;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            ProblemeAResoudre->NombreDeVariables += 1;   /* Final Stock Level */
            ProblemeAResoudre->NombreDeVariables += 100; /* Reservoir layers  */
        }
    }

    // =============================
    // Counting constraints
    // =============================

    ProblemeAResoudre->NombreDeContraintes = problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDePays;

    // For constraint : max unsupplied energy
    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDInterconnexions;

    // Hourly binding constraints
    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante].TypeDeContrainteCouplante
            == CONTRAINTE_HORAIRE)
        {
            ProblemeAResoudre->NombreDeContraintes++;
        }
    }

    // Very important : make previous constraints hourly constraints : we multiply by nb of hours
    ProblemeAResoudre->NombreDeContraintes *= nombreDePasDeTempsPourUneOptimisation;

    int nombreDeJoursDansUnIntervalleOptimise;
    if (nombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        nombreDeJoursDansUnIntervalleOptimise = problemeHebdo->NombreDePasDeTemps
                                                / problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
    else
    {
        nombreDeJoursDansUnIntervalleOptimise = 1;
    }

    // Daily binding constraints
    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante].TypeDeContrainteCouplante
            == CONTRAINTE_JOURNALIERE)
        {
            ProblemeAResoudre->NombreDeContraintes += nombreDeJoursDansUnIntervalleOptimise;
        }
    }

    // Weekly binding constraints
    if (nombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante]
                  .TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                ProblemeAResoudre->NombreDeContraintes++;
            }
        }
    }

    // Hydro
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        char Pump = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable;
        char TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                 .TurbinageEntreBornes;

        if (!Pump && !TurbEntreBornes
            && problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            ProblemeAResoudre->NombreDeContraintes++;
        }

        ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;

        if (Pump && !TurbEntreBornes)
        {
            ProblemeAResoudre->NombreDeContraintes += 2; /* 2 constraints bounding the overall
                                                            energy generated over the period (10a in
                                                            the reference document) */
            ProblemeAResoudre->NombreDeContraintes++;
            /* 1 constraint setting the level variation over the period
                                               (10b in the reference document) */
            ProblemeAResoudre->NombreDeContraintes++;
            /* 1 constraint bounding the overall energy pumped over the
                                               period (10c in the reference document) */
        }

        if (!Pump && TurbEntreBornes)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (Pump && TurbEntreBornes)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (!Pump && TurbEntreBornes)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
        }

        if (Pump && TurbEntreBornes)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
        }
        if (Pump && !TurbEntreBornes)
        {
            ProblemeAResoudre->NombreDeContraintes += 2; /* 2 constraints bounding the overall
                                                            energy generated over the period (10a in
                                                            the reference document) */
            ProblemeAResoudre->NombreDeContraintes++;
            /* 1 constraint setting the level variation over the period
                                               (10b in the reference document) */
            ProblemeAResoudre->NombreDeContraintes++;
            /* 1 constraint bounding the overall energy pumped over the
                                               period (10c in the reference document) */
            ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
            /* T constraints expressing the level hourly
                                                                variations (14a in the reference
                                                                document) */
        }
    }

    // Short term storage
    {
        const uint nbSTS = problemeHebdo->NumberOfShortTermStorages;
        // Level, injection, withdrawal
        ProblemeAResoudre->NombreDeVariables += 3 * nbSTS * nombreDePasDeTempsPourUneOptimisation;
        // Level equation (Level[h+1] = Level[h] + ...)
        ProblemeAResoudre->NombreDeContraintes += nbSTS * nombreDePasDeTempsPourUneOptimisation;

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            for (const auto& storage: problemeHebdo->ShortTermStorage[pays])
            {
                /*
                 * ShortTermStorageCostVariationInjectionBackward
                 * ShortTermStorageCostVariationInjectionForward
                 */
                if (storage.penalizeVariationInjection)
                {
                    // CostVariationInjection
                    ProblemeAResoudre->NombreDeVariables += nombreDePasDeTempsPourUneOptimisation;
                    ProblemeAResoudre->NombreDeContraintes
                      += 2 * nombreDePasDeTempsPourUneOptimisation;
                }

                /*
                 * ShortTermStorageCostVariationWithdrawalBackward
                 * ShortTerStorageCostVariationWithdrawalForward
                 */
                if (storage.penalizeVariationWithdrawal)
                {
                    // CostVariationWithdrawal
                    ProblemeAResoudre->NombreDeVariables += nombreDePasDeTempsPourUneOptimisation;
                    ProblemeAResoudre->NombreDeContraintes
                      += 2 * nombreDePasDeTempsPourUneOptimisation;
                }
                if (!storage.additionalConstraints.empty())
                {
                    for (const auto& additionalConstraints: storage.additionalConstraints)
                    {
                        ProblemeAResoudre->NombreDeContraintes += additionalConstraints
                                                                    ->enabledConstraintsCount();
                    }
                }
                if (storage.allowOverflow)
                {
                    ProblemeAResoudre->NombreDeVariables += nbSTS
                                                            * nombreDePasDeTempsPourUneOptimisation;
                }
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                ProblemeAResoudre->NombreDeVariables += nombreDePasDeTempsPourUneOptimisation * 2;
                ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
            }
            else if (problemeHebdo->TypeDeLissageHydraulique
                     == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
            {
                ProblemeAResoudre->NombreDeVariables += 2;
                ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation * 2;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            ProblemeAResoudre->NombreDeContraintes += 2; /* Final Stock Level : (1 equivalence cnt +
                                                            1 expression cnt )*/
        }
    }

    if (problemeHebdo->OptimisationNotFastMode)
    {
        OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(problemeHebdo);
        if (problemeHebdo->allReserves)
        {
            OPT_DecompteDesVariablesEtDesContraintesReserves(problemeHebdo);
        }
    }

    return mxPaliers;
}
