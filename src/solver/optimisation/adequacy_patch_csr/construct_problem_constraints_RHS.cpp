// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/logs/logs.h>
#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void HourlyCSRProblem::setRHSvalueOnFlows()
{
    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of
    // type 2.
    for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it = numberOfConstraintCsrFlowDissociation.find(Interco);
            if (it != numberOfConstraintCsrFlowDissociation.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] = 0.;
                logs.debug() << Cnt << "Flow=D-I: RHS[" << Cnt
                             << "] = " << problemeAResoudre_.SecondMembre[Cnt];
            }
        }
    }
}

void HourlyCSRProblem::setRHSnodeBalanceValue()
{
    // constraint:
    // ENS(node A) +
    // [ Sum flow_direct(node 2 upstream -> node A) + Sum flow_indirect(node A <- node 2 downstream)
    // – Sum flow_indirect(node 2 upstream <- node A) – Sum flow_direct(node A -> node 2 downstream)
    // ] – spillage(node A) = ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it = numberOfConstraintCsrAreaBalance.find(Area);
            if (it != numberOfConstraintCsrAreaBalance.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] = rhsAreaBalanceValues[Area];
                logs.debug() << Cnt << ": Area Balance: RHS[" << Cnt
                             << "] = " << problemeAResoudre_.SecondMembre[Cnt]
                             << " (Area = " << Area << ")";
            }
        }
    }
}

void HourlyCSRProblem::setRHSfictitiousLoadValue()
{
    // constraint: FictitiousLoad for all areas inside adequacy patch
    // Formula: spillage <= STt - (1-BT)*STmint + BH*Ht + BF*Max(0, Ft - Lt)
    // where:
    //   STt = sum of thermal dispatchable generation (from first optimization step)
    //   STmint = sum of Pmin of thermal units
    //   Ht = hydro generation (from first optimization step)
    //   Ft = must-run generation
    //   Lt = load
    //   BT = DefaillanceNegativeUtiliserPMinThermique
    //   BH = DefaillanceNegativeUtiliserHydro
    //   BF = DefaillanceNegativeUtiliserConsoAbattue
    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it = numberOfConstraintCsrFictitiousLoad.find(Area);
            if (it != numberOfConstraintCsrFictitiousLoad.end())
            {
                int Cnt = it->second;

                // Start with thermal dispatchable generation (STt)
                double stt = 0.0;

                const auto& paliersThermiques = problemeHebdo_->PaliersThermiquesDuPays[Area];
                const auto& productionThermique = problemeHebdo_->ResultatsHoraires[Area]
                                                    .ProductionThermique[triggeredHour];
                for (int index = 0; index < paliersThermiques.NombreDePaliersThermiques; index++)
                {
                    stt += productionThermique.ProductionThermiqueDuPalier[index];
                }

                // Subtract (1-BT)*STmint term
                // If BT is false: subtract STmint (sum of Pmin)
                // If BT is true: subtract 0 (i.e., use full STt)
                double stmint = 0.0;
                if (!problemeHebdo_->DefaillanceNegativeUtiliserPMinThermique[Area])
                {
                    for (int index = 0; index < paliersThermiques.NombreDePaliersThermiques;
                         index++)
                    {
                        stmint += paliersThermiques.PminDuPalierThermiquePendantUneHeure[index];
                    }
                }

                // Add BH*Ht term (hydro generation if enabled)
                double ht = 0.0;
                if (problemeHebdo_->DefaillanceNegativeUtiliserHydro[Area])
                {
                    ht = problemeHebdo_->ResultatsHoraires[Area].TurbinageHoraire[triggeredHour];
                }

                // Add BF*Max(0, Ft - Lt) term
                double bfTerm = 0.0;
                if (problemeHebdo_->DefaillanceNegativeUtiliserConsoAbattue[Area])
                {
                    double allMustRunGen = problemeHebdo_->AllMustRunGeneration[triggeredHour]
                                             .AllMustRunGenerationOfArea[Area];
                    double consommationAbattue = problemeHebdo_
                                                   ->ConsommationsAbattues[triggeredHour]
                                                   .ConsommationAbattueDuPays[Area];

                    // Ft = allMustRunGen (must-run generation)
                    // Lt = -consommationAbattue (load is negative in this context)
                    double ftMinusLt = allMustRunGen + consommationAbattue;
                    bfTerm = std::max(0.0, ftMinusLt);
                }

                // Add short term storage withdrawal
                double stsNetProduction = 0.0;
                const auto& shortTermStorageResults = problemeHebdo_->ResultatsHoraires[Area]
                                                        .ShortTermStorage;
                for (const auto& storageResults: shortTermStorageResults)
                {
                    stsNetProduction += storageResults.withdrawal[triggeredHour];
                }

                // RHS = STt - (1-BT)*STmint + BH*Ht + BF*Max(0, Ft - Lt) + STS_net_production
                double rhs = stt - stmint + ht + bfTerm + stsNetProduction;

                problemeAResoudre_.SecondMembre[Cnt] = rhs;
                logs.debug() << Cnt << ": FictitiousLoad: RHS[" << Cnt
                             << "] = " << problemeAResoudre_.SecondMembre[Cnt]
                             << " (Area = " << Area << ")";
            }
        }
    }
}

