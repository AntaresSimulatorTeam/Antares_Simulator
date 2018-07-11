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
#include "simulation.h"

#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"
#include <antares/timeelapsed.h>




void SIM_AlgorithmeDeFlot()
{
	double AugmentationDuFlot ; long   TestDeConvergence  ;
	long   NoeudDestination   ; long   NoeudOrigine       ;
	long       i              ; long     j                ;
	long       k              ;
	double TailleDeBloc       ; long   Relaxation         ;
	PROBLEME_HORAIRE_ADEQUATION *pt;
	auto& study = *Antares::Data::Study::Current::Get();
	const unsigned int areaCount = study.areas.size();

	pt             = &ProblemeHoraireAdequation;

	memset(pt->FluxDeSecours, 0 ,pt->NombreDElementsChainage * sizeof(double));
	memset(pt->NTCDeReference,0 ,pt->NombreDElementsChainage * sizeof(double));
	memset(pt->X             ,0 ,(areaCount + 2)          * sizeof(long));
	memset(pt->S             ,0 ,(areaCount + 2)          * sizeof(double));
	memset(pt->A             ,0 ,(areaCount + 2)          * sizeof(long));
	memset(pt->B             ,0 ,(areaCount + 2)          * sizeof(long));

	
	TailleDeBloc = study.parameters.adequacyBlockSize;

	
	
	
	
	for (i = pt->PointeurSurIndices[0] ; i < pt->PointeurSurIndices[1] ; i++)
	{
		pt->ValeursEffectivesNTC[i] = 0.0;
		if(pt->ResultatsParPays[pt->IndicesDesVoisins[i]-1]->Offre < 0.)
		{
			pt->ValeursEffectivesNTC[i] = - pt->ResultatsParPays[pt->IndicesDesVoisins[i]-1]->Offre;
		}
		pt->ValeursEffectivesNTC[pt->PointeurSurIndices[pt->IndicesDesVoisins[i]]+pt->RangDansVoisinage[i]]=0.0;
	}
	for (i = pt->PointeurSurIndices[areaCount+1] ; i < pt->PointeurSurIndices[areaCount+2] ; i++)
	{
		pt->ValeursEffectivesNTC[i] = 0.0;
		pt->ValeursEffectivesNTC[pt->PointeurSurIndices[pt->IndicesDesVoisins[i]]+pt->RangDansVoisinage[i]]=0.0;
		if(pt->ResultatsParPays[pt->IndicesDesVoisins[i]-1]->Offre > 0.)
		{
			pt->ValeursEffectivesNTC[pt->PointeurSurIndices[pt->IndicesDesVoisins[i]]+pt->RangDansVoisinage[i]] = pt->ResultatsParPays[pt->IndicesDesVoisins[i]-1]->Offre;
		}
	}
	
	for (i = pt->PointeurSurIndices[0] ; i < pt->PointeurSurIndices[1] ; i++)
	{
		pt->NTCDeReference[i]=pt->ValeursEffectivesNTC[i];
		pt->ValeursEffectivesNTC[i]=0.0;
	}
	for (i = pt->PointeurSurIndices[areaCount+1] ; i < pt->PointeurSurIndices[areaCount+2] ; i++)
	{
		pt->NTCDeReference[i]=pt->ValeursEffectivesNTC[i];
		pt->ValeursEffectivesNTC[i]=0.0;
	}
	
LOOP0 : ;
		Relaxation = 1;
		
		for (i = pt->PointeurSurIndices[0] ; i < pt->PointeurSurIndices[1] ; i++)
		{
			pt->ValeursEffectivesNTC[i]  =  (pt->NTCDeReference[i] < pt->ValeursEffectivesNTC[i]+TailleDeBloc ? pt->NTCDeReference[i] : pt->ValeursEffectivesNTC[i]+TailleDeBloc);
			if (pt->ValeursEffectivesNTC[i] != pt->NTCDeReference[i]) {Relaxation = 0;}
		}
		for (i = pt->PointeurSurIndices[areaCount + 1] ; i < pt->PointeurSurIndices[areaCount + 2] ; i++)
		{
			pt->ValeursEffectivesNTC[i]  =  (pt->NTCDeReference[i] < pt->ValeursEffectivesNTC[i]+TailleDeBloc ? pt->NTCDeReference[i]: pt->ValeursEffectivesNTC[i]+TailleDeBloc);
			if(pt->ValeursEffectivesNTC[i] != pt->NTCDeReference[i]) {Relaxation = 0;}
		}
		
LOOP1 : ;
		pt->X[0]=1;
		pt->S[0]=999999.;
		for (i = 1 ; i < (long)areaCount + 2 ; i++)
		{
			pt->X[i]=0;
		}
LOOP2 : ;
		TestDeConvergence = 0;
		for (i = 0 ; i < (long)areaCount + 1 ; i++)
		{
			if (pt->X[i])
			{
				for (j = pt->PointeurSurIndices[i] ; j < pt->PointeurSurIndices[i+1] ; j++)
				{
					k = pt->IndicesDesVoisins[j];
					if((pt->X[k] == 0) && (pt->FluxDeSecours[j] < pt->ValeursEffectivesNTC[j]))
					{
						TestDeConvergence=1;
						pt->S[k] = ((pt->S[i]< pt->ValeursEffectivesNTC[j]-pt->FluxDeSecours[j] )? pt->S[i]: pt->ValeursEffectivesNTC[j]-pt->FluxDeSecours[j]);
						pt->A[k] = i;
						pt->B[k] = j;
						pt->X[k] = 1;
						if (k == ((long)areaCount + 1))
						{
							AugmentationDuFlot = pt->S[k];
							NoeudOrigine       = areaCount + 1;
							NoeudDestination   = pt->A[areaCount + 1];
LOOP3:;
	  pt->FluxDeSecours[pt->B[NoeudOrigine]]                                                                                           = pt->FluxDeSecours[pt->B[NoeudOrigine]]+AugmentationDuFlot;
	  pt->FluxDeSecours[pt->PointeurSurIndices[pt->IndicesDesVoisins[pt->B[NoeudOrigine]]]+pt->RangDansVoisinage[pt->B[NoeudOrigine]]] = -pt->FluxDeSecours[pt->B[NoeudOrigine]];
	  if(NoeudDestination)
	  {
		  NoeudOrigine     = NoeudDestination;
		  NoeudDestination = pt->A[NoeudOrigine];
		  goto LOOP3;
	  }
	  goto LOOP1;
						}
					}
				}
			}
		}
		if (TestDeConvergence)
		{
			goto LOOP2 ;
		}
		if (Relaxation == 0)
			goto LOOP0;

		
		
		
		for (i = 0 ; i < (long)areaCount; i++)
		{
			if (pt->ResultatsParPays[i]->Offre == 0) pt->ResultatsParPays[i]->MargeAvecReseau = pt->ResultatsParPays[i]->MargeHorsReseau ; 
			if (pt->ResultatsParPays[i]->Offre > 0)
			{
				
				for(j = pt->PointeurSurIndices[i+1] ; j < pt->PointeurSurIndices[i+2]; j++)
				{
					if(pt->IndicesDesVoisins[j] == (long)areaCount + 1)
						pt->ResultatsParPays[i]->MargeAvecReseau = pt->ResultatsParPays[i]->MargeHorsReseau - pt->FluxDeSecours[j];
				}
			}
			if (pt->ResultatsParPays[i]->Offre < 0)
			{
				
				for(j = pt->PointeurSurIndices[0] ; j < pt->PointeurSurIndices[1] ; j++)
				{
					if(pt->IndicesDesVoisins[j] == (i+1)) pt->ResultatsParPays[i]->MargeAvecReseau = pt->ResultatsParPays[i]->MargeHorsReseau + pt->FluxDeSecours[j];
				}
			}
		}
		
		ProblemeHoraireAdequation = *pt;
		

}


