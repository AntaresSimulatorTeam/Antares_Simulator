/*
** Copyright 2007-2023 RTE
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

    DonneesAnnuelles->TurbineMax.assign(NbPdt, 0.);
    DonneesAnnuelles->TurbineMin.assign(NbPdt, 0.);
    DonneesAnnuelles->TurbineCible.assign(NbPdt, 0.);
    DonneesAnnuelles->Turbine.assign(NbPdt, 0.);

    DonneesAnnuelles->Apport.assign(NbPdt, 0.);

    DonneesAnnuelles->Volume.assign(NbPdt, 0.);
    DonneesAnnuelles->VolumeMin.assign(NbPdt, 0.);
    DonneesAnnuelles->VolumeMax.assign(NbPdt, 0.);

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

    ProblemeLineairePartieFixe->CoutLineaire.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieFixe->CoutLineaireBruite.assign(NombreDeVariables, 0.);

    ProblemeLineairePartieFixe->TypeDeVariable.assign(NombreDeVariables, 0);

    NombreDeContraintes = 0;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += 1;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;

    ProblemeLineairePartieFixe->NombreDeContraintes = NombreDeContraintes;
    ProblemeLineairePartieFixe->Sens.assign(NombreDeContraintes, 0);

    ProblemeLineairePartieFixe->IndicesDebutDeLigne.assign(NombreDeContraintes, 0);
    ProblemeLineairePartieFixe->NombreDeTermesDesLignes.assign(NombreDeContraintes, 0);

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
        .assign(NombreDeTermesAlloues, 0.);

    ProblemeLineairePartieFixe->IndicesColonnes.assign(NombreDeTermesAlloues, 0);

    ProblemeLineairePartieVariable->Xmin.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieVariable->Xmax.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieVariable->SecondMembre.assign(NombreDeContraintes, 0.);

    ProblemeLineairePartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees
      = (double**)malloc((unsigned int)NombreDeVariables * sizeof(double*));
    if (ProblemeLineairePartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees == NULL)
    {
        return (0);
    }

    ProblemeLineairePartieVariable->X.assign(NombreDeVariables, 0.);

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

    ProblemeLineairePartieVariable->CoutsReduits.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieVariable->CoutsMarginauxDesContraintes.assign(NombreDeContraintes, 0.);

    H2O_M_ConstruireLesVariables(DonneesAnnuelles);

    H2O_M_ConstruireLesContraintes(DonneesAnnuelles);

    return (DonneesAnnuelles);
}
