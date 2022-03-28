#include <antares/mersenne-twister/mersenne-twister.h>
#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

namespace Constants
{
constexpr double noiseAmplitude = 1e-3;
constexpr unsigned int seed = 0x79686a64; // "hydj" in hexa
} // namespace Constants

void H2O_J_AjouterBruitAuCout(DONNEES_MENSUELLES* donnesMensuelles)
{
    auto ProblemeHydraulique = donnesMensuelles->ProblemeHydraulique;
    auto ProblemeLineairePartieFixe = ProblemeHydraulique->ProblemeLineairePartieFixe;
    auto CorrespondanceDesVariables = ProblemeHydraulique->CorrespondanceDesVariables;
    auto NombreDeProblemes = ProblemeHydraulique->NombreDeProblemes;
    Antares::MersenneTwister noiseGenerator;
    noiseGenerator.reset(Constants::seed); // Arbitrary seed, hard-coded since we don't really want
                                           // the user to change it

    for (int i = 0; i < NombreDeProblemes; i++)
    {
        for (int j = 0; j < ProblemeLineairePartieFixe[i]->NombreDeVariables; j++)
        {
            ProblemeLineairePartieFixe[i]->CoutLineaire[j] += noiseGenerator() * Constants::noiseAmplitude;
        }

        ProblemeLineairePartieFixe[i]
          ->CoutLineaire[CorrespondanceDesVariables[i]->NumeroDeLaVariableMu]
          += noiseGenerator() * Constants::noiseAmplitude;
        ProblemeLineairePartieFixe[i]
          ->CoutLineaire[CorrespondanceDesVariables[i]->NumeroDeLaVariableXi]
          += noiseGenerator() * Constants::noiseAmplitude;
    }
}
