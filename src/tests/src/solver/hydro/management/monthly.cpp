#include <memory>
#define BOOST_TEST_MODULE hydro monthly
#include <boost/test/unit_test.hpp>

#include <antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h>
#include <antares/solver/hydro/monthly/h2o_m_fonctions.h>
#include <antares/solver/hydro/probleme_spx_wrapper.h>

namespace DonneesOptimisationMensuelle
{

BOOST_AUTO_TEST_CASE(TestInitialization)
{
    DONNEES_ANNUELLES data = H2O_M_Instanciation(1);

    H2O_M_InitialiserBornesEtCoutsDesVariables(data);

    BOOST_CHECK_EQUAL(data.Volume[0], data.VolumeInitial);

    auto& Xmin = data.ProblemeHydraulique.ProblemeLineairePartieVariable.Xmin;
    auto& Xmax = data.ProblemeHydraulique.ProblemeLineairePartieVariable.Xmax;
    int Var = data.ProblemeHydraulique.CorrespondanceDesVariables.NumeroDeVariableVolume[0];
    BOOST_CHECK_EQUAL(Xmin[Var], data.VolumeInitial);
    BOOST_CHECK_EQUAL(Xmax[Var], data.VolumeInitial);

    auto& CoutLineaire = data.ProblemeHydraulique.ProblemeLineairePartieFixe.CoutLineaire;
    for (int Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = data.ProblemeHydraulique.CorrespondanceDesVariables
                .NumeroDeVariableDepassementVolumeMax[Pdt];
        BOOST_CHECK_EQUAL(CoutLineaire[Var], data.CoutDepassementVolume);
    }

    Var = data.ProblemeHydraulique.CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
    BOOST_CHECK_EQUAL(CoutLineaire[Var], data.CoutViolMaxDuVolumeMin);
}

BOOST_AUTO_TEST_CASE(Test_optimiser_une_annee)
{
    DONNEES_ANNUELLES data = H2O_M_Instanciation(1);

    // invalid reservoir number, should return without resolving the problem
    H2O_M_OptimiserUneAnnee(data, -1);
    H2O_M_OptimiserUneAnnee(data, 2);

    BOOST_CHECK(data.ProblemeHydraulique.ProblemeSpx[0] == nullptr);

    // valid reservoir number, should resolve the problem
    H2O_M_OptimiserUneAnnee(data, 0);
    BOOST_CHECK(data.ProblemeHydraulique.ProblemeSpx[0] != nullptr);
}

} // namespace DonneesOptimisationMensuelle
