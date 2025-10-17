#include <iostream>

#include "antares/api/singleProblemGetter.h"

constexpr int kMaxDisplay = 10'000;

// This is a temporary client for singleProblemGetter.h for testing & debugging purposes

// Utility for printing vectors with optional truncation
template<typename T>
std::ostream& printVector(std::ostream& os,
                          const std::vector<T>& vec,
                          std::size_t maxDisplay = kMaxDisplay)
{
    os << "(size = " << vec.size() << " ) [";
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (i >= maxDisplay)
        {
            os << " ... (" << vec.size() << " elements)";
            break;
        }
        os << vec[i];
        if (i + 1 < vec.size() && i + 1 < maxDisplay)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Antares::Solver::ConstantDataFromAntares& data)
{
    os << "ConstantDataFromAntares {\n"
       << "  VariablesCount       = " << data.VariablesCount << ",\n"
       << "  ConstraintesCount    = " << data.ConstraintesCount << ",\n"
       << "  CoeffCount           = " << data.CoeffCount << ",\n"
       << "  VariablesType        = ";
    printVector(os, data.VariablesType) << ",\n"
                                        << "  Mdeb                 = ";
    printVector(os, data.Mdeb) << ",\n"
                               << "  NotNullTermCount     = ";
    printVector(os, data.NotNullTermCount) << ",\n"
                                           << "  ColumnIndexes        = ";
    printVector(os, data.ColumnIndexes) << ",\n"
                                        << "  ConstraintsMatrixCoeff = ";
    printVector(os, data.ConstraintsMatrixCoeff) << ",\n"
                                                 << "  VariablesMeaning     = ";
    printVector(os, data.VariablesMeaning) << ",\n"
                                           << "  ConstraintsMeaning   = ";
    printVector(os, data.ConstraintsMeaning) << "\n"
                                             << "}";
    return os;
}

// Overload specifically for printing std::vector<char> more clearly
std::ostream& printVector(std::ostream& os,
                          const std::vector<char>& vec,
                          std::size_t maxDisplay = kMaxDisplay)
{
    os << "[";
    os << "(size = " << vec.size() << ") [";
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (i >= maxDisplay)
        {
            os << " ... (" << vec.size() << " elements)";
            break;
        }
        os << '\'' << vec[i] << '\'';
        if (i + 1 < vec.size() && i + 1 < maxDisplay)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Antares::Solver::WeeklyDataFromAntares& data)
{
    os << "WeeklyDataFromAntares {\n"
       << "  name         = \"" << data.name << "\",\n"
       << "  Direction    = ";
    printVector(os, data.Direction) << ",\n"
                                    << "  Xmax         = ";
    printVector(os, data.Xmax) << ",\n"
                               << "  Xmin         = ";
    printVector(os, data.Xmin) << ",\n"
                               << "  LinearCost   = ";
    printVector(os, data.LinearCost) << ",\n"
                                     << "  RHS          = ";
    printVector(os, data.RHS) << "\n"
                              << "}";
    return os;
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage \n" << argv[0] << " path/to/study year week\n";
        return 1;
    }
    const unsigned int year = std::atoi(argv[2]);
    const unsigned int week = std::atoi(argv[3]);

    Antares::Solver::SingleProblemGetter getter;
    getter.load(argv[1]);
    auto constant = getter.getConstantData();
    auto weekly = getter.getWeeklyData({year, week});

    std::cout << constant << std::endl;
    std::cout << weekly << std::endl;
    return 0;
}
