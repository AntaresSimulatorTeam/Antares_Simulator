#include <iostream>

#include "antares/api/singleProblemGetter.h"

#include "private/singleProblemGetterImpl.h"

constexpr int kMaxDisplay = 10'000;

// This is a temporary client for singleProblemGetter.h for testing & debugging purposes

// Helper: convert any value to string
template<typename T>
std::string to_string_any(const T& value)
{
    std::ostringstream oss;
    oss << std::setprecision(8) << value;
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

unsigned int toInt(const char* in)
{
    return strtoul(in, nullptr, 10);
}

int main(int argc, char** argv)
{ // dirty options reader
    if (argc < 4 || argc > 5)
    {
        std::cerr << "Usage \n" << argv[0] << " path/to/study year week [--mps]\n";
        return 1;
    }

    const unsigned int year = toInt(argv[2]);
    const unsigned int week = toInt(argv[3]);
    bool writeMps = false;
    if (argc == 5 && std::string(argv[4]) != "--mps")
    {
        std::cerr << "skipped unknown option " << argv[4] << std::endl;
    }
    else
    {
        writeMps = true;
    }

    Antares::Solver::SingleProblemGetter getter(argv[1]);
    auto constant = getter.getConstantData();
    auto weekly = getter.getWeeklyData({year, week});
    if (writeMps)
    {
        std::string mps;
        weekly.solver_->ExportModelAsMpsFormat(false, false, &mps);

        std::cout << "******************************** BEGIN MPS ********************************"
                  << std::endl;
        std::cout << mps << std::endl;
        std::cout << "******************************** END MPS ********************************"
                  << std::endl;
    }

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
