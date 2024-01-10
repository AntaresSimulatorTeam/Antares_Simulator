/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/sim_structure_donnees.h"
#include "../simulation/adequacy_patch_runtime_data.h"

#include "../solver/optimisation/opt_fonctions.h"

#include "pi_constantes_externes.h"
#include "sim_structure_probleme_economique.h"

#include <yuni/core/math.h>

using namespace Yuni;

void HourlyCSRProblem::setBoundsOnENS()
{
    double* AdresseDuResultat;
    const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];

    // variables: ENS for each area inside adq patch
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDefaillancePositive[area];

            problemeAResoudre_.Xmin[var] = -belowThisThresholdSetToZero;
            problemeAResoudre_.Xmax[var]
              = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDENS[triggeredHour]
                + belowThisThresholdSetToZero;

            problemeAResoudre_.X[var] = problemeHebdo_->ResultatsHoraires[area]
                                          .ValeursHorairesDeDefaillancePositive[triggeredHour];

            AdresseDuResultat = &(problemeHebdo_->ResultatsHoraires[area]
                                    .ValeursHorairesDeDefaillancePositive[triggeredHour]);

            problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
              = AdresseDuResultat;

            logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                         << problemeAResoudre_.Xmax[var];
        }
    }
}

void HourlyCSRProblem::setBoundsOnSpilledEnergy()
{
    const auto& CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];

    // variables: Spilled Energy for each area inside adq patch
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDefaillanceNegative[area];

            problemeAResoudre_.Xmin[var] = -belowThisThresholdSetToZero;
            problemeAResoudre_.Xmax[var] = LINFINI_ANTARES;

            problemeAResoudre_.X[var] = problemeHebdo_->ResultatsHoraires[area]
                                          .ValeursHorairesDeDefaillanceNegative[triggeredHour];

            double* AdresseDuResultat = &(problemeHebdo_->ResultatsHoraires[area]
                                            .ValeursHorairesSpilledEnergyAfterCSR[triggeredHour]);

            problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
              = AdresseDuResultat;

            logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                         << problemeAResoudre_.Xmax[var];
        }
    }
}

void HourlyCSRProblem::setBoundsOnFlows()
{
    const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];
    std::vector<double>& Xmin = problemeAResoudre_.Xmin;
    std::vector<double>& Xmax = problemeAResoudre_.Xmax;
    VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo_->ValeursDeNTC[triggeredHour];

    // variables bounds: transmissin flows (flow, direct_direct and flow_indirect). For links
    // between nodes of type 2. Set hourly bounds for links between nodes of type 2, depending on
    // the user input (max direct and indirect flow).
    for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; ++Interco)
    {
        // only consider link between 2 and 2
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              != Data::AdequacyPatch::physicalAreaInsideAdqPatch
            || problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        // flow
        int var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[Interco];
        Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco] + belowThisThresholdSetToZero;
        Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco]) - belowThisThresholdSetToZero;
        problemeAResoudre_.X[var] = ValeursDeNTC.ValeurDuFlux[Interco];

        if (Math::Infinite(Xmax[var]) == 1)
        {
            if (Math::Infinite(Xmin[var]) == -1)
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_NON_BORNEE;
            else
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }
        else
        {
            if (Math::Infinite(Xmin[var]) == -1)
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
            else
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        }

        double* AdresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[Interco]);
        problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];

        // direct / indirect flow
        var = CorrespondanceVarNativesVarOptim
                .NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];

        Xmin[var] = -belowThisThresholdSetToZero;
        Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco] + belowThisThresholdSetToZero;
        problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (Math::Infinite(Xmax[var]) == 1)
        {
            problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];

        var = CorrespondanceVarNativesVarOptim
                .NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];

        Xmin[var] = -belowThisThresholdSetToZero;
        Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco] + belowThisThresholdSetToZero;
        problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (Math::Infinite(Xmax[var]) == 1)
        {
            problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];
    }
}
