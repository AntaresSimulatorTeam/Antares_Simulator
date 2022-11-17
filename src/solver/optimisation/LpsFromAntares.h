#pragma once
#include <memory>
#include <map>
#include <array>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/array.hpp>

using YearWeekNum = std::array<int, 3>;
class LpFromAntares;
class ConstantDataFromAntares;
class HebdoDataFromAntares;

using ConstantDataFromAntaresPtr = std::shared_ptr<ConstantDataFromAntares>;
using HebdoDataFromAntaresPtr = std::shared_ptr<HebdoDataFromAntares>;

using WeekConstantDataFromAntares = std::map<int, ConstantDataFromAntaresPtr>;
using YearWeekNumHebdoDataFromAntares = std::map<YearWeekNum, HebdoDataFromAntaresPtr>;

class ConstantDataFromAntares {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & NombreDeVariables;
        ar & NombreDeContraintes;
        ar & NombreDeCoefficients;
        ar & TypeDeVariable;
        ar & Mdeb;
        ar & Nbterm;
        ar & IndicesColonnes;
        ar & CoefficientsDeLaMatriceDesContraintes;
    }
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
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, [[maybe_unused]] const unsigned int version) {
        ar & Sens;
        ar & Xmax;
        ar & Xmin;
        ar & CoutLineaire;
        ar & SecondMembre;
    }
public:
    std::vector<char>   Sens;
    std::vector<double> Xmax;
    std::vector<double> Xmin;
    std::vector<double> CoutLineaire;
    std::vector<double> SecondMembre;
};

class LpsFromAntares {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, [[maybe_unused]] const unsigned int version) {
        ar & _constant;
        ar & _hedbo;
    }

public:
    WeekConstantDataFromAntares _constant;
    YearWeekNumHebdoDataFromAntares _hedbo;
};
