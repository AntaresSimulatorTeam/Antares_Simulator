#include "constraint_builder.h"

// for debug
int Variable::Visitor::operator()(const NTCDirect& v) const
{
    return nativeOptimVar.NumeroDeVariableDeLInterconnexion[v.index];
}

ConstraintBuilderData::ConstraintBuilderData(
  std::vector<double>& Pi,
  std::vector<int>& Colonne,
  int& nombreDeContraintes,
  int& nombreDeTermesDansLaMatriceDeContrainte,
  std::vector<int>& IndicesDebutDeLigne,
  std::vector<double>& CoefficientsDeLaMatriceDesContraintes,
  std::vector<int>& IndicesColonnes,
  int& NombreDeTermesAllouesDansLaMatriceDesContraintes,
  std::vector<int>& NombreDeTermesDesLignes,
  std::string& Sens,
  int& IncrementDAllocationMatriceDesContraintes,
  const std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
  const int32_t& NombreDePasDeTempsPourUneOptimisation,
  const std::vector<int>& NumeroDeVariableStockFinal,
  const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
  std::vector<std::string>& NomDesContraintes,
  const bool& NamedProblems,
  const std::vector<const char*>& NomsDesPays,
  const uint32_t& weekInTheYear,
  const uint32_t& NombreDePasDeTemps) :
 Pi(Pi),
 Colonne(Colonne),
 nombreDeContraintes(nombreDeContraintes),
 nombreDeTermesDansLaMatriceDeContrainte(nombreDeTermesDansLaMatriceDeContrainte),
 IndicesDebutDeLigne(IndicesDebutDeLigne),
 CoefficientsDeLaMatriceDesContraintes(CoefficientsDeLaMatriceDesContraintes),
 IndicesColonnes(IndicesColonnes),
 NombreDeTermesAllouesDansLaMatriceDesContraintes(NombreDeTermesAllouesDansLaMatriceDesContraintes),
 NombreDeTermesDesLignes(NombreDeTermesDesLignes),
 Sens(Sens),
 IncrementDAllocationMatriceDesContraintes(IncrementDAllocationMatriceDesContraintes),
 CorrespondanceVarNativesVarOptim(CorrespondanceVarNativesVarOptim),
 NombreDePasDeTempsPourUneOptimisation(NombreDePasDeTempsPourUneOptimisation),
 NumeroDeVariableStockFinal(NumeroDeVariableStockFinal),
 NumeroDeVariableDeTrancheDeStock(NumeroDeVariableDeTrancheDeStock),
 NomDesContraintes(NomDesContraintes),
 NamedProblems(NamedProblems),
 NomsDesPays(NomsDesPays),
 weekInTheYear(weekInTheYear),
 NombreDePasDeTemps(NombreDePasDeTemps)
{
}

void ConstraintBuilder::build()
{
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
                // Matrix
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes();
    }
    nombreDeTermes_ = 0;
}

int ConstraintBuilder::getVariableIndex(const Variable::Variant& variable,
                                        int shift,
                                        bool wrap,
                                        int delta) const
{
    auto pdt = hourInWeek_ + shift;
    const auto nbTimeSteps = data.NombreDePasDeTempsPourUneOptimisation;
    // if (wrap)
    // {
    //     pdt %= nbTimeSteps;
    //     if (pdt < 0)
    //         pdt += problemeHebdo->NombreDePasDeTemps;
    // }
    // if (pdt < 0 || pdt >= nbTimeSteps)
    // {
    //     return -1;
    // }

    if (wrap)
    {
        if (shift >= 0)
        {
            pdt = (pdt) % nbTimeSteps;
        }
        else
        {
            pdt = (pdt + delta) % nbTimeSteps;
        }
    }
    // auto& a = data.CorrespondanceVarNativesVarOptim[pdt];
    const Variable::Visitor visitor(data.CorrespondanceVarNativesVarOptim[pdt],
                                    data.NumeroDeVariableStockFinal,
                                    data.NumeroDeVariableDeTrancheDeStock);
    return std::visit(visitor, variable);
}

ConstraintBuilder& ConstraintBuilder::include(Variable::Variant var,
                                              double coeff,
                                              int shift,
                                              bool wrap,
                                              int delta)
{
    int varIndex = getVariableIndex(var, shift, wrap, delta);
    if (varIndex >= 0)
    {
        data.Pi[nombreDeTermes_] = coeff;
        data.Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
    return *this;
}

void ConstraintBuilder::OPT_ChargerLaContrainteDansLaMatriceDesContraintes()
{
    // int& nombreDeContraintes = data.nombreDeContraintes;
    // int& nombreDeTermesDansLaMatriceDeContrainte = data.nombreDeTermesDansLaMatriceDeContrainte;
    // std::vector<double>& Pi = data.Pi;
    // std::vector<int>& Colonne = data.Colonne;

    data.IndicesDebutDeLigne[data.nombreDeContraintes]
      = data.nombreDeTermesDansLaMatriceDeContrainte;
    for (int i = 0; i < nombreDeTermes_; i++)
    {
        data.CoefficientsDeLaMatriceDesContraintes[data.nombreDeTermesDansLaMatriceDeContrainte]
          = data.Pi[i];
        data.IndicesColonnes[data.nombreDeTermesDansLaMatriceDeContrainte] = data.Colonne[i];
        data.nombreDeTermesDansLaMatriceDeContrainte++;
        if (data.nombreDeTermesDansLaMatriceDeContrainte
            == data.NombreDeTermesAllouesDansLaMatriceDesContraintes)
        {
            OPT_AugmenterLaTailleDeLaMatriceDesContraintes();
        }
    }
    data.NombreDeTermesDesLignes[data.nombreDeContraintes] = nombreDeTermes_;

    data.Sens[data.nombreDeContraintes] = operator_;
    data.nombreDeContraintes++;

    return;
}

void ConstraintBuilder::OPT_AugmenterLaTailleDeLaMatriceDesContraintes()
{
    int NbTermes = data.NombreDeTermesAllouesDansLaMatriceDesContraintes;
    NbTermes += data.IncrementDAllocationMatriceDesContraintes;

    logs.info();
    logs.info() << " Expected Number of Non-zero terms in Problem Matrix : increased to : "
                << NbTermes;
    logs.info();

    data.CoefficientsDeLaMatriceDesContraintes.resize(NbTermes);

    data.IndicesColonnes.resize(NbTermes);

    data.NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
}