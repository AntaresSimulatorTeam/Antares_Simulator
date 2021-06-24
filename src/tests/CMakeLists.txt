cmake_minimum_required(VERSION 3.10)

project(unit_tests_antares VERSION 1.0)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Make found targets globally available.
if (Boost_FOUND)
	set_target_properties(Boost::unit_test_framework PROPERTIES IMPORTED_GLOBAL TRUE)
endif()


add_subdirectory(src)

# J'ai viré tout le reste, pour ne pas être ennuyé avec les téléchargements !