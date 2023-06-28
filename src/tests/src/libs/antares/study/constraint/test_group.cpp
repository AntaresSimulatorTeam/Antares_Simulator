//
// Created by marechaljas on 28/06/23.
//

#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <fstream>
#include "antares/study/study.h"

using namespace Antares::Data;
namespace fs = std::filesystem;

BOOST_AUTO_TEST_SUITE(BindingConstraintTests_Groups)

class PublicStudy: public Study {
public:
    bool internalLoadBindingConstraints(const StudyLoadOptions& options) override {
        return Study::internalLoadBindingConstraints(options);
    }
};

BOOST_AUTO_TEST_CASE(WhenLoadingAConstraint_AGroupExists) {
    auto study = std::make_shared<PublicStudy>();

    StudyLoadOptions options;

    auto tmp_dir = fs::temp_directory_path();
    auto working_tmp_dir = tmp_dir / std::tmpnam(nullptr);
    fs::create_directories(working_tmp_dir / "bindingconstraints");

    std::ofstream constraints(working_tmp_dir / "bindingconstraints" / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                <<"id = dummy_id\n"
                << "enabled = false\n"
                << "type = hourly\n"
                << "operator = equal\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "group = dummy_group\n"
            ;
    constraints.close();
    std::ofstream rhs(working_tmp_dir / "bindingconstraints"/ "dummy_id_eq.txt");
    rhs.close();

    study->header.version = version870;

    study->folderInput = working_tmp_dir.string();
    const bool loading_ok = study->internalLoadBindingConstraints(options);

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(study->bindingConstraints.size(), 1);
    BOOST_CHECK_EQUAL(study->bindingConstraintsGroups.size(), 1);

    auto constraint = *study->bindingConstraints.begin();

    BOOST_CHECK_EQUAL(constraint->name(), "dummy_name");
    BOOST_CHECK_EQUAL(constraint->id(), "dummy_id");
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->type(), BindingConstraint::Type::typeHourly);
    BOOST_CHECK_EQUAL(constraint->operatorType(), BindingConstraint::Operator::opEquality);
    BOOST_CHECK_EQUAL(constraint->yearByYearFilter(), FilterFlag::filterAnnual);
    BOOST_CHECK_EQUAL(constraint->synthesisFilter(), FilterFlag::filterHourly);
    BOOST_CHECK_EQUAL(constraint->comments(), "dummy_comment");
    BOOST_CHECK_EQUAL(constraint->group(), "dummy_group");
}


BOOST_AUTO_TEST_SUITE_END()