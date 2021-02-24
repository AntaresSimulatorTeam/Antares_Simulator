
set(SRC_EXT_SOLVER_H2O_DAILY
	hydro/daily/h2o_j_construire_les_contraintes.c
	hydro/daily/h2o_j_construire_les_variables.c
	hydro/daily/h2o_j_donnees_mensuelles.h
	hydro/daily/h2o_j_donnees_optimisation.h
	hydro/daily/h2o_j_fonctions.h
	hydro/daily/h2o_j_free.c
	hydro/daily/h2o_j_initialiser_les_bornes_des_variables.c
	hydro/daily/h2o_j_initialiser_les_second_membre.c
	hydro/daily/h2o_j_instanciation.c
	hydro/daily/h2o_j_optimiser_un_mois.c
	hydro/daily/h2o_j_resoudre_le_probleme_lineaire.c
	hydro/daily/h2o_j_lisser_les_sur_turbines.c
	hydro/daily/h2o_j_sys.h
)

set(SRC_EXT_SOLVER_H2O_MONTHLY
	hydro/monthly/h2o_m_construire_les_contraintes.c
	hydro/monthly/h2o_m_construire_les_variables.c
	hydro/monthly/h2o_m_donnees_annuelles.h
	hydro/monthly/h2o_m_donnees_optimisation.h
	hydro/monthly/h2o_m_fonctions.h
	hydro/monthly/h2o_m_free.c
	hydro/monthly/h2o_m_initialiser_les_bornes_des_variables.c
	hydro/monthly/h2o_m_initialiser_les_second_membre.c
	hydro/monthly/h2o_m_instanciation.c
	hydro/monthly/h2o_m_optimiser_une_annee.c
	hydro/monthly/h2o_m_resoudre_le_probleme_lineaire.c
	hydro/monthly/h2o_m_sys.h
)

set(SRC_EXT_SOLVER_H2O2_DAILY
	hydro/daily2/h2o2_j_construire_les_contraintes.cpp
	hydro/daily2/h2o2_j_construire_les_variables.cpp
	hydro/daily2/h2o2_j_donnees_mensuelles.h
	hydro/daily2/h2o2_j_donnees_optimisation.h
	hydro/daily2/h2o2_j_fonctions.h
	hydro/daily2/h2o2_j_free.cpp
	hydro/daily2/h2o2_j_initialiser_les_bornes_des_variables.cpp
	hydro/daily2/h2o2_j_initialiser_les_second_membre.cpp
	hydro/daily2/h2o2_j_instanciation.cpp
	hydro/daily2/h2o2_j_optimiser_un_mois.cpp
	hydro/daily2/h2o2_j_resoudre_le_probleme_lineaire.cpp
	hydro/daily2/h2o2_j_optim_costs.cpp
	hydro/daily2/h2o2_j_apply_costs.cpp
)

set(SRC_MANAGEMENT
	hydro/management.h
	hydro/management/management.h
	hydro/management/management.cpp
	hydro/management/monthly.cpp
	hydro/management/daily.cpp
)






macro(add_solver_hydro  suffix)

	add_library(libantares-solver-hydro${suffix}
		${SRC_EXT_SOLVER_H2O_DAILY}
		${SRC_EXT_SOLVER_H2O_MONTHLY}
		${SRC_EXT_SOLVER_H2O2_DAILY}
		${SRC_MANAGEMENT} )

	target_link_libraries(libantares-solver-hydro${suffix}
		PRIVATE
		libmodel_antares${suffix} 
		libantares-solver-variable${suffix} PUBLIC sirius_solver)


endmacro()


add_solver_hydro("")

if(BUILD_SWAP)
    
    add_solver_hydro("-swap")
    set_target_properties(libantares-solver-hydro-swap PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")

endif()
