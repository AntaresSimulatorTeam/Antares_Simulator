// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <pi_constantes_externes.h>

#include <antares/logs/logs.h>
#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/study/parameters/adq-patch-params.h"

using namespace Antares;
using namespace Antares::Data::AdequacyPatch;

void HourlyCSRProblem::setBoundsOnENS()
{
    double* AdresseDuResultat;
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            int var = variableManager_.UnsuppliedEnergy(area, triggeredHour);

            double ensLegacy = problemeHebdo_->ResultatsHoraires[area]
                                 .ValeursHorairesDENS[triggeredHour];

            problemeAResoudre_.Xmin[var] = -belowThisThresholdSetToZero;
            problemeAResoudre_.Xmax[var] = ensLegacy + belowThisThresholdSetToZero;

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
    // variables: Spilled Energy for each area inside adq patch
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            int var = variableManager_.Spillage(area, triggeredHour);

            problemeAResoudre_.Xmin[var] = -belowThisThresholdSetToZero;
            problemeAResoudre_.Xmax[var] = LINFINI_ANTARES;

            problemeAResoudre_.X[var] = problemeHebdo_->ResultatsHoraires[area]
                                          .ValeursHorairesDeDefaillanceNegative[triggeredHour];

            double* AdresseDuResultat = &(problemeHebdo_->ResultatsHoraires[area]
                                            .ValeursHorairesDeDefaillanceNegative[triggeredHour]);

            problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
              = AdresseDuResultat;

            logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                         << problemeAResoudre_.Xmax[var];
        }
    }
}

void HourlyCSRProblem::setBoundsOnFlows()
{
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
        int var = variableManager_.DirectFlow(Interco, triggeredHour);
        Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco]
                    + belowThisThresholdSetToZero;
        Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco])
                    - belowThisThresholdSetToZero;
        problemeAResoudre_.X[var] = ValeursDeNTC.ValeurDuFlux[Interco];

        if (std::isinf(Xmax[var]))
        {
            if (std::isinf(Xmin[var]))
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_NON_BORNEE;
            }
            else
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
        }
        else
        {
            if (std::isinf(Xmin[var]))
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
            }
            else
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }
        }

        double* AdresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[Interco]);
        problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];

        // direct / indirect flow
        var = variableManager_.PositiveDirectFlow(Interco, triggeredHour);

        Xmin[var] = -belowThisThresholdSetToZero;
        Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco]
                    + belowThisThresholdSetToZero;
        problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (std::isinf(Xmax[var]))
        {
            problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];

        var = variableManager_.PositiveIndirectFlow(Interco, triggeredHour);

        Xmin[var] = -belowThisThresholdSetToZero;
        Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco]
                    + belowThisThresholdSetToZero;
        problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (std::isinf(Xmax[var]))
        {
            problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];
    }
}
