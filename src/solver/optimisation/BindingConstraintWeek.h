#pragma once
#include "constraint_builder.h"

struct BindingConstraintWeekData : public BindingConstraintData
{
    BindingConstraintWeekData(BindingConstraintWeekData& data) :
     BindingConstraintData(data),
     NumeroDeContrainteDesContraintesCouplantes(data.NumeroDeContrainteDesContraintesCouplantes)
    {
    }

    BindingConstraintWeekData(const char& TypeDeContrainteCouplante,
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
                              std::vector<int>& NumeroDeContrainteDesContraintesCouplantes) :
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
     NumeroDeContrainteDesContraintesCouplantes(NumeroDeContrainteDesContraintesCouplantes)
    {
    }
    // std::vector<std::vector<int>&>& CorrespondanceCntNativesCntOptimJournalieres;
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
    // CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires;
};

class BindingConstraintWeek : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante, BindingConstraintWeekData& data);
};