#define BOOST_TEST_MODULE test check input data

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "antares/checks/checkLoadedInputData.h"
#include "antares/study/version.h"

using namespace Antares::Data;
using namespace Antares::Check;

BOOST_AUTO_TEST_SUITE(check_study_version)

BOOST_AUTO_TEST_CASE(study_version_is_unknown___exception_raised)
{
    StudyVersion version;
    const AnyString StudyFolder; // Whaterver the value
    std::string err_msg = "does not seem to be a valid study";
    BOOST_CHECK_EXCEPTION(checkStudyVersion(version, StudyFolder),
                          std::runtime_error,
                          containsMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(study_version_is_too_high___exception_raised)
{
    StudyVersion version(9999, 8888);
    const AnyString StudyFolder; // Whaterver the value
    std::string err_msg = "Invalid version for the study : found";
    BOOST_CHECK_EXCEPTION(checkStudyVersion(version, StudyFolder),
                          std::runtime_error,
                          containsMessage(err_msg));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_simplex_range_and_hydro_pricing)

BOOST_AUTO_TEST_CASE(splx_optim_range_and_hydro_pricing_are_incompatible___exception_raised)
{
    SimplexOptimization splxOptimRange = SimplexOptimization::sorDay;
    HydroPricingMode hydroPricingMode = HydroPricingMode::hpMILP;
    std::string err_msg = "Simplex optimization range and hydro pricing mode : values are not "
                          "compatible ";
    BOOST_CHECK_EXCEPTION(checkSimplexRangeHydroPricing(splxOptimRange, hydroPricingMode),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_simplex_range_and_unit_commitment)

BOOST_AUTO_TEST_CASE(splx_optim_range_and_unit_commitment_are_incompatible___exception_raised)
{
    SimplexOptimization splxOptimRange = SimplexOptimization::sorDay;
    UnitCommitmentMode ucMode = UnitCommitmentMode::ucMILP;
    std::string err_msg = "Simplexe optimization range and unit commitment mode : values are not "
                          "compatible";
    BOOST_CHECK_EXCEPTION(checkSimplexRangeUnitCommitmentMode(splxOptimRange, ucMode),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()
