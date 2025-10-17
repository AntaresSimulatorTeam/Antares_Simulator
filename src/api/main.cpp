#include <iostream>

#include "antares/api/singleProblemGetter.h"

constexpr int kMaxDisplay = 10'000;

// This is a temporary client for singleProblemGetter.h for testing & debugging purposes

// Helper: convert any value to string
template<typename T>
std::string to_string_any(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Variadic print function that limits rows to n
template<typename... Vecs>
void print_side_by_side(size_t n, const Vecs&... vecs)
{
    for (size_t i = 0; i < n; ++i)
    {
        size_t col = 0;
        ((std::cout << (i < vecs.size() ? to_string_any(vecs[i]) : "")
                    << (++col < sizeof...(vecs) ? "\t" : "")),
         ...);
        std::cout << '\n';
    }
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

    print_side_by_side(constant.VariablesCount,
                       constant.VariablesMeaning,
                       weekly.Xmin,
                       weekly.Xmax,
                       weekly.LinearCost);
    print_side_by_side(constant.ConstraintesCount,
                       constant.ConstraintsMeaning,
                       weekly.Direction,
                       weekly.RHS);
    return 0;
}
