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

// Base case: find max size among vectors
template<typename T>
size_t max_size(const std::vector<T>& v)
{
    return v.size();
}

template<typename T, typename... Rest>
size_t max_size(const std::vector<T>& v, const Rest&... rest)
{
    return std::max(v.size(), max_size(rest...));
}

// Recursive printing row by row
template<typename... Vecs>
void print_side_by_side(const Vecs&... vecs)
{
    size_t n = max_size(vecs...);
    for (size_t i = 0; i < n; ++i)
    {
        size_t col = 0;
        // Fold expression to print each column
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

    print_side_by_side(constant.VariablesMeaning, weekly.Xmin, weekly.Xmax, weekly.LinearCost);
    print_side_by_side(constant.ConstraintsMeaning, weekly.Direction, weekly.RHS);
    return 0;
}
