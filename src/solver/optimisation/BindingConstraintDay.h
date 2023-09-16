#pragma once
#include "constraint_builder.h"

struct BindingConstraintDayData : public BindingConstraintData
{
    BindingConstraintDayData(BindingConstraintDayData& data) :
     BindingConstraintData(data),
     CorrespondanceCntNativesCntOptimJournalieres(
       data.CorrespondanceCntNativesCntOptimJournalieres),
     NombreDePasDeTempsDUneJournee(data.NombreDePasDeTempsDUneJournee),
     NumeroDeJourDuPasDeTemps(data.NumeroDeJourDuPasDeTemps)
    {
    }
    BindingConstraintDayData(const char& TypeDeContrainteCouplante,
                             const int& NombreDInterconnexionsDansLaContrainteCouplante,
                             const std::vector<int>& NumeroDeLInterconnexion,
                             const std::vector<double>& PoidsDeLInterconnexion,
                             const std::vector<int>& OffsetTemporelSurLInterco,
                             const int& NombreDePaliersDispatchDansLaContrainteCouplante,
                             const std::vector<int>& PaysDuPalierDispatch,
                             const std::vector<int>& NumeroDuPalierDispatch,
                             const std::vector<double>& PoidsDuPalierDispatch,
                             const std::vector<int>& OffsetTemporelSurLePalierDispatch,
                             const char& SensDeLaContrainteCouplante,
                             const char* const& NomDeLaContrainteCouplante,
                             const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays,
                             std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES>&
                               CorrespondanceCntNativesCntOptimJournalieres,
                             const int32_t& NombreDePasDeTempsDUneJournee,
                             std::vector<int32_t>& NumeroDeJourDuPasDeTemps) :
     BindingConstraintData(TypeDeContrainteCouplante,
                           NombreDInterconnexionsDansLaContrainteCouplante,
                           NumeroDeLInterconnexion,
                           PoidsDeLInterconnexion,
                           OffsetTemporelSurLInterco,
                           NombreDePaliersDispatchDansLaContrainteCouplante,
                           PaysDuPalierDispatch,
                           NumeroDuPalierDispatch,
                           PoidsDuPalierDispatch,
                           OffsetTemporelSurLePalierDispatch,
                           SensDeLaContrainteCouplante,
                           NomDeLaContrainteCouplante,
                           PaliersThermiquesDuPays),
     CorrespondanceCntNativesCntOptimJournalieres(CorrespondanceCntNativesCntOptimJournalieres),
     NombreDePasDeTempsDUneJournee(NombreDePasDeTempsDUneJournee),
     NumeroDeJourDuPasDeTemps(NumeroDeJourDuPasDeTemps)
    {
    }

    // std::vector<std::vector<int>&>& CorrespondanceCntNativesCntOptimJournalieres;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES>&
      CorrespondanceCntNativesCntOptimJournalieres;

    const int32_t& NombreDePasDeTempsDUneJournee;

    std::vector<int32_t>& NumeroDeJourDuPasDeTemps;
};
class BindingConstraintDay : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante, BindingConstraintDayData& data);
};
