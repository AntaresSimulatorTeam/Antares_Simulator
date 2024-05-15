

#define BOOST_TEST_MODULE test inifile IO
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include "antares/inifile/inifile.h"

struct ReadFromStreamFixture
{
	std::string ini_content;
	Antares::IniFile my_inifile;
	Antares::IniFile::Section* section = nullptr;
};

BOOST_AUTO_TEST_SUITE(populating_inicontent_from_stream)

BOOST_FIXTURE_TEST_CASE(one_section_with_one_property, ReadFromStreamFixture)
{
	ini_content += "[some section]\n";
	ini_content += "some key = some value";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));	// Reading the stream causes no trouble

	section = my_inifile.find("some section");
	BOOST_CHECK(section);	// The IniFile obj contains the section we supplied
	BOOST_CHECK(section->find("some key"));
}

BOOST_FIXTURE_TEST_CASE(nothing_in_the_stream___no_section_created, ReadFromStreamFixture)
{
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));
	BOOST_CHECK(my_inifile.empty());
}

BOOST_FIXTURE_TEST_CASE(properties_without_parent_section___ini_content_remains_empty, ReadFromStreamFixture)
{
	ini_content += "key-1 = value-1\n";
	ini_content += "key-2 = value-2";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));
	BOOST_CHECK(my_inifile.empty());
}

BOOST_FIXTURE_TEST_CASE(a_section_has_no_property___the_section_is_created_and_remains_empty, ReadFromStreamFixture)
{
	ini_content += "[some section]";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));
	section = my_inifile.find("some section");
	BOOST_CHECK(section->empty());
}

BOOST_FIXTURE_TEST_CASE(a_property_has_2_equal_signs___reading_stream_fails, ReadFromStreamFixture)
{
	ini_content += "[some section]\n";
	ini_content += "key-1 = value-1 key-2 = value-2";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(! my_inifile.readStream(input_stream));
}

BOOST_FIXTURE_TEST_CASE(two_properties_commented___they_are_not_created, ReadFromStreamFixture)
{
	ini_content += "[my section]\n";
	ini_content += "key-1 = value-1\n";
	ini_content += "# key-2 = value-2";
	ini_content += "; key-3 = value-3";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));
	section = my_inifile.find("my section");
	BOOST_CHECK(section->find("key-1"));
	BOOST_CHECK(section->size() == 1);
}

BOOST_FIXTURE_TEST_CASE(blank_line_does_not_matter, ReadFromStreamFixture)
{
	ini_content += "[my section]\n";
	ini_content += "	\n";
	ini_content += "key-1 = value-1\n";
	ini_content += "key-2 = value-2";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(my_inifile.readStream(input_stream));
	section = my_inifile.find("my section");
	BOOST_CHECK(section->size() == 2);
	BOOST_CHECK(section->find("key-1") && section->find("key-2"));
}

BOOST_FIXTURE_TEST_CASE(we_have_a_line_with_unknown_format___reading_stream_fails, ReadFromStreamFixture)
{
	ini_content += "[my section]\n";
	ini_content += "Some strange line\n";
	ini_content += "key-1 = value-1\n";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(! my_inifile.readStream(input_stream));
}

BOOST_FIXTURE_TEST_CASE(ill_formed_section_because_brace_missing___reading_stream_fails, ReadFromStreamFixture)
{
	ini_content += "[my section\n";
	ini_content += "some key = some value\n";
	std::istringstream input_stream(ini_content);

	BOOST_CHECK(! my_inifile.readStream(input_stream));
}

BOOST_AUTO_TEST_SUITE_END()


struct SavingToStreamFixture
{
	Antares::IniFile my_inifile;
	Antares::IniFile::Section* section = nullptr;
	std::ostringstream output_stream;
	uint64_t written = 0;
	std::string expected_stream_content;
};

BOOST_AUTO_TEST_SUITE(saving_inicontent_to_stream)

BOOST_FIXTURE_TEST_CASE(one_section_one_property, SavingToStreamFixture)
{
	section = my_inifile.addSection("my section");
	section->add("some key", "some value");

	my_inifile.saveToStream(output_stream, written);

	expected_stream_content += "[my section]\n";
	expected_stream_content += "some key = some value\n\n";
	BOOST_CHECK_EQUAL(output_stream.str(), expected_stream_content);
}

BOOST_AUTO_TEST_SUITE_END()