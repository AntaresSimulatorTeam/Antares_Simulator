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







# include "h2o_m_donnees_annuelles.h"
# include "h2o_m_fonctions.h"



void H2O_M_ConstruireLesContraintes( DONNEES_ANNUELLES * DonneesAnnuelles )
{
int NombreDeContraintes; int il; int Pdt; int NbPdt;
double * CoefficientsDeLaMatriceDesContraintes; int * IndicesColonnes; int * IndicesDebutDeLigne;
int * NombreDeTermesDesLignes; char * Sens; double ChgmtSens;
int * NumeroDeVariableVolume; int * NumeroDeVariableTurbine; int * NumeroDeVariableDepassementVolumeMax;
int * NumeroDeVariableDepassementVolumeMin; int * NumeroDeVariableDEcartPositifAuTurbineCible;
int * NumeroDeVariableDEcartNegatifAuTurbineCible; int NumeroDeLaVariableXi;

PROBLEME_HYDRAULIQUE * ProblemeHydraulique;
CORRESPONDANCE_DES_VARIABLES *      CorrespondanceDesVariables;
PROBLEME_LINEAIRE_PARTIE_FIXE *     ProblemeLineairePartieFixe;
PROBLEME_LINEAIRE_PARTIE_VARIABLE * ProblemeLineairePartieVariable;

ChgmtSens = -1.0;

NbPdt = DonneesAnnuelles->NombreDePasDeTemps;

ProblemeHydraulique = DonneesAnnuelles->ProblemeHydraulique;
CorrespondanceDesVariables     = ProblemeHydraulique->CorrespondanceDesVariables;
ProblemeLineairePartieFixe     = ProblemeHydraulique->ProblemeLineairePartieFixe;
ProblemeLineairePartieVariable = ProblemeHydraulique->ProblemeLineairePartieVariable;

CoefficientsDeLaMatriceDesContraintes = ProblemeLineairePartieFixe->CoefficientsDeLaMatriceDesContraintes;
IndicesColonnes = ProblemeLineairePartieFixe->IndicesColonnes;
IndicesDebutDeLigne = ProblemeLineairePartieFixe->IndicesDebutDeLigne;
Sens = ProblemeLineairePartieFixe->Sens;
NombreDeTermesDesLignes = ProblemeLineairePartieFixe->NombreDeTermesDesLignes;

NumeroDeVariableVolume = CorrespondanceDesVariables->NumeroDeVariableVolume;
NumeroDeVariableTurbine = CorrespondanceDesVariables->NumeroDeVariableTurbine;
NumeroDeVariableDepassementVolumeMax = CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMax;
NumeroDeVariableDepassementVolumeMin = CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMin;
NumeroDeVariableDEcartPositifAuTurbineCible = CorrespondanceDesVariables->NumeroDeVariableDEcartPositifAuTurbineCible;
NumeroDeVariableDEcartNegatifAuTurbineCible = CorrespondanceDesVariables->NumeroDeVariableDEcartNegatifAuTurbineCible;
NumeroDeLaVariableXi = CorrespondanceDesVariables->NumeroDeLaVariableXi;

NombreDeContraintes = 0;
il = 0;






for ( Pdt = 1 ; Pdt < NbPdt ; Pdt++ ) {
  IndicesDebutDeLigne[NombreDeContraintes] = il;
	
	CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
	IndicesColonnes[il] = NumeroDeVariableVolume[Pdt];
	il++;
	
	CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
	IndicesColonnes[il] = NumeroDeVariableVolume[Pdt-1];
	il++;
	
	CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
	IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt-1];	
	il++;

  Sens[NombreDeContraintes] = '=';
  NombreDeTermesDesLignes[NombreDeContraintes] = 3;
  NombreDeContraintes++;
}



IndicesDebutDeLigne[NombreDeContraintes] = il;

CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
IndicesColonnes[il] = NumeroDeVariableVolume[NbPdt-1];
il++;

CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
IndicesColonnes[il] = NumeroDeVariableTurbine[NbPdt-1];
il++;

Sens[NombreDeContraintes] = '=';
NombreDeTermesDesLignes[NombreDeContraintes] = 2;
NombreDeContraintes++;


for ( Pdt = 1 ; Pdt < NbPdt ; Pdt++ ) {
	
	IndicesDebutDeLigne[NombreDeContraintes] = il;
	
	CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
	IndicesColonnes[il] = NumeroDeVariableVolume[Pdt];
	il++;

	ProblemeLineairePartieFixe->CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
	ProblemeLineairePartieFixe->IndicesColonnes[il] = NumeroDeVariableDepassementVolumeMax[Pdt];
	il++;
	
	Sens[NombreDeContraintes] = '<';
	NombreDeTermesDesLignes[NombreDeContraintes] = 2;
	NombreDeContraintes++;
	
	
	IndicesDebutDeLigne[NombreDeContraintes] = il;
	
	CoefficientsDeLaMatriceDesContraintes[il] = 1.0 * ChgmtSens; 
	IndicesColonnes[il] = NumeroDeVariableVolume[Pdt];	
	il++;

	CoefficientsDeLaMatriceDesContraintes[il] = 1.0 * ChgmtSens; 
	IndicesColonnes[il] = NumeroDeVariableDepassementVolumeMin[Pdt];
	il++;
	
	Sens[NombreDeContraintes] = '<';
	NombreDeTermesDesLignes[NombreDeContraintes] = 2;
	NombreDeContraintes++;
}




for (Pdt = 1; Pdt < NbPdt; Pdt++)
{
	IndicesDebutDeLigne[NombreDeContraintes] = il;

	CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
	IndicesColonnes[il] = NumeroDeVariableDepassementVolumeMin[Pdt];
	il++;

	CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
	IndicesColonnes[il] = CorrespondanceDesVariables->NumeroDeLaVariableViolMaxVolumeMin;
	il++;

	Sens[NombreDeContraintes] = '<';
	NombreDeTermesDesLignes[NombreDeContraintes] = 2;
	NombreDeContraintes++;
}







for ( Pdt = 0 ; Pdt < NbPdt ; Pdt++ ) {
  
  IndicesDebutDeLigne[NombreDeContraintes] = il;

  CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
  IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
  il++;			
	
  CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
  IndicesColonnes[il] = NumeroDeVariableDEcartPositifAuTurbineCible[Pdt];
  il++;		
		
  CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
  IndicesColonnes[il] = NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt];
  il++;			

  Sens[NombreDeContraintes] = '=';
  NombreDeTermesDesLignes[NombreDeContraintes] = 3;
  NombreDeContraintes++;	

  	
  IndicesDebutDeLigne[NombreDeContraintes] = il;   

  CoefficientsDeLaMatriceDesContraintes[il] = 1.0 * ChgmtSens; 
  IndicesColonnes[il] = NumeroDeLaVariableXi;	
  il++;				
	
  CoefficientsDeLaMatriceDesContraintes[il] = -1.0 * ChgmtSens; 
  IndicesColonnes[il] = NumeroDeVariableDEcartPositifAuTurbineCible[Pdt];
  il++;		

  CoefficientsDeLaMatriceDesContraintes[il] = -1.0 * ChgmtSens; 
  IndicesColonnes[il] = NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt];
  il++;			

  Sens[NombreDeContraintes] = '<' ;
  NombreDeTermesDesLignes[NombreDeContraintes] = 3;
  NombreDeContraintes++;		
}

ProblemeLineairePartieFixe->NombreDeContraintes = NombreDeContraintes;

return;
}
