#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

#include "antares/optim/api/LinearProblemBuilder.h"
#include "antares/optim/impl/LinearProblemImpl.h"
#include "../include/Battery.h"
#include "../include/Thermal.h"
#include "../include/Balance.h"
#include "../include/ProductionPriceMinimization.h"

using namespace Antares::optim::api;

BOOST_AUTO_TEST_CASE(test_productionMaxP_notEnough)
{
    vector<int> timesteps{0, 1, 2, 3}; // toujours commencer à 0 sinon ça plante actuellement
    int timestep = 60;

    vector<Battery*> batteries;
    vector<Thermal*> thermals;

    LinearProblemImpl linearProblem(true, "xpress");
    LinearProblemBuilder linearProblemBuilder(linearProblem);
    Battery battery(timesteps, timestep, 100, 1000, 0);
    batteries.push_back(&battery);

    vector<double> pCost{1, 3, 10, 8};
    Thermal thermal(timesteps, 100, pCost);
    thermals.push_back(&thermal);

    vector<double> consumption{50, 50, 150, 120};
    Balance balance(timesteps, "nodeA", batteries, thermals, consumption);

    ProductionPriceMinimization objective(timesteps, thermals);

    linearProblemBuilder.addFiller(battery);
    linearProblemBuilder.addFiller(thermal);
    linearProblemBuilder.addFiller(balance);
    linearProblemBuilder.addFiller(objective);

    LinearProblemData linearProblemData;
    linearProblemBuilder.build(linearProblemData);
    auto solution = linearProblemBuilder.solve();

    // la conso est supérieure à la Pmax du thermique sur les pas de temps 2 & 3
    // Donc la batterie doit se charger pendant les TS 0 & 1
    // En plus, le coût de la production est supérieur en 2 & 3, donc la batterie devrait se charger à fond en 0 & 1
    // La conso en 0 & 1 est de 50, donc la batterie peut se recharger de 50
    // Elle répartira sa charge surtout en 2 (prix thermique plus cher qu'en 3), en gardant au moins 20
    // pour le pdt 3, dans lequel la conso est de 120 et la prod thermique max de 100
    // Le thermique complète
    // d'où le plan optimal
    // thermique : 100, 100, 70, 100
    // batterie : -50, -50, 80, 20

    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(thermal.getPVarName(0))) == 100, tt::tolerance(0.001));
    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(thermal.getPVarName(1))) == 100, tt::tolerance(0.001));
    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(thermal.getPVarName(2))) == 70, tt::tolerance(0.001));
    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(thermal.getPVarName(3))) == 100, tt::tolerance(0.001));

    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(battery.getPVarName(0))) == -50, tt::tolerance(0.001));
    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(battery.getPVarName(1))) == -50, tt::tolerance(0.001));
    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(battery.getPVarName(2))) == 80, tt::tolerance(0.001));
    BOOST_TEST(solution.getOptimalValue(linearProblem.getVariable(battery.getPVarName(3))) == 20, tt::tolerance(0.001));
}