#pragma once
#include <memory>
#include <map>
#include <array>
#include <vector>


typedef std::array<int, 3> YearWeekNum;
class LpFromAntares;
class ConstantDataFromAntares;
class HebdoDataFromAntares;

typedef std::shared_ptr<ConstantDataFromAntares> ConstantDataFromAntaresPtr;
typedef std::shared_ptr<HebdoDataFromAntares> HebdoDataFromAntaresPtr;

typedef std::map<int, ConstantDataFromAntaresPtr> WeekConstantDataFromAntares;
typedef std::map<YearWeekNum, HebdoDataFromAntaresPtr> YearWeekNumHebdoDataFromAntares;

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
