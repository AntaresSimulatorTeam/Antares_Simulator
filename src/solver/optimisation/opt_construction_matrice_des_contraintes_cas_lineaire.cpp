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
# include <math.h>

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"


#include <antares/study.h>
#include <antares/study/area/scratchpad.h>
#include "../simulation/sim_structure_donnees.h"


void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire( PROBLEME_HEBDO * ProblemeHebdo, uint numSpace )
{
int Interco  ; int Index       ; int Pays   ; int Pdt; int Var ; int Palier;
int NbInterco; int CntCouplante; double Poids; int NombreDeTermes; int PdtDebut;
int NombreDePasDeTempsDUneJournee; int NombreDePasDeTempsPourUneOptimisation;
int Jour; int Semaine; int * NumeroDeJourDuPasDeTemps; int Pdt1; int Var1;
int Var2; int Var3; int Offset; int Zone; int NombreDeZonesDeReserveJMoins1;
int * NumeroDeZoneDeReserveJMoins1; char ContrainteDeReserveJMoins1ParZone;
char Simulation;

CORRESPONDANCES_DES_VARIABLES *        CorrespondanceVarNativesVarOptim;
CORRESPONDANCES_DES_CONTRAINTES *      CorrespondanceCntNativesCntOptim;
PALIERS_THERMIQUES *                   PaliersThermiquesDuPays;
COUTS_DE_TRANSPORT *                   CoutDeTransport;
CONTRAINTES_COUPLANTES *               MatriceDesContraintesCouplantes;
PROBLEME_ANTARES_A_RESOUDRE *          ProblemeAResoudre;

CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES  * CorrespondanceCntNativesCntOptimJournalieres;
CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES * CorrespondanceCntNativesCntOptimHebdomadaires;

double * Pi; int * Colonne; char PiegerDonnees; double X; 


PiegerDonnees = 0;
if ( PiegerDonnees == 1 ) {
  Pays = 5;
  
  auto& study = *Antares::Data::Study::Current::Get();
  for (uint i = 0; i != study.areas.size(); ++i) {
	  if ( i != (uint) Pays ) continue;
	  
	  auto& area = *study.areas[i];
	  
	  auto& scratchpad = *(area.scratchpad[numSpace]);

	  
	  auto& matrixload = scratchpad.ts.load;
	  
	  
	  
	  auto& tsIndex     = *NumeroChroniquesTireesParPays[0][i];
	  
	  auto& load = matrixload[tsIndex.Consommation];
	  for (uint hour = 0; hour != 8760; ++hour) {
		  
		  
			








		}
	  printf("conso: zone: %s\n", area.name.c_str());
    Pdt = 0;
		while ( Pdt < 8760 ) {
			X = -100000;
      for ( Var = 0; Var < 24 ; Var++ ) {
        if ( load[Pdt] > X ) X = load[Pdt];
				Pdt++;
			}
		  printf("%f;",X);
		}		
  }
  
	PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
	printf("\n");
	printf("NBMOYENS\n");
	printf("%d\n", PaliersThermiquesDuPays->NombreDePaliersThermiques);
	for (Palier = 0 ; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Palier++) {
		X = PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique[Palier];	
    printf("NBENTITES\n");
		
    printf("%d\n", (int) (PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier]->PuissanceDisponibleDuPalierThermique[0]/X));
    printf("PUISSANCE_JETON\n"); 
    printf("%f\n",X);
	}
	  
	exit(0);
}


ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
NombreDeZonesDeReserveJMoins1 = ProblemeHebdo->NombreDeZonesDeReserveJMoins1;
NumeroDeZoneDeReserveJMoins1 = ProblemeHebdo->NumeroDeZoneDeReserveJMoins1;
ContrainteDeReserveJMoins1ParZone = ProblemeHebdo->ContrainteDeReserveJMoins1ParZone;

NombreDePasDeTempsDUneJournee         = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
NumeroDeJourDuPasDeTemps              = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

Pi      = ProblemeAResoudre->Pi;
Colonne = ProblemeAResoudre->Colonne;