void HourlyCSRProblem::setRHSMaxEnsLoadValue()
{
    std::vector<double>& SecondMembre = problemeAResoudre_.SecondMembre;

    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; ++Area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it = numberOfConstraintCsrMaxEnsLoad.find(Area);
            if (it != numberOfConstraintCsrMaxEnsLoad.end())
            {
                int Cnt = it->second;

                double load = problemeHebdo_->ConsommationsAbattues[triggeredHour]
                                .ConsommationAbattueDuPays[Area];

                const auto& shortTermStorageResults = problemeHebdo_->ResultatsHoraires[Area]
                                                        .ShortTermStorage;
                for (const auto& storageResults: shortTermStorageResults)
                {
                    load += storageResults.injection[triggeredHour];
                }

                SecondMembre[Cnt] = load;
                logs.debug() << Cnt << ": MaxEnsLoad: RHS[" << Cnt << "] = " << SecondMembre[Cnt]
                             << " (Area = " << Area << ")";
            }
        }
    }
}

void HourlyCSRProblem::setRHSbindingConstraintsValue()
{
    std::vector<double>& SecondMembre = problemeAResoudre_.SecondMembre;

    // constraint:
    // user defined Binding constraints between transmission flows
    // and/or power generated from generating units.
    for (uint32_t CntCouplante = 0; CntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (numberOfConstraintCsrHourlyBinding.find(CntCouplante)
            == numberOfConstraintCsrHourlyBinding.end())
        {
            continue;
        }

        const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
          = problemeHebdo_->MatriceDesContraintesCouplantes[CntCouplante];

        int Cnt = numberOfConstraintCsrHourlyBinding[CntCouplante];

        // 1. The original RHS of bingding constraint
        SecondMembre[Cnt] = MatriceDesContraintesCouplantes
                              .SecondMembreDeLaContrainteCouplante[triggeredHour];

        // 2. RHS part 2: flow other than 2<->2
        int NbInterco = MatriceDesContraintesCouplantes
                          .NombreDInterconnexionsDansLaContrainteCouplante;
        for (int Index = 0; Index < NbInterco; Index++)
        {
            int Interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[Index];
            double Poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[Index];

            if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
                  != Data::AdequacyPatch::physicalAreaInsideAdqPatch
                || problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                     != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                double ValueOfFlow = problemeHebdo_->ValeursDeNTC[triggeredHour]
                                       .ValeurDuFlux[Interco];
                SecondMembre[Cnt] -= ValueOfFlow * Poids;
            }
        }

        // 3. RHS part 3: - cluster
        int NbClusters = MatriceDesContraintesCouplantes
                           .NombreDePaliersDispatchDansLaContrainteCouplante;

        for (int Index = 0; Index < NbClusters; Index++)
        {
            int Area = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[Index];

            int IndexNumeroDuPalierDispatch = MatriceDesContraintesCouplantes
                                                .NumeroDuPalierDispatch[Index];

            double Poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[Index];

            double ValueOfVar = problemeHebdo_->ResultatsHoraires[Area]
                                  .ProductionThermique[triggeredHour]
                                  .ProductionThermiqueDuPalier[IndexNumeroDuPalierDispatch];

            SecondMembre[Cnt] -= ValueOfVar * Poids;
        }
        if (MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante == '<')
        {
            SecondMembre[Cnt] += belowThisThresholdSetToZero;
        }
        else if (MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante == '>')
        {
            SecondMembre[Cnt] -= belowThisThresholdSetToZero;
        }
        logs.debug() << Cnt << ": Hourly bc: -RHS[" << Cnt << "] = " << SecondMembre[Cnt];
    }
}
