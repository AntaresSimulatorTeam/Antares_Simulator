#define BOOST_TEST_MODULE test save scenario - builder.dat

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <string>
#include <filesystem>

#include <study.h>
#include <rules.h>
#include <scenario-builder/sets.h>
#include "area/files-helper.h"

using namespace std;
using namespace Antares::Data;
namespace fs = std::filesystem;

const string generatedScBuilderFileName = "scenariobuilder.dat";
const string referenceScBuilderFileName = "scenariobuilder-reference.dat";

// ===========================================
// Reference scenario builder file handler
// ===========================================
class referenceScBuilderFile
{
public:
    referenceScBuilderFile() = default;
    ~referenceScBuilderFile() = default;
    string path() const
    {
        return absolute_path_;
    }
    void write();

    void append(const string line);

private:
    string absolute_path_ = fs::current_path().append(referenceScBuilderFileName).string();
    vector<string> content_;
};

void referenceScBuilderFile::append(const string line)
{
    content_.push_back(line);
}

void referenceScBuilderFile::write()
{
    ofstream file;
    file.open(absolute_path_);
    for (int i = 0; i < content_.size(); ++i)
        file << content_[i] << endl;
    file << endl;
}

// ===========================
// Add a cluster to an area
// ===========================

void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
    area->thermal.clusters.push_back(cluster.get());
    area->thermal.list.add(cluster);
    area->thermal.list.mapping[cluster->id()] = cluster;
}

void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
    area->renewable.clusters.push_back(cluster.get());
    area->renewable.list.add(cluster);
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
    auto cluster = std::make_shared<ClusterType>(area);
    cluster->setName(clusterName);
    cluster->series = new DataSeriesCommon();

    addClusterToAreaList(area, cluster);

    return cluster;
}

// ======================================
// Scenario builder common fixture
// ======================================
struct commonFixture
{
    commonFixture(const commonFixture& f) = delete;
    commonFixture(const commonFixture&& f) = delete;
    commonFixture& operator=(const commonFixture& f) = delete;
    commonFixture& operator=(const commonFixture&& f) = delete;
    commonFixture()
    {
        // Set study parameters
        study.parameters.nbYears = 20;
        study.parameters.timeSeriesToGenerate
          = 0; // No generated time-series, only ready made time-series

        // Add areas
        area_1 = study.areaAdd("Area 1");
        area_2 = study.areaAdd("Area 2");
        area_3 = study.areaAdd("Area 3");
        study.areas.rebuildIndexes();

        // Load : set the nb of ready made TS
        uint nbReadyMadeTS = 13;
        area_1->load.series->series.resize(nbReadyMadeTS, 1);
        area_2->load.series->series.resize(nbReadyMadeTS, 1);
        area_3->load.series->series.resize(nbReadyMadeTS, 1);

        // Wind : set the nb of ready made TS
        nbReadyMadeTS = 17;
        area_1->wind.series->series.resize(nbReadyMadeTS, 1);
        area_2->wind.series->series.resize(nbReadyMadeTS, 1);
        area_3->wind.series->series.resize(nbReadyMadeTS, 1);

        // Solar : set the nb of ready made TS
        nbReadyMadeTS = 9;
        area_1->solar.series->series.resize(nbReadyMadeTS, 1);
        area_2->solar.series->series.resize(nbReadyMadeTS, 1);
        area_3->solar.series->series.resize(nbReadyMadeTS, 1);

        // Hydro : set the nb of ready made TS
        nbReadyMadeTS = 12;
        area_1->hydro.series->count = nbReadyMadeTS;
        area_2->hydro.series->count = nbReadyMadeTS;
        area_3->hydro.series->count = nbReadyMadeTS;

        // Links
        link_12 = AreaAddLinkBetweenAreas(area_1, area_2, false);
        link_12->directCapacities.resize(15, 1);

        link_13 = AreaAddLinkBetweenAreas(area_1, area_3, false);
        link_13->directCapacities.resize(11, 1);

        link_23 = AreaAddLinkBetweenAreas(area_2, area_3, false);
        link_23->directCapacities.resize(8, 1);

        // Add thermal clusters
        thCluster_11 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-11");
        thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
        thCluster_31 = addClusterToArea<ThermalCluster>(area_3, "th-cluster-31");

        // Thermal clusters : set the nb of ready made TS
        thCluster_11->series->series.resize(14, 1);
        thCluster_12->series->series.resize(14, 1);
        thCluster_31->series->series.resize(14, 1);

        // Thermal clusters : update areas local numbering for clusters
        area_1->thermal.prepareAreaWideIndexes();
        area_2->thermal.prepareAreaWideIndexes();
        area_3->thermal.prepareAreaWideIndexes();

        // Add renewable clusters
        rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
        rnCluster_31 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-31");
        rnCluster_32 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-32");

        // Renewable clusters : set the nb of ready made TS
        rnCluster_21->series->series.resize(9, 1);
        rnCluster_31->series->series.resize(9, 1);
        rnCluster_32->series->series.resize(9, 1);

        // Renewable clusters : update areas local numbering for clusters
        area_1->renewable.prepareAreaWideIndexes();
        area_2->renewable.prepareAreaWideIndexes();
        area_3->renewable.prepareAreaWideIndexes();

        // Resize all TS numbers storage (1 column x nbYears lines)
        area_1->resizeAllTimeseriesNumbers(study.parameters.nbYears);
        area_2->resizeAllTimeseriesNumbers(study.parameters.nbYears);
        area_3->resizeAllTimeseriesNumbers(study.parameters.nbYears);

        // Scenario builder initialization
        study.scenarioRules = new ScenarioBuilder::Sets();
        study.scenarioRules->setStudy(study);
        my_rule = study.scenarioRules->createNew("my rule name");
        BOOST_CHECK(my_rule->reset());
    }

