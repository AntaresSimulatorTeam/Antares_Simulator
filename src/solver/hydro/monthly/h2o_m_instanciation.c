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

#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

DONNEES_ANNUELLES* H2O_M_Instanciation(int NombreDeReservoirs)
{
    int i;
    int NbPdt;
    int j;
    int NombreDeVariables;
    int NombreDeContraintes;
    int NombreDeTermesAlloues;
    DONNEES_ANNUELLES* DonneesAnnuelles;
    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;

    CORRESPONDANCE_DES_VARIABLES* CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_PARTIE_FIXE* ProblemeLineairePartieFixe;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE* ProblemeLineairePartieVariable;

    DonneesAnnuelles = (DONNEES_ANNUELLES*)malloc(sizeof(DONNEES_ANNUELLES));
    if (DonneesAnnuelles == NULL)
    {
        return (NULL);
    }
    DonneesAnnuelles->NombreDePasDeTemps = 12;

    NbPdt = DonneesAnnuelles->NombreDePasDeTemps;

    DonneesAnnuelles->ProblemeHydraulique
      = (PROBLEME_HYDRAULIQUE*)malloc(sizeof(PROBLEME_HYDRAULIQUE));
    if (DonneesAnnuelles->ProblemeHydraulique == NULL)
    {
        return (NULL);
    }

    DonneesAnnuelles->TurbineMax = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->TurbineMax == NULL)
    {
        return (NULL);
    }
    DonneesAnnuelles->TurbineMin = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->TurbineMin == NULL)
    {
        return (NULL);
    }    

    DonneesAnnuelles->TurbineCible = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->TurbineCible == NULL)
    {
        return (NULL);
    }

    DonneesAnnuelles->Apport = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->Apport == NULL)
    {
        return (NULL);
    }

    DonneesAnnuelles->VolumeMin = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->VolumeMin == NULL)
    {
        return (NULL);
    }
    DonneesAnnuelles->VolumeMax = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->VolumeMax == NULL)
    {
        return (NULL);
    }

    DonneesAnnuelles->Turbine = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->Turbine == NULL)
    {
        return (NULL);
    }

    DonneesAnnuelles->Volume = (double*)malloc((unsigned int)NbPdt * sizeof(double));
    if (DonneesAnnuelles->Volume == NULL)
    {
        return (NULL);
    }

    ProblemeHydraulique = DonneesAnnuelles->ProblemeHydraulique;

    ProblemeHydraulique->NombreDeReservoirs = NombreDeReservoirs;

    ProblemeHydraulique->CorrespondanceDesVariables
      = (CORRESPONDANCE_DES_VARIABLES*)malloc(sizeof(CORRESPONDANCE_DES_VARIABLES));
    if (ProblemeHydraulique->CorrespondanceDesVariables == NULL)
    {
        return (0);
    }

    ProblemeHydraulique->ProblemeLineairePartieFixe
      = (PROBLEME_LINEAIRE_PARTIE_FIXE*)malloc(sizeof(PROBLEME_LINEAIRE_PARTIE_FIXE));
    if (ProblemeHydraulique->ProblemeLineairePartieFixe == NULL)
    {
        return (0);
    }

    ProblemeHydraulique->ProblemeLineairePartieVariable
      = (PROBLEME_LINEAIRE_PARTIE_VARIABLE*)malloc(sizeof(PROBLEME_LINEAIRE_PARTIE_VARIABLE));
    if (ProblemeHydraulique->ProblemeLineairePartieVariable == NULL)
    {
        return (0);
    }

    ProblemeHydraulique->ProblemeSpx
      = (void**)malloc((unsigned int)NombreDeReservoirs * sizeof(void*));
    if (ProblemeHydraulique->ProblemeSpx == NULL)
    {
        return (0);
    }
    for (i = 0; i < NombreDeReservoirs; i++)
    {
        ProblemeHydraulique->ProblemeSpx[i] = NULL;
    }

    ProblemeHydraulique->Probleme = NULL;

    CorrespondanceDesVariables = ProblemeHydraulique->CorrespondanceDesVariables;
    ProblemeLineairePartieFixe = ProblemeHydraulique->ProblemeLineairePartieFixe;
    ProblemeLineairePartieVariable = ProblemeHydraulique->ProblemeLineairePartieVariable;

    CorrespondanceDesVariables->NumeroDeVariableVolume
      = (int*)malloc((unsigned int)NbPdt * sizeof(int));
    if (CorrespondanceDesVariables->NumeroDeVariableVolume == NULL)
    {
        return (0);
    }

    CorrespondanceDesVariables->NumeroDeVariableTurbine
      = (int*)malloc((unsigned int)NbPdt * sizeof(int));
    if (CorrespondanceDesVariables->NumeroDeVariableTurbine == NULL)
    {
        return (0);
    }

    CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMin
      = (int*)malloc((unsigned int)NbPdt * sizeof(int));
    if (CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMin == NULL)
    {
        return (0);
    }

    CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMax
      = (int*)malloc((unsigned int)NbPdt * sizeof(int));
    if (CorrespondanceDesVariables->NumeroDeVariableDepassementVolumeMax == NULL)
    {
        return (0);
    }

    CorrespondanceDesVariables->NumeroDeVariableDEcartPositifAuTurbineCible
      = (int*)malloc((unsigned int)NbPdt * sizeof(int));
    if (CorrespondanceDesVariables->NumeroDeVariableDEcartPositifAuTurbineCible == NULL)
    {
        return (0);
    }

    CorrespondanceDesVariables->NumeroDeVariableDEcartNegatifAuTurbineCible
      = (int*)malloc((unsigned int)NbPdt * sizeof(int));
    if (CorrespondanceDesVariables->NumeroDeVariableDEcartNegatifAuTurbineCible == NULL)
    {
        return (0);
    }

    NombreDeVariables = 0;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += 1;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += 1;

    ProblemeLineairePartieFixe->NombreDeVariables = NombreDeVariables;
    ProblemeLineairePartieFixe->CoutLineaire
      = (double*)malloc((unsigned int)NombreDeVariables * sizeof(double));
    if (ProblemeLineairePartieFixe->CoutLineaire == NULL)
    {
        return (0);
    }
    ProblemeLineairePartieFixe->CoutLineaireBruite
      = (double*)malloc((unsigned int)NombreDeVariables * sizeof(double));
    if (ProblemeLineairePartieFixe->CoutLineaireBruite == NULL)
    {
        return (0);
    }
    ProblemeLineairePartieFixe->TypeDeVariable
      = (int*)malloc((unsigned int)NombreDeVariables * sizeof(int));
    if (ProblemeLineairePartieFixe->TypeDeVariable == NULL)
    {
        return (0);
    }

    NombreDeContraintes = 0;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += 1;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;

    ProblemeLineairePartieFixe->NombreDeContraintes = NombreDeContraintes;
    ProblemeLineairePartieFixe->Sens
      = (char*)malloc((unsigned int)NombreDeContraintes * sizeof(char));
    if (ProblemeLineairePartieFixe->Sens == NULL)
    {
        return (0);
    }
    ProblemeLineairePartieFixe->IndicesDebutDeLigne
      = (int*)malloc((unsigned int)NombreDeContraintes * sizeof(int));
    if (ProblemeLineairePartieFixe->IndicesDebutDeLigne == NULL)
    {
        return (0);
    }
    ProblemeLineairePartieFixe->NombreDeTermesDesLignes
      = (int*)malloc((unsigned int)NombreDeContraintes * sizeof(int));
    if (ProblemeLineairePartieFixe->NombreDeTermesDesLignes == NULL)
    {
        return (0);
    }

    NombreDeTermesAlloues = 0;
    NombreDeTermesAlloues += 3 * NbPdt;
    NombreDeTermesAlloues += 2;
    NombreDeTermesAlloues += 2 * NbPdt;
    NombreDeTermesAlloues += 2 * NbPdt;
    NombreDeTermesAlloues += 2 * NbPdt;
    NombreDeTermesAlloues += 3 * NbPdt;
    NombreDeTermesAlloues += 3 * NbPdt;

    ProblemeLineairePartieFixe->NombreDeTermesAlloues = NombreDeTermesAlloues;

    ProblemeLineairePartieFixe->CoefficientsDeLaMatriceDesContraintes
      = (double*)malloc((unsigned int)NombreDeTermesAlloues * sizeof(double));
    if (ProblemeLineairePartieFixe->CoefficientsDeLaMatriceDesContraintes == NULL)
    {
        return (0);
    }
    ProblemeLineairePartieFixe->IndicesColonnes
      = (int*)malloc((unsigned int)NombreDeTermesAlloues * sizeof(int));
    if (ProblemeLineairePartieFixe->IndicesColonnes == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->Xmin
      = (double*)malloc((unsigned int)NombreDeVariables * sizeof(double));
    if (ProblemeLineairePartieVariable->Xmin == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->Xmax
      = (double*)malloc((unsigned int)NombreDeVariables * sizeof(double));
    if (ProblemeLineairePartieVariable->Xmax == NULL)
    {
        return (0);
    }
    ProblemeLineairePartieVariable->SecondMembre
      = (double*)malloc((unsigned int)NombreDeContraintes * sizeof(double));
    if (ProblemeLineairePartieVariable->SecondMembre == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees
      = (double**)malloc((unsigned int)NombreDeVariables * sizeof(double*));
    if (ProblemeLineairePartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->X
      = (double*)malloc((unsigned int)NombreDeVariables * sizeof(double));
    if (ProblemeLineairePartieVariable->X == NULL)
    {
        return (0);
    }

    for (j = 0; j < NombreDeVariables; j++)
    {
        ProblemeLineairePartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees[j] = NULL;
    }

    ProblemeLineairePartieVariable->PositionDeLaVariable
      = (int*)malloc((unsigned int)NombreDeVariables * sizeof(int));
    if (ProblemeLineairePartieVariable->PositionDeLaVariable == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->ComplementDeLaBase
      = (int*)malloc((unsigned int)NombreDeContraintes * sizeof(int));
    if (ProblemeLineairePartieVariable->ComplementDeLaBase == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->CoutsReduits
      = (double*)malloc((unsigned int)NombreDeVariables * sizeof(double));
    if (ProblemeLineairePartieVariable->CoutsReduits == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->CoutsMarginauxDesContraintes
      = (double*)malloc((unsigned int)NombreDeContraintes * sizeof(double));
    if (ProblemeLineairePartieVariable->CoutsMarginauxDesContraintes == NULL)
    {
        return (0);
    }

    H2O_M_ConstruireLesVariables(DonneesAnnuelles);

    H2O_M_ConstruireLesContraintes(DonneesAnnuelles);

    return (DonneesAnnuelles);
}
