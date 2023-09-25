#include <vector>
#include <antares/mersenne-twister/mersenne-twister.h>
#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

namespace Constants
{
constexpr double noiseAmplitude = 1e-3;
constexpr unsigned int seed = 0x79686d64; // "hydm" in hexa
} // namespace Constants

void H2O_M_AjouterBruitAuCout(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
        = ProblemeHydraulique.ProblemeLineairePartieFixe;
    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables
        = ProblemeHydraulique.CorrespondanceDesVariables;
    auto& CoutLineaireBruite = ProblemeLineairePartieFixe.CoutLineaireBruite;
    const auto& CoutLineaire = ProblemeLineairePartieFixe.CoutLineaire;

    Antares::MersenneTwister noiseGenerator;
    noiseGenerator.reset(Constants::seed); // Arbitrary seed, hard-coded since we don't really want
                                           // the user to change it
    const std::vector<std::vector<int>*> monthlyVariables
      = {&CorrespondanceDesVariables.NumeroDeVariableVolume,
         &CorrespondanceDesVariables.NumeroDeVariableTurbine,
         &CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax,
         &CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin,
         &CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible,
         &CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible};

    for (const auto variable : monthlyVariables)
    {
        for (int Pdt = 0; Pdt < DonneesAnnuelles.NombreDePasDeTemps; Pdt++)
        {
            int Var = (*variable)[Pdt];
            CoutLineaireBruite[Var]
              = CoutLineaire[Var] + noiseGenerator() * Constants::noiseAmplitude;
        }
    }
    int Var = CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
    CoutLineaireBruite[Var] = CoutLineaire[Var] + noiseGenerator() * Constants::noiseAmplitude;

    Var = CorrespondanceDesVariables.NumeroDeLaVariableXi;
    CoutLineaireBruite[Var] = CoutLineaire[Var] + noiseGenerator() * Constants::noiseAmplitude;
}
