#include <string>
#include <vector>

#include <antares/solver/optimisation/opt_rename_problem.h>

#define BOOST_TEST_MODULE "constraints_namer"

#include <boost/test/unit_test.hpp>

constexpr std::size_t nbConstraints = 2;

// TODO add more tests

BOOST_AUTO_TEST_CASE(cumulation_constraint)
{
    int constraintGlobalIndex = 0;
    std::vector<std::string> constraintNames(nbConstraints);

    ConstraintNamer namer(constraintNames);
    namer.UpdateArea("fr");

    namer.UpdateTimeStep(33);
    namer.ShortTermStorageCumulation("a", constraintGlobalIndex++, "b", "c");
    BOOST_CHECK_EQUAL(constraintNames[0],
                      "a::area<fr>::ShortTermStorage<b>::Constraint<c>::week<33>");

    namer.UpdateTimeStep(34);
    namer.ShortTermStorageCumulation("a", constraintGlobalIndex++, "b", "c");
    BOOST_CHECK_EQUAL(constraintNames[1],
                      "a::area<fr>::ShortTermStorage<b>::Constraint<c>::week<34>");
}