    ~commonFixture() = default;

    // Data members
    Study study;
    Area* area_1;
    Area* area_2;
    Area* area_3;
    AreaLink* link_12;
    AreaLink* link_13;
    AreaLink* link_23;
    std::shared_ptr<ThermalCluster> thCluster_11;
    std::shared_ptr<ThermalCluster> thCluster_12;
    std::shared_ptr<ThermalCluster> thCluster_31;
    std::shared_ptr<RenewableCluster> rnCluster_21;
    std::shared_ptr<RenewableCluster> rnCluster_31;
    std::shared_ptr<RenewableCluster> rnCluster_32;

    ScenarioBuilder::Rules::Ptr my_rule;
};

// ======================================
// Scenario builder save fixture
// ======================================

struct saveFixture : public commonFixture
{
    saveFixture() : commonFixture()
    {
    }
    ~saveFixture();
    void saveScenarioBuilder();

    // Data members
    std::string path_to_generated_file
      = fs::current_path().append(generatedScBuilderFileName).string();
    referenceScBuilderFile referenceFile;
};

void saveFixture::saveScenarioBuilder()
{
    study.scenarioRules->saveToINIFile(path_to_generated_file);
}

saveFixture::~saveFixture()
{
    vector<string> filesToRemove = {path_to_generated_file, referenceFile.path()};
    remove_files(filesToRemove);
}

// ==================
// Tests section
// ==================

BOOST_FIXTURE_TEST_SUITE(s, saveFixture)

