project(hydro)

set(SRC_EXT_SOLVER_H2O_DAILY
	daily/h2o_j_construire_les_contraintes.c
	daily/h2o_j_construire_les_variables.c
	daily/h2o_j_donnees_mensuelles.h
	daily/h2o_j_donnees_optimisation.h
	daily/h2o_j_fonctions.h
	daily/h2o_j_free.c
	daily/h2o_j_initialiser_les_bornes_des_variables.cpp
	daily/h2o_j_initialiser_les_second_membre.c
	daily/h2o_j_instanciation.c
	daily/h2o_j_optimiser_un_mois.c
	daily/h2o_j_resoudre_le_probleme_lineaire.c
	daily/h2o_j_lisser_les_sur_turbines.c
	daily/h2o_j_sys.h
    daily/h2o_j_ajouter_bruit_au_cout.cpp
)

set(SRC_EXT_SOLVER_H2O_MONTHLY
	monthly/h2o_m_construire_les_contraintes.c
	monthly/h2o_m_construire_les_variables.c
	monthly/h2o_m_donnees_annuelles.h
	monthly/h2o_m_donnees_optimisation.h
	monthly/h2o_m_fonctions.h
	monthly/h2o_m_free.c
	monthly/h2o_m_initialiser_les_bornes_des_variables.c
	monthly/h2o_m_initialiser_les_second_membre.c
	monthly/h2o_m_instanciation.c
	monthly/h2o_m_optimiser_une_annee.c
    monthly/h2o_m_ajouter_bruit.cpp
	monthly/h2o_m_resoudre_le_probleme_lineaire.c
	monthly/h2o_m_sys.h
)

set(SRC_EXT_SOLVER_H2O2_DAILY
	daily2/h2o2_j_construire_les_contraintes.cpp
	daily2/h2o2_j_construire_les_variables.cpp
	daily2/h2o2_j_donnees_mensuelles.h
	daily2/h2o2_j_donnees_optimisation.h
	daily2/h2o2_j_fonctions.h
	daily2/h2o2_j_free.cpp
	daily2/h2o2_j_initialiser_les_bornes_des_variables.cpp
	daily2/h2o2_j_initialiser_les_second_membre.cpp
	daily2/h2o2_j_instanciation.cpp
	daily2/h2o2_j_optimiser_un_mois.cpp
	daily2/h2o2_j_resoudre_le_probleme_lineaire.cpp
	daily2/h2o2_j_optim_costs.cpp
	daily2/h2o2_j_apply_costs.cpp
)

set(SRC_MANAGEMENT
	management.h
	management/management.h
	management/management.cpp
	management/monthly.cpp
	management/daily.cpp
)



add_library(libantares-solver-hydro
	${SRC_EXT_SOLVER_H2O_DAILY}
	${SRC_EXT_SOLVER_H2O_MONTHLY}
	${SRC_EXT_SOLVER_H2O2_DAILY}
	${SRC_MANAGEMENT} )

target_link_libraries(libantares-solver-hydro
		PRIVATE
		libmodel_antares
		libantares-solver-variable
		Antares::study
		Antares::mersenne
		PUBLIC sirius_solver)

target_include_directories(libantares-solver-hydro
		PRIVATE
		${CMAKE_SOURCE_DIR}/solver)


