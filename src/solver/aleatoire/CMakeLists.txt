set(SRC_ALEATOIRE
		alea_fonctions.h
		alea_sys.h
		alea_tirage_au_sort_chroniques.cpp)
source_group("aleatoire" FILES ${SRC_ALEATOIRE})


add_library(antares-solver-aleatoire ${SRC_ALEATOIRE})

target_include_directories(antares-solver-aleatoire
		PRIVATE
		${CMAKE_SOURCE_DIR}/solver)

target_link_libraries(antares-solver-aleatoire PRIVATE antares-core)