// ====================
// Tests on Load
// ====================
BOOST_AUTO_TEST_CASE(
  LOAD__on_area2_and_year_11_chosen_ts_number_is_6__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->load.set(area_2->index, 11, 6);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("l,area 2,11 = 6");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

BOOST_AUTO_TEST_CASE(
  LOAD__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->load.set(area_3->index, 7, 2);
    my_rule->load.set(area_2->index, 11, 6);
    my_rule->load.set(area_1->index, 5, 12);
    my_rule->load.set(area_3->index, 8, 3);
    my_rule->load.set(area_2->index, 10, 4);
    my_rule->load.set(area_1->index, 18, 1);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("l,area 1,5 = 12");
    referenceFile.append("l,area 1,18 = 1");
    referenceFile.append("l,area 2,10 = 4");
    referenceFile.append("l,area 2,11 = 6");
    referenceFile.append("l,area 3,7 = 2");
    referenceFile.append("l,area 3,8 = 3");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// ====================
// Tests on Wind
// ====================
BOOST_AUTO_TEST_CASE(
  WIND__on_area3_and_year_19_chosen_ts_number_is_17__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->wind.set(area_3->index, 19, 17);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("w,area 3,19 = 17");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// ====================
// Tests on Solar
// ====================
BOOST_AUTO_TEST_CASE(
  SOLAR__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->solar.set(area_1->index, 9, 9);
    my_rule->solar.set(area_3->index, 18, 7);
    my_rule->solar.set(area_1->index, 5, 8);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("s,area 1,5 = 8");
    referenceFile.append("s,area 1,9 = 9");
    referenceFile.append("s,area 3,18 = 7");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// =================
// Tests on Hydro
// =================
BOOST_AUTO_TEST_CASE(
  HYDRO__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->hydro.set(area_2->index, 17, 12);
    my_rule->hydro.set(area_3->index, 18, 7);
    my_rule->hydro.set(area_1->index, 5, 8);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("h,area 1,5 = 8");
    referenceFile.append("h,area 2,17 = 12");
    referenceFile.append("h,area 3,18 = 7");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// ===========================
// Tests on Thermal clusters
// ===========================
BOOST_AUTO_TEST_CASE(
  THERMAL__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->thermal[area_3->index].set(thCluster_31.get(), 5, 13);
    my_rule->thermal[area_1->index].set(thCluster_11.get(), 19, 8);
    my_rule->thermal[area_1->index].set(thCluster_12.get(), 2, 4);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("t,area 1,19,th-cluster-11 = 8");
    referenceFile.append("t,area 1,2,th-cluster-12 = 4");
    referenceFile.append("t,area 3,5,th-cluster-31 = 13");

    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// =============================
// Tests on Renewable clusters
// =============================
BOOST_AUTO_TEST_CASE(
  RENEWABLE_CLUSTERS__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->renewable[area_3->index].set(rnCluster_32.get(), 5, 13);
    my_rule->renewable[area_2->index].set(rnCluster_21.get(), 19, 8);
    my_rule->renewable[area_3->index].set(rnCluster_31.get(), 2, 4);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("r,area 2,19,rn-cluster-21 = 8");
    referenceFile.append("r,area 3,2,rn-cluster-31 = 4");
    referenceFile.append("r,area 3,5,rn-cluster-32 = 13");

    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// ========================
// Tests on Hydro levels
// ========================
BOOST_AUTO_TEST_CASE(
  HYDRO_LEVEL__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->hydroLevels.set(area_1->index, 9, 9);
    my_rule->hydroLevels.set(area_3->index, 18, 7);
    my_rule->hydroLevels.set(area_1->index, 5, 8);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("hl,area 1,5 = 8");
    referenceFile.append("hl,area 1,9 = 9");
    referenceFile.append("hl,area 3,18 = 7");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// ======================
// Tests on Links NTC
// ======================
BOOST_AUTO_TEST_CASE(
  LINKS_NTC__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->linksNTC[area_1->index].setDataForLink(link_12, 5, 13);
    my_rule->linksNTC[area_1->index].setDataForLink(link_13, 19, 8);
    my_rule->linksNTC[area_2->index].setDataForLink(link_23, 2, 4);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("ntc,area 1,area 2,5 = 13");
    referenceFile.append("ntc,area 1,area 3,19 = 8");
    referenceFile.append("ntc,area 2,area 3,2 = 4");

    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
}

// ================================
// Tests on All assets together
// ================================
BOOST_AUTO_TEST_CASE(
  ALL_TOGETHER__TS_number_for_many_areas_and_years__generated_and_ref_sc_buider_files_are_identical)
{
    my_rule->load.set(area_2->index, 11, 6);
    my_rule->load.set(area_3->index, 7, 2);
    my_rule->solar.set(area_1->index, 9, 9);
    my_rule->solar.set(area_3->index, 15, 8);
    my_rule->hydro.set(area_3->index, 18, 7);
    my_rule->wind.set(area_3->index, 19, 17);
    my_rule->thermal[area_3->index].set(thCluster_31.get(), 5, 13);
    my_rule->thermal[area_1->index].set(thCluster_11.get(), 19, 8);
    my_rule->renewable[area_3->index].set(rnCluster_32.get(), 5, 13);
    my_rule->linksNTC[area_1->index].setDataForLink(link_13, 19, 8);
    my_rule->linksNTC[area_2->index].setDataForLink(link_23, 2, 4);
    my_rule->hydroLevels.set(area_1->index, 5, 8);

    saveScenarioBuilder();

    // Build reference scenario builder file
    referenceFile.append("[my rule name]");
    referenceFile.append("l,area 2,11 = 6");
    referenceFile.append("l,area 3,7 = 2");
    referenceFile.append("s,area 1,9 = 9");
    referenceFile.append("s,area 3,15 = 8");
    referenceFile.append("h,area 3,18 = 7");
    referenceFile.append("w,area 3,19 = 17");
    referenceFile.append("t,area 1,19,th-cluster-11 = 8");
    referenceFile.append("ntc,area 1,area 3,19 = 8");
    referenceFile.append("ntc,area 2,area 3,2 = 4");
    referenceFile.append("t,area 3,5,th-cluster-31 = 13");
    referenceFile.append("r,area 3,5,rn-cluster-32 = 13");
    referenceFile.append("hl,area 1,5 = 8");
    referenceFile.write();

    BOOST_CHECK(files_identical(path_to_generated_file, referenceFile.path()));
    /*

    */
}

BOOST_AUTO_TEST_SUITE_END()
