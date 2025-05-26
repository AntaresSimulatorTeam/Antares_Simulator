/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE sc_builder_get_set

/* The goal of this file is to test the setTSnumber and get method for the following classes
- loadTSNumberData
- windTSNumberData
- solarTSNumberData
- hydroTSNumberData
- thermalTSNumberData
- renewableTSNumberData
- ntcTSNumberData
- BindingConstraintsTSNumberData

There are two test suites
- sc_nominal tests the nominal cases, when all year indices are within bounds (0 <= index
<study->parameters.nbYears)
- sc_too_many_years tests the error cases, when some indices are out of bounds (index >=
study->parameters.nbYears). In this case, calls to setTSnumber should have no effect.
*/

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/study/scenario-builder/rules.h>
#include <antares/study/study.h>

using namespace Antares::Data;
using namespace Antares::Data::ScenarioBuilder;

// We need a distinct TS number for each year
// in order for tests to be more specific
uint dummyTSNumber(uint y)
{
    return y * y * y + 1;
}

template<class TSData>
class IntegerIndex
{
public:
    IntegerIndex():
        study(std::make_unique<Study>()),
        area(study->areaAdd("area 1"))
    {
    }

    void setNumberOfYears(uint nbYears)
    {
        study->parameters.nbYears = nbYears;
    }

    void initializeTSNumbers(uint nbYears)
    {
        tsdata.reset(*study);
        for (unsigned int year = 0; year < nbYears; ++year)
        {
            tsdata.setTSnumber(area->index, year, dummyTSNumber(year));
        }
    }

    void check() const
    {
        for (unsigned int year = 0; year < study->parameters.nbYears; ++year)
        {
            BOOST_CHECK_EQUAL(tsdata.get_value(year, area->index), dummyTSNumber(year));
        }
    }

private:
    TSData tsdata;
    std::unique_ptr<Study> study;
    Area* area;
};

template<class TSData, class ObjectT>
class StructureIndex
{
public:
    virtual ObjectT getObject() const = 0;
    virtual void attachArea() = 0;

    explicit StructureIndex():
        study(std::make_unique<Study>()),
        area1(study->areaAdd("area 1")),
        area2(study->areaAdd("area 2")),
        link(AreaAddLinkBetweenAreas(area1, area2, false)),
        thcluster(std::make_shared<ThermalCluster>(area1)),
        rencluster(std::make_shared<RenewableCluster>(area1)),
        bc(study->bindingConstraints.add("my-bc"))
    {
        area1->thermal.list.addToCompleteList(thcluster);
        area1->renewable.list.addToCompleteList(rencluster);

        bc->group("my-group");
    }

    void setNumberOfYears(uint nbYears)
    {
        study->parameters.nbYears = nbYears;
    }

    // virtual function calls not allowed in constructors (UB)
    void initializeTSNumbers(uint nbYears)
    {
        attachArea();
        tsdata.reset(*study);
        for (unsigned int year = 0; year < nbYears; ++year)
        {
            tsdata.setTSnumber(getObject(), year, dummyTSNumber(year));
        }
    }

    void check() const
    {
        for (unsigned int year = 0; year < study->parameters.nbYears; ++year)
        {
            BOOST_CHECK_EQUAL(tsdata.get(getObject(), year), dummyTSNumber(year));
        }
    }

protected:
    std::unique_ptr<Study> study;
    TSData tsdata;
    Area *area1, *area2;
    AreaLink* link;
    std::shared_ptr<ThermalCluster> thcluster;
    std::shared_ptr<RenewableCluster> rencluster;
    std::shared_ptr<BindingConstraint> bc;
};

namespace Fixture
{
// BOOST_FIXTURE_TEST_CASE doesn't support templates
using Load = IntegerIndex<loadTSNumberData>;
using Wind = IntegerIndex<windTSNumberData>;
using Solar = IntegerIndex<solarTSNumberData>;
using Hydro = IntegerIndex<hydroTSNumberData>;

struct Thermal: public StructureIndex<thermalTSNumberData, const ThermalCluster*>
{
    const ThermalCluster* getObject() const override
    {
        return thcluster.get();
    }

    void attachArea() override
    {
        tsdata.attachArea(area1);
    }
};

struct Renewable: public StructureIndex<renewableTSNumberData, const RenewableCluster*>
{
    const RenewableCluster* getObject() const override
    {
        return rencluster.get();
    }

    void attachArea() override
    {
        tsdata.attachArea(area1);
    }
};

struct Link: public StructureIndex<ntcTSNumberData, const AreaLink*>
{
    const AreaLink* getObject() const override
    {
        return link;
    }

    void attachArea() override
    {
        tsdata.attachArea(area1);
    }
};

struct BindingConstraint: public StructureIndex<BindingConstraintsTSNumberData, std::string>
{
    BindingConstraint()
    {
        BOOST_REQUIRE(study->bindingConstraintsGroups.buildFrom(study->bindingConstraints));
    }

    std::string getObject() const override
    {
        return bc->group();
    }

    void attachArea() override
    {
        // No area is attached to a binding constraint
    }
};

} // namespace Fixture

BOOST_AUTO_TEST_SUITE(sc_nominal)

BOOST_FIXTURE_TEST_CASE(load, Fixture::Load)

{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}

BOOST_FIXTURE_TEST_CASE(wind, Fixture::Wind)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}

BOOST_FIXTURE_TEST_CASE(solar, Fixture::Solar)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}

BOOST_FIXTURE_TEST_CASE(hydro, Fixture::Hydro)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}

BOOST_FIXTURE_TEST_CASE(thermal, Fixture::Thermal)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}

BOOST_FIXTURE_TEST_CASE(renewable, Fixture::Renewable)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}

BOOST_FIXTURE_TEST_CASE(link, Fixture::Link)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);
    check();
}

BOOST_FIXTURE_TEST_CASE(binding_constraint, Fixture::BindingConstraint)
{
    setNumberOfYears(5);
    initializeTSNumbers(5);

    check();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sc_too_many_years)

BOOST_FIXTURE_TEST_CASE(load, Fixture::Load)

{
    setNumberOfYears(5);
    initializeTSNumbers(10);

    check();
}

BOOST_FIXTURE_TEST_CASE(wind, Fixture::Wind)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);

    check();
}

BOOST_FIXTURE_TEST_CASE(solar, Fixture::Solar)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);

    check();
}

BOOST_FIXTURE_TEST_CASE(hydro, Fixture::Hydro)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);

    check();
}

BOOST_FIXTURE_TEST_CASE(thermal, Fixture::Thermal)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);

    check();
}

BOOST_FIXTURE_TEST_CASE(renewable, Fixture::Renewable)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);
    check();
}

BOOST_FIXTURE_TEST_CASE(link, Fixture::Link)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);
    check();
}

// This test is disabled for now because is causes a crash, wait for fix to be published
BOOST_FIXTURE_TEST_CASE(binding_constraint, Fixture::BindingConstraint)
{
    setNumberOfYears(5);
    initializeTSNumbers(10);

    check();
}
BOOST_AUTO_TEST_SUITE_END()