ProblemeAResoudre->NombreDeContraintes = 0;
ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;












for ( Pdt = 0 ; Pdt < NombreDePasDeTempsPourUneOptimisation ; Pdt++ ) {
	CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
	CorrespondanceCntNativesCntOptim = ProblemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		
		NombreDeTermes = 0;

		
		Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Pays];
		while ( Interco >= 0 ) {
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
			if ( Var >= 0 ) {
				Pi     [NombreDeTermes] = 1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
			Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
		}
		Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Pays];
		while ( Interco >= 0 ) {
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
			if ( Var >= 0 ) {
				Pi     [NombreDeTermes] = -1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
			Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
		}

		
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		for ( Index = 0 ; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Index++ ) {
			Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
			if ( Var >= 0 ) {
				Pi     [NombreDeTermes] = -1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
		}

		
		Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
		if ( Var >= 0 ) {
			Pi     [NombreDeTermes] = -1.0;
			Colonne[NombreDeTermes] = Var;
			NombreDeTermes++;
		}

		
		Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
		if ( Var >= 0 ) {
			Pi     [NombreDeTermes] = -1.0;
			Colonne[NombreDeTermes] = Var;
			NombreDeTermes++;
		}
		Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
		if ( Var >= 0 ) {
			Pi     [NombreDeTermes] = 1.0;
			Colonne[NombreDeTermes] = Var;
			NombreDeTermes++;
		}

		
		CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[Pays] = ProblemeAResoudre->NombreDeContraintes;
		
		OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=' );

		
    

		NombreDeTermes = 0;
		
		
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		for ( Index = 0 ; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Index++ ) {
			Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
			if ( Var >= 0 ) {
				Pi     [NombreDeTermes] = -1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
		}

		
		Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
		if ( Var >= 0 ) {
			
          
      Pi     [NombreDeTermes] = -ProblemeHebdo->DefaillanceNegativeUtiliserHydro[Pays];
			Colonne[NombreDeTermes] = Var;
			NombreDeTermes++;
		}

		
		Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
		if ( Var >= 0 ) {
			Pi     [NombreDeTermes] = 1.0;
			Colonne[NombreDeTermes] = Var;
			NombreDeTermes++;
		}

		
		CorrespondanceCntNativesCntOptim->NumeroDeContraintePourEviterLesChargesFictives[Pays] = ProblemeAResoudre->NombreDeContraintes;		
		
		OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<' );		
		
    





































		
	}

	
	if ( COUT_TRANSPORT == OUI_ANTARES ) {
		for ( Interco = 0 ; Interco < ProblemeHebdo->NombreDInterconnexions ; Interco++ ) {
			CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];
			if ( CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES ) {
				NombreDeTermes = 0;
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
				if ( Var >= 0 ) {
					Pi     [NombreDeTermes] = 1.0;
					Colonne[NombreDeTermes] = Var;
					NombreDeTermes++;
				}
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
				if ( Var >= 0 ) {
					Pi     [NombreDeTermes] = -1.0;
					Colonne[NombreDeTermes] = Var;
					NombreDeTermes++;
				}
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
				if ( Var >= 0 ) {
					Pi     [NombreDeTermes] = 1.0;
					Colonne[NombreDeTermes] = Var;
					NombreDeTermes++;
				}
				
				CorrespondanceCntNativesCntOptim->NumeroDeContrainteDeDissociationDeFlux[Interco] = ProblemeAResoudre->NombreDeContraintes;
				
				OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=' );				
			}
		}
	}

	
	for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
		MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
    if ( MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE ) {
		  NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
		  NombreDeTermes = 0;
		  for ( Index = 0 ; Index < NbInterco ; Index++ ) {
			  Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
			  Poids   = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
			  Offset  = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];
				



				
        							
				if ( Offset >= 0 ) {
          Pdt1 = (Pdt+Offset) % NombreDePasDeTempsPourUneOptimisation;
				}
				else {
          Pdt1 = (Pdt+Offset+ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
				}
		
				
        Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDeLInterconnexion[Interco];
        
			  
			  if ( Var >= 0 ) {
				  Pi     [NombreDeTermes] = Poids;
				  Colonne[NombreDeTermes] = Var;
				  NombreDeTermes++;
			  }
		  }			
		  
		  CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesContraintesCouplantes[CntCouplante] = ProblemeAResoudre->NombreDeContraintes;
		  
		  OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes,
		                                                      MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante );
	  }
	}

  if ( ContrainteDeReserveJMoins1ParZone == OUI_ANTARES ) {
    



		
	for ( Zone = 0 ; Zone < NombreDeZonesDeReserveJMoins1 ; Zone++ ) {
	  
	  NombreDeTermes = 0;
	  for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
      if ( NumeroDeZoneDeReserveJMoins1[Pays] != Zone ) continue;
		  
		  PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		  for ( Index = 0 ; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Index++ ) {
			  Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
			  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
			  if ( Var >= 0 ) {
				  Pi     [NombreDeTermes] = 1.0;
				  Colonne[NombreDeTermes] = Var;
				  NombreDeTermes++;
			  }
		  }
		  
		  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }

		  
		  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceEnReserve[Pays];
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }
		}
		
		CorrespondanceCntNativesCntOptim->NumeroPremiereContrainteDeReserveParZone[Zone] = ProblemeAResoudre->NombreDeContraintes;
		
		OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>' );

	  
	  NombreDeTermes = 0;
	  for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
      if ( NumeroDeZoneDeReserveJMoins1[Pays] != Zone ) continue;
		  
		  PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		  for ( Index = 0 ; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Index++ ) {
			  Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
			  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
			  if ( Var >= 0 ) {
				  Pi     [NombreDeTermes] = -1.0;
				  Colonne[NombreDeTermes] = Var;
				  NombreDeTermes++;
			  }
		  }
		  
		  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = -1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }

		  
		  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceEnReserve[Pays];
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }
		}
		
		CorrespondanceCntNativesCntOptim->NumeroDeuxiemeContrainteDeReserveParZone[Zone] = ProblemeAResoudre->NombreDeContraintes;
		
		OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>' );
  }
	   
	}
}



