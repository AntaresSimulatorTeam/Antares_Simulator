#pragma once
#include <memory>
#include <map>
#include <array>
#include <vector>


using YearWeekNum = std::array<int, 3>;
class LpFromAntares;
class ConstantDataFromAntares;
class HebdoDataFromAntares;

using ConstantDataFromAntaresPtr = std::shared_ptr<ConstantDataFromAntares>;
using HebdoDataFromAntaresPtr = std::shared_ptr<HebdoDataFromAntares>;

using WeekConstantDataFromAntares = std::map<int, ConstantDataFromAntaresPtr>;
using YearWeekNumHebdoDataFromAntares = std::map<YearWeekNum, HebdoDataFromAntaresPtr>;

class ConstantDataFromAntares {
public:
    int     NombreDeVariables;
    int     NombreDeContraintes;
    int     NombreDeCoefficients;

    std::vector<int> TypeDeVariable;
    std::vector<int> Mdeb;
    std::vector<int> Nbterm;
    std::vector<int> IndicesColonnes;
    std::vector<double> CoefficientsDeLaMatriceDesContraintes;
};

class HebdoDataFromAntares {
public:
    std::vector<char>   Sens;
    std::vector<double> Xmax;
    std::vector<double> Xmin;
    std::vector<double> CoutLineaire;
    std::vector<double> SecondMembre;
};

class LpsFromAntares {
public:
    WeekConstantDataFromAntares _constant;
    YearWeekNumHebdoDataFromAntares _hedbo;
};
