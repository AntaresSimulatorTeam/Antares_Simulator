// Je modifie la structure de Manuel en fonction des remarques que je vous ai
// faites vendredi

#pragma once
#include <array>
#include <map>
#include <memory>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>

struct ProblemHebdoId {
    unsigned int year;
    unsigned int week;

    bool operator <(const ProblemHebdoId& other) const {
        if (year < other.year)
            return true;
        if (year == other.year)
            return week < other.week;
        return false;
    }

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar& year;
        ar& week;
    }
};

class ConstantDataFromAntares;
class HebdoDataFromAntares;

using ConstantDataFromAntaresPtr = std::shared_ptr<ConstantDataFromAntares>;
using HebdoDataFromAntaresPtr = std::shared_ptr<HebdoDataFromAntares>;

// Type de données inutile car les matrices de tous les pbs hebdo sont
// identiques. Cela pourra changer à l'avenir si des coefficients de contraintes
// couplantes peuvent varier au cours du temps (ex: rendement d'une pompe à
// chaleur qui varie selon la température, FlowBased ?, etc)
using YearWeekHebdoDataFromAntares =
        std::map<ProblemHebdoId, HebdoDataFromAntaresPtr>;

class ConstantDataFromAntares {
public:
    int NombreDeVariables;     // Mathématiquement : Nb colonnes de la matrice,
    // Informatiquement = TypeDeVariable.size()
    int NombreDeContraintes;   // Mathématiqument : Nb lignes de la matrice,
    // Informatiquement = Mdeb.size()
    int NombreDeCoefficients;  // Mathématiquement : Nb coeffs non nuls de la
    // matrice, Informatiquement = Nbterm.size() =
    // IndicesColonnes.size()=
    // CoefficientsDeLaMatriceDesContraintes.size()

    std::vector<int> TypeDeVariable;  // Variables entières ou biniaires
    std::vector<int>
            Mdeb;  // Indique dans les indices dans le vecteur IndicesColonnes qui
    // correspondent au début de chaque ligne. Ex : Mdeb[3] = 8 et
    // Mdeb[4] = 13 -> Les termes IndicesColonnes[8] à
    // IndicesColonnes[12] correspondent à des Id de colonnes de la
    // ligne 3 de la matrice (en supposant que les lignes sont indexées
    // à partir de 0)
    std::vector<int> Nbterm;  // Nombre de termes non nuls sur chaque ligne.
    // Inutile car NbTerm[i] = Mdeb[i+1] - Mdeb[i]
    std::vector<int>
            IndicesColonnes;  // Id des colonnes des termes de
    // CoefficientsDeLaMatriceDesContraintes : Ex
    // IndicesColonnes[3] = 8 ->
    // CoefficientsDeLaMatriceDesContraintes[8] donne la
    // valeur du terme de la colonne 8, et de la ligne i où
    // i est tel que Mdeb[i] <= 3 < Mdeb[i+1]
    std::vector<double>
            CoefficientsDeLaMatriceDesContraintes;  // Coefficients de la matrice

    std::vector<std::string>
            SignificationMetierDesVariables;  // Nouveau par rapport à ce qu'à fait
    // Manuel, contient les infos
    // actuellement dans variables.txt. ->
    // Utile pour l'étape
    // problem_generation. Comme ce fichier
    // contient des infos de type
    // {signification metier, id zone, pas
    // horaire} ou {signification metier, id
    // lien, pas horaire}, peut-être qu'un
    // vecteur de string n'est pas le
    // meilleur choix (et il faudra aller
    // vérifier précisément les infos dont
    // on a besoin qui sont un peu
    // différentes selon les variables)
    std::vector<std::string>
            SignificationMetierDesContraintes;  // Nouveau mais pas utile dans
    // l'immédiat (y penser lorsqu'on
    // essaiera d'investir directement sur
    // les noeuds ou tester des choses
    // plus complexes sur la gestion des
    // stocks), idem que précédemment mais
    // pour les contraintes, nécessite de
    // définir précisément les infos dont
    // on a besoin -> à faire lorsqu'on
    // fera le nommage des contraintes
    // dans Antarès ?

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar& NombreDeVariables;
        ar& NombreDeContraintes;
        ar& NombreDeCoefficients;
        ar& TypeDeVariable;
        ar& Mdeb;
        ar& Nbterm;
        ar& IndicesColonnes;
        ar& CoefficientsDeLaMatriceDesContraintes;
        ar& SignificationMetierDesVariables;
        ar& SignificationMetierDesContraintes;
    }
};

class HebdoDataFromAntares {
public:
    std::vector<char> Sens;  // Sens de la contrainte : < ou > ou =, taille = NombreDeContraintes
    std::vector<double>
            Xmax;  // Borne max des variables de la semaine considérée, taille = NombreDeVariables
    std::vector<double>
            Xmin;  // Borne min des variables de la semaine considérée, taille =  NombreDeVariables
    std::vector<double>
            CoutLineaire;  // Coefficients du vecteur de coût de la fonction objectif,
    // taille = NombreDeVariables
    std::vector<double>
            SecondMembre;  // Vecteur des second membre des contraintes, taille = NombreDeContraintes

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar& Sens;
        ar& Xmax;
        ar& Xmin;
        ar& CoutLineaire;
        ar& SecondMembre;
    }
};

class LpsFromAntares {
public:
    ConstantDataFromAntaresPtr _constant;
    YearWeekHebdoDataFromAntares _hebdo;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar& _constant;
        ar& _hebdo;
    }
};