for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
	MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
  if ( MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_JOURNALIERE ) {
	  NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
    PdtDebut = 0;
	  while ( PdtDebut < NombreDePasDeTempsPourUneOptimisation ) {
      
	    Jour = NumeroDeJourDuPasDeTemps[PdtDebut];
      CorrespondanceCntNativesCntOptimJournalieres = ProblemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[Jour];
 	    NombreDeTermes = 0;
 	    for ( Index = 0 ; Index < NbInterco ; Index++ ) {
		    Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
	 	    Poids   = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
			  Offset  = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];
		    for ( Pdt = PdtDebut ; Pdt < PdtDebut + NombreDePasDeTempsDUneJournee ; Pdt++ ) {
          
	        



					



					
          							
				  if ( Offset >= 0 ) {
            Pdt1 = (Pdt+Offset) % NombreDePasDeTempsPourUneOptimisation;
				  }
				  else {
            Pdt1 = (Pdt+Offset+ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
				  }
					
          Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDeLInterconnexion[Interco];
		      if ( Var >= 0 ) {
			      Pi     [NombreDeTermes] = Poids;
			      Colonne[NombreDeTermes] = Var;
			      NombreDeTermes++;
			    }
		    }
	    }
	    
	    assert(CntCouplante >= 0);
      CorrespondanceCntNativesCntOptimJournalieres->NumeroDeContrainteDesContraintesCouplantes[CntCouplante] = ProblemeAResoudre->NombreDeContraintes;
	    
	    OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes,
	                                                        MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante );
      PdtDebut+= NombreDePasDeTempsDUneJournee;
	  }
  }
}


if ( ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation > ProblemeHebdo->NombreDePasDeTempsDUneJournee ) {
  Semaine = 0;
  CorrespondanceCntNativesCntOptimHebdomadaires = ProblemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires[Semaine];
  for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
	  MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
    if ( MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HEBDOMADAIRE ) {
	    NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
 	    NombreDeTermes = 0;
 	    for ( Index = 0 ; Index < NbInterco ; Index++ ) {
		    Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
	 	    Poids   = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
			  Offset  = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];
		    for ( Pdt = 0 ; Pdt < NombreDePasDeTempsPourUneOptimisation ; Pdt++ ) {
          
          



					



					
          							
				  if ( Offset >= 0 ) {
            Pdt1 = (Pdt+Offset) % NombreDePasDeTempsPourUneOptimisation;
				  }
				  else {
            Pdt1 = (Pdt+Offset+ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
				  }
					
          Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDeLInterconnexion[Interco];
		      if ( Var >= 0 ) {
			      Pi     [NombreDeTermes] = Poids;
			      Colonne[NombreDeTermes] = Var;
			      NombreDeTermes++;
			    }
		    }
	    }
	    
      CorrespondanceCntNativesCntOptimHebdomadaires->NumeroDeContrainteDesContraintesCouplantes[CntCouplante] = ProblemeAResoudre->NombreDeContraintes;
	    
	    OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes,
	                                                        MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante );
    }
  }
}


for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
	if ( ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable == OUI_ANTARES ) {
		NombreDeTermes = 0;
		for ( Pdt = 0 ; Pdt < NombreDePasDeTempsPourUneOptimisation ; Pdt++ ) {
			Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]->NumeroDeVariablesDeLaProdHyd[Pays];
			if ( Var >= 0 ) {
				Pi     [NombreDeTermes] = 1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
		}
		
		ProblemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays] = ProblemeAResoudre->NombreDeContraintes;
		
		OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
	}
	else
		ProblemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays] = -1;
}

if ( ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS ) {
  for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
	  if ( ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable != OUI_ANTARES ) continue;
    
    for ( Pdt = 0 ; Pdt < NombreDePasDeTempsPourUneOptimisation ; Pdt++ ) {
	    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
      NombreDeTermes = 0;
		  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }
		  Pdt1 = Pdt + 1;
		  if ( Pdt1 >= NombreDePasDeTempsPourUneOptimisation ) Pdt1 = 0;
		  Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariablesDeLaProdHyd[Pays];
		  if ( Var1 >= 0 ) {
			  Pi     [NombreDeTermes] = -1.0;
			  Colonne[NombreDeTermes] = Var1;
			  NombreDeTermes++;
		  }
		  Var2 = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaBaisse[Pays];
		  if ( Var2 >= 0 ) {
			  Pi     [NombreDeTermes] = -1.0;
			  Colonne[NombreDeTermes] = Var2;
			  NombreDeTermes++;
		  }
		  Var3 = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaHausse[Pays];
		  if ( Var3 >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var3;
			  NombreDeTermes++;
		  }
		  
		  
		  OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=' );
    }
	}
}
else if ( ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX ) {
  




  for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
	  if ( ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable != OUI_ANTARES ) continue;
    
    for ( Pdt = 0 ; Pdt < NombreDePasDeTempsPourUneOptimisation ; Pdt++ ) {
	    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
      NombreDeTermes = 0;
		  Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }
		  Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]->NumeroDeVariablesVariationHydALaBaisse[Pays];
		  if ( Var1 >= 0 ) {
			  Pi     [NombreDeTermes] = -1.0;
			  Colonne[NombreDeTermes] = Var1;
			  NombreDeTermes++;
		  }
		  
		  
		  
		  OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<' );

      NombreDeTermes = 0;
		  if ( Var >= 0 ) {
			  Pi     [NombreDeTermes] = 1.0;
			  Colonne[NombreDeTermes] = Var;
			  NombreDeTermes++;
		  }
		  Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]->NumeroDeVariablesVariationHydALaHausse[Pays];
		  if ( Var1 >= 0 ) {
			  Pi     [NombreDeTermes] = -1.0;
			  Colonne[NombreDeTermes] = Var1;
			  NombreDeTermes++;
		  }
		  
		  
		  OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>' );
    }
	}
}

if ( ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES ) {
  Simulation = NON_ANTARES;
  OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage( ProblemeHebdo, Simulation );
}

return;
}



