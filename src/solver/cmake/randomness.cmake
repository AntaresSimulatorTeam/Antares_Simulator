

set(SRC_ALEATOIRE
		aleatoire/alea_fonctions.h
		aleatoire/alea_sys.h
		aleatoire/alea_tirage_au_sort_chroniques.cpp)
source_group("aleatoire" FILES ${SRC_ALEATOIRE})


add_library(libantares-solver-aleatoire ${SRC_ALEATOIRE})
target_link_libraries(libantares-solver-aleatoire PRIVATE libantares-core)
