#define BOOST_TEST_MODULE multi storage remix

#define WIN32_LEAN_AND_MEAN

#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

using namespace Antares::Solver::Simulation;

template<unsigned int size>
struct STS_setup
{
    STS_setup()
    {
        withdrawal.assign(size, 0);
        injection.assign(size, 0);
        levels.assign(size, 0.);
        pmax.assign(size, std::numeric_limits<double>::max());
        inflows.assign(size, 0.);
        ovf.assign(size, 0.);
        lowRuleCurve.assign(size, 0.);
        upRuleCurve.assign(size, capacity);
    }

    std::shared_ptr<IStorageForRemix> createSTS(std::vector<double>& unsupE)
    {
        return makeSTSforRemix(withdrawal,
                               unsupE,
                               levels,
                               pmax,
                               inflows,
                               injection,
                               lowRuleCurve,
                               upRuleCurve,
                               initLevel,
                               withdrawalEff,
                               injectionEff);
    }

    std::vector<double> withdrawal, injection, levels, pmax, inflows, ovf;
    std::vector<double> lowRuleCurve, upRuleCurve;

    double initLevel = 0.;
    double capacity = std::numeric_limits<double>::max();
    const double withdrawalEff = 1.;
    const double injectionEff = 1.;
};

template<unsigned int size>
struct InputFixture
{
    InputFixture()
    {
    }

    void callRemixStorageAlgorithm()
    {
        storagesForRemix.clear();
        storagesForRemix.push_back(sts1.createSTS(UnsupE));
        storagesForRemix.push_back(sts2.createSTS(UnsupE));
        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);
    }

    STS_setup<size> sts1;
    STS_setup<size> sts2;

    std::vector<double> TotalGenNoHydro, Load, UnsupE, Spillage, DTG_MRG;
    ListStorageForRemix storagesForRemix;
};

BOOST_AUTO_TEST_CASE(dummy_test)
{
    BOOST_CHECK(true);
}
