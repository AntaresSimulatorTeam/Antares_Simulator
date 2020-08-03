/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/







# ifdef __CPLUSPLUS
  extern "C"
  {
# endif

# include "spx_definition_arguments.h"
# include "spx_fonctions.h"

# ifdef __CPLUSPLUS
  }
# endif

# include "h2o_m_donnees_annuelles.h"
# include "h2o_m_fonctions.h"



void H2O_M_Free( DONNEES_ANNUELLES * DonneesAnnuelles )
{
int i; PROBLEME_SPX * ProbSpx;

PROBLEME_HYDRAULIQUE * ProblemeHydraulique;

ProblemeHydraulique = DonneesAnnuelles->ProblemeHydraulique;
 
free( (ProblemeHydraulique->CorrespondanceDesVariables)->NumeroDeVariableVolume );
free( (ProblemeHydraulique->CorrespondanceDesVariables)->NumeroDeVariableTurbine );
free( (ProblemeHydraulique->CorrespondanceDesVariables)->NumeroDeVariableDepassementVolumeMax );
free( (ProblemeHydraulique->CorrespondanceDesVariables)->NumeroDeVariableDepassementVolumeMin );
free( (ProblemeHydraulique->CorrespondanceDesVariables)->NumeroDeVariableDEcartPositifAuTurbineCible );
free( (ProblemeHydraulique->CorrespondanceDesVariables)->NumeroDeVariableDEcartNegatifAuTurbineCible );
free( ProblemeHydraulique->CorrespondanceDesVariables );
		
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->CoutLineaire );
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->TypeDeVariable );
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->Sens );
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->IndicesDebutDeLigne );
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->NombreDeTermesDesLignes );
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->CoefficientsDeLaMatriceDesContraintes );
free( (ProblemeHydraulique->ProblemeLineairePartieFixe)->IndicesColonnes );
free( ProblemeHydraulique->ProblemeLineairePartieFixe );
	
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->Xmin );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->Xmax );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->SecondMembre );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->AdresseOuPlacerLaValeurDesVariablesOptimisees );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->X );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->PositionDeLaVariable );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->ComplementDeLaBase );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->CoutsReduits );
free( (ProblemeHydraulique->ProblemeLineairePartieVariable)->CoutsMarginauxDesContraintes );
free( ProblemeHydraulique->ProblemeLineairePartieVariable );

for ( i = 0 ; i < ProblemeHydraulique->NombreDeReservoirs ; i++ ) {
  ProbSpx = (PROBLEME_SPX *) ProblemeHydraulique->ProblemeSpx[i];
  if ( ProbSpx != NULL ) {
	  SPX_LibererProbleme( ProbSpx );
  }
}
				
free( ProblemeHydraulique->ProblemeSpx );
free( ProblemeHydraulique->Probleme );
free( ProblemeHydraulique );
 
free( DonneesAnnuelles->TurbineMax );
free( DonneesAnnuelles->TurbineCible );
free( DonneesAnnuelles->Apport );
free( DonneesAnnuelles->VolumeMin );
free( DonneesAnnuelles->VolumeMax );
free( DonneesAnnuelles->Turbine );
free( DonneesAnnuelles->Volume );
free( DonneesAnnuelles );

return;	
}
