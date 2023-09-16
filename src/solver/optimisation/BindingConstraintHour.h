#pragma once
#include "constraint_builder.h"

struct BindingConstraintHourData : public BindingConstraintData
{
    BindingConstraintHourData(BindingConstraintHourData& data) :
     BindingConstraintData(data),
     NumeroDeContrainteDesContraintesCouplantes(data.NumeroDeContrainteDesContraintesCouplantes)
    {
    }
    BindingConstraintHourData(const char& TypeDeContrainteCouplante,
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

    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};

class BindingConstraintHour : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pdt, int cntCouplante, BindingConstraintHourData& data);
};
