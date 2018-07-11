

set(SRC_ALEATOIRE
		aleatoire/alea_fonctions.h
		aleatoire/alea_sys.h
		aleatoire/alea_tirage_au_sort_chroniques.cpp)
source_group("aleatoire" FILES ${SRC_ALEATOIRE})


add_library(libantares-solver-aleatoire
		${SRC_ALEATOIRE}  )

add_library(libantares-solver-aleatoire-swap
		${SRC_ALEATOIRE}  )
set_target_properties(libantares-solver-aleatoire-swap PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
