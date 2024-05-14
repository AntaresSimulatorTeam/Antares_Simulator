

#define BOOST_TEST_MODULE test inifile IO
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include "inifile.h"

struct Fixture
{
	std::string ini_content;
	Antares::IniFile my_inifile;
	Antares::IniFile::Section* section = nullptr;
};


BOOST_AUTO_TEST_SUITE(building_inicontent_from_stream)

BOOST_FIXTURE_TEST_CASE(one_section_with_one_property, Fixture)
{
	ini_content += "[some section]\n";
	ini_content += "some key = some value";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));	// Reading the stream causes no trouble

	section = my_inifile.find("some section");
	BOOST_CHECK(section);	// The IniFile obj contains the section we supplied
	BOOST_CHECK(section->find("some key"));
}

BOOST_FIXTURE_TEST_CASE(properties_without_parent_section___ini_content_remains_empty, Fixture)
{
	ini_content += "key-1 = value-1\n";
	ini_content += "key-2 = value-2";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));	// Reading the stream causes no trouble
	BOOST_CHECK(my_inifile.empty());
}

BOOST_FIXTURE_TEST_CASE(a_property_has_2_equal_signs___reading_stream_fails, Fixture)
{
	ini_content += "[some section]\n";
	ini_content += "key-1 = value-1 key-2 = value-2";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(!my_inifile.readStream(input_stream));
}

BOOST_AUTO_TEST_SUITE_END()
