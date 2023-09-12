set(SRC_ALEATOIRE
		alea_fonctions.h
		alea_sys.h
		alea_tirage_au_sort_chroniques.cpp)
source_group("aleatoire" FILES ${SRC_ALEATOIRE})


add_library(libantares-solver-aleatoire ${SRC_ALEATOIRE})

target_include_directories(libantares-solver-aleatoire
		PRIVATE
		${CMAKE_SOURCE_DIR}/solver)

target_link_libraries(libantares-solver-aleatoire
	PUBLIC
		Antares::study
)
