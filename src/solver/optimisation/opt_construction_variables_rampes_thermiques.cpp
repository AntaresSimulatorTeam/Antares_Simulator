#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_rename_problem.h"

#include "spx_constantes_externes.h"
#include "variables/VariableManagerUtils.h"


void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireRampesThermiques(PROBLEME_HEBDO* problemeHebdo, bool Simulation)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int& NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        auto& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] >= 0 )
                {
                    const int palier = PaliersThermiquesDuPays
                                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                    const auto& clusterName
                      = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];

                    if (!Simulation)
                    {
                        CorrespondanceVarNativesVarOptim.powerRampingIncreaseIndex[palier]
                          = NombreDeVariables;
                        ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                          = VARIABLE_BORNEE_DES_DEUX_COTES;
                        variableNamer.ProductionIncreaseAboveMin(NombreDeVariables, clusterName);
                    }
                    NombreDeVariables++;

                    if (!Simulation)
                    {
                        CorrespondanceVarNativesVarOptim.powerRampingDecreaseIndex[palier]
                          = NombreDeVariables;
                        ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                          = VARIABLE_BORNEE_DES_DEUX_COTES;
                        variableNamer.ProductionDecreaseAboveMin(NombreDeVariables, clusterName);
                    }
                    NombreDeVariables++;
                }
            }
        }
    }
}
