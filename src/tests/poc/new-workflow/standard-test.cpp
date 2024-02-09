#define BOOST_TEST_MODULE poc tests
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace tt = boost::test_tools;

#include "antares/optim/api/LinearProblemBuilder.h"
#include "antares/optim/impl/LinearProblemImpl.h"
#include "../include/standard/ComponentFiller.h"

using namespace Antares::optim::api;

BOOST_AUTO_TEST_CASE(test_std_oneWeek_oneNode_oneBattery_oneThermal)
{
    vector<int> timeStamps{0, 1, 2, 3}; // toujours commencer à 0 sinon ça plante actuellement
    int timeResolution = 60;

    LinearProblemImpl linearProblem(false, "xpress");
    LinearProblemBuilder linearProblemBuilder(linearProblem);
    PortConnexionsManager portConnexionsManager;

    Component thermal("thermal1", THERMAL, {{"maxP", 100.0}}, {});
    ComponentFiller thermal1Filler(thermal, portConnexionsManager);

    Component battery("battery1", BATTERY, {{"maxP", 100}, {"maxStock", 1000}}, {});
    ComponentFiller battery1Filler(battery, portConnexionsManager);

    Component balance("balanceA", BALANCE, {}, {{"nodeName", "nodeA"}});
    ComponentFiller balanceAFiller(balance, portConnexionsManager);
    portConnexionsManager.addConnexion({&balanceAFiller, "P"}, {&thermal1Filler, "P"});
    portConnexionsManager.addConnexion({&balanceAFiller, "P"}, {&battery1Filler, "P"});

    Component priceMinim("priceMinim", PRICE_MINIM, {}, {});
    ComponentFiller priceMinimFiller(priceMinim, portConnexionsManager);
    portConnexionsManager.addConnexion({&priceMinimFiller, "cost"}, {&thermal1Filler, "cost"});
    portConnexionsManager.addConnexion({&priceMinimFiller, "cost"}, {&battery1Filler, "cost"});

    linearProblemBuilder.addFiller(thermal1Filler);
    linearProblemBuilder.addFiller(battery1Filler);
    linearProblemBuilder.addFiller(balanceAFiller);
    linearProblemBuilder.addFiller(priceMinimFiller);

    LinearProblemData linearProblemData(
            timeStamps, // TODO : move to LinearProblem ?
            timeResolution, // TODO : move to LinearProblem ?
            {
                    {"initialStock_battery1", 0}
            },
            {
                    {"consumption_nodeA", {50, 50, 150, 120}},
                    {"cost_thermal1",     {1,  3,  10,  8}}
            });
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

    vector<double> actualThermalP = solution.getOptimalValues({"P_thermal1_0", "P_thermal1_1", "P_thermal1_2", "P_thermal1_3"});
    vector<double> expectedThermalP({100., 100., 70., 100.});
    BOOST_TEST(actualThermalP == expectedThermalP, tt::per_element()); // TODO add tolerance?

    vector<double> actualBatteryP = solution.getOptimalValues({"P_battery1_0", "P_battery1_1", "P_battery1_2", "P_battery1_3"});
    vector<double> expectedBatteryP({-50, -50, 80, 20});
    BOOST_TEST(actualBatteryP == expectedBatteryP, tt::per_element()); // TODO add tolerance?
}

BOOST_AUTO_TEST_CASE(test_std_oneWeek_oneNode_oneBattery_twoThermals)
{
    /*vector<int> timeStamps{0, 1, 2, 3}; // toujours commencer à 0 sinon ça plante actuellement
    int timeResolution = 60;

    LinearProblemImpl linearProblem(false, "xpress");
    LinearProblemBuilder linearProblemBuilder(linearProblem);

    ComponentFiller battery1("battery1", 180, 200);
    ComponentFiller thermal1("thermal1", 100);
    ComponentFiller thermal2("thermal2", 100);
    ComponentFiller balance("nodeA", {&battery1}, {&thermal1, &thermal2});
    ComponentFiller objective({&thermal1, &thermal2});

    linearProblemBuilder.addFiller(battery1);
    linearProblemBuilder.addFiller(thermal1);
    linearProblemBuilder.addFiller(thermal2);
    linearProblemBuilder.addFiller(balance);
    linearProblemBuilder.addFiller(objective);

    LinearProblemData linearProblemData(
            timeStamps, // TODO : move to LinearProblem ?
            timeResolution, // TODO : move to LinearProblem ?
            {
                    {"initialStock_battery1", 0}
            },
            {
                    {"consumption_nodeA", {0, 150, 150, 150}},
                    {"cost_thermal1",     {1,  4,  8,  11}},
                    {"cost_thermal2",     {2,  3,  10,  9}}
            });
    linearProblemBuilder.build(linearProblemData);
    auto solution = linearProblemBuilder.solve();

    // La production thermique n'est pas chère au pas 1 et très chère ensuite
    // La batterie a intérêt à se charger au plus bas prix (pas 1), se décharger au max quand le prix est très élevé
    // (par ordre de priorité pas 4 puis pas 3). Elle est limitée en stock et en puissance, elle peut charger 180 MW en 1
    // et 20 MW en 2.
    // Les groupes complètent selon leur merit order de prix

    vector<double> actualBatteryP = solution.getOptimalValues({"P_battery1_0", "P_battery1_1", "P_battery1_2", "P_battery1_3"});
    vector<double> expectedBatteryP({-180, -20, 50, 150});
    BOOST_TEST(actualBatteryP == expectedBatteryP, tt::per_element()); // TODO add tolerance?

    vector<double> actualThermal1P = solution.getOptimalValues({"P_thermal1_0", "P_thermal1_1", "P_thermal1_2", "P_thermal1_3"});
    vector<double> expectedThermal1P({100., 70., 100., 0.});
    BOOST_TEST(actualThermal1P == expectedThermal1P, tt::per_element()); // TODO add tolerance?
    vector<double> actualThermal2P = solution.getOptimalValues({"P_thermal2_0", "P_thermal2_1", "P_thermal2_2", "P_thermal2_3"});
    vector<double> expectedThermal2P({80., 100., 0., 0.});
    BOOST_TEST(actualThermal2P == expectedThermal2P, tt::per_element()); // TODO add tolerance?*/
}