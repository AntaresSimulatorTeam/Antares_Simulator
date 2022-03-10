#include <vector>
#include <antares/mersenne-twister/mersenne-twister.h>
#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

namespace Constants
{
constexpr double denom = 1e3;
constexpr unsigned int seed = 0x79686d64; // "hydm" in hexa
} // namespace Constants

void H2O_M_AjouterBruitAuCout(DONNEES_ANNUELLES* DonneesAnnuelles)
{
    CORRESPONDANCE_DES_VARIABLES* CorrespondanceDesVariables;
    double* CoutLineaire;
    int Var;
    int Pdt;
    int NbPdt;
    // We don't want to keep variables ProblemeHydraulique and ProblemeLineairePartieFixe
    {
        PROBLEME_HYDRAULIQUE* ProblemeHydraulique;
        PROBLEME_LINEAIRE_PARTIE_FIXE* ProblemeLineairePartieFixe;
        ProblemeHydraulique = DonneesAnnuelles->ProblemeHydraulique;
        ProblemeLineairePartieFixe = ProblemeHydraulique->ProblemeLineairePartieFixe;
        CorrespondanceDesVariables = ProblemeHydraulique->CorrespondanceDesVariables;
        CoutLineaire = ProblemeLineairePartieFixe->CoutLineaire;
        NbPdt = DonneesAnnuelles->NombreDePasDeTemps;
    }

    Antares::MersenneTwister noiseGenerator;
    noiseGenerator.reset(Constants::seed); // Arbitrary seed, hard-coded since we don't really want
                                           // the user to change it
    const std::vector<const int*> monthlyVariables
      = {CorrespondanceDesVariables->NumeroDeVariableVolume,
         CorrespondanceDesVariables->NumeroDeVariableTurbine,
         CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMax,
         CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMin,
         CorrespondanceDesVariables->NumeroDeVariableDEcartPositifAuTurbineCible,
         CorrespondanceDesVariables->NumeroDeVariableDEcartNegatifAuTurbineCible};

    for (const auto variable : monthlyVariables)
    {
        for (Pdt = 0; Pdt < NbPdt; Pdt++)
        {
            Var = variable[Pdt];
            CoutLineaire[Var] += noiseGenerator() / Constants::denom;
        }
    }
    Var = CorrespondanceDesVariables->NumeroDeLaVariableViolMaxVolumeMin;
    CoutLineaire[Var] += noiseGenerator() / Constants::denom;

    Var = CorrespondanceDesVariables->NumeroDeLaVariableXi;
    CoutLineaire[Var] += noiseGenerator() / Constants::denom;
}
