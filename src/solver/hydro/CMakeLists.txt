project(hydro)

set(SRC_EXT_SOLVER_H2O_DAILY
	daily/h2o_j_construire_les_contraintes.cpp
	daily/h2o_j_construire_les_variables.cpp
	daily/h2o_j_donnees_mensuelles.h
	daily/h2o_j_donnees_optimisation.h
	daily/h2o_j_fonctions.h
	daily/h2o_j_free.cpp
	daily/h2o_j_initialiser_les_bornes_des_variables.cpp
	daily/h2o_j_initialiser_les_second_membre.cpp
	daily/h2o_j_instanciation.cpp
	daily/h2o_j_optimiser_un_mois.cpp
	daily/h2o_j_resoudre_le_probleme_lineaire.cpp
	daily/h2o_j_lisser_les_sur_turbines.cpp
	daily/h2o_j_sys.h
    daily/h2o_j_ajouter_bruit_au_cout.cpp
)

set(SRC_EXT_SOLVER_H2O_MONTHLY
	monthly/h2o_m_construire_les_contraintes.cpp
	monthly/h2o_m_construire_les_variables.cpp
	monthly/h2o_m_donnees_annuelles.h
	monthly/h2o_m_donnees_optimisation.h
	monthly/h2o_m_fonctions.h
	monthly/h2o_m_free.cpp
	monthly/h2o_m_initialiser_les_bornes_des_variables.cpp
	monthly/h2o_m_initialiser_les_second_membre.cpp
	monthly/h2o_m_instanciation.cpp
	monthly/h2o_m_optimiser_une_annee.cpp
    monthly/h2o_m_ajouter_bruit.cpp
	monthly/h2o_m_resoudre_le_probleme_lineaire.cpp
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



add_library(antares-solver-hydro
	${SRC_EXT_SOLVER_H2O_DAILY}
	${SRC_EXT_SOLVER_H2O_MONTHLY}
	${SRC_EXT_SOLVER_H2O2_DAILY}
	${SRC_MANAGEMENT} )

target_link_libraries(antares-solver-hydro
		PRIVATE
		model_antares
		antares-solver-variable
		Antares::study
		Antares::mersenne
		PUBLIC sirius_solver)

target_include_directories(antares-solver-hydro
		PRIVATE
		${CMAKE_SOURCE_DIR}/solver)


