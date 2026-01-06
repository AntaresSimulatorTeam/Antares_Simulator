#include <iostream>
#include <fstream>
#include <mutex>
#include "yuni/core/getopt/parser.h"

#include "antares/api/singleProblemGetter.h"


using namespace Antares::Solver;
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
struct ApiOptions
{
    std::string studyFolder = "";
    std::string outputFolder = "";
    unsigned int year = -1;
    unsigned int week = -1;
    bool writeMps = false;
};

// Boost.ProgramOptions is another candidate
Yuni::GetOpt::Parser Parser(ApiOptions& options)
{
    Yuni::GetOpt::Parser parser;
    parser.addFlag(options.studyFolder, 'i', "input", "Study folder");
    std::cout<<" study folder: " << options.studyFolder << std::endl;
    parser.add(options.outputFolder, 'o', "output", "Output folder");
    parser.add(options.year, 'y', "year", "year");
    parser.add(options.week, 'w', "week", "week");
    parser.addFlag(options.writeMps, 's', "write-mps");
    
    parser.remainingArguments(options.studyFolder);
    return parser;
}

bool ParseOptions(ApiOptions& options, int argc,  const char* argv[])
{
    auto parser = Parser(options);
    switch (parser.operator()(argc, argv))
    {
    case Yuni::GetOpt::ReturnCode::error:
        throw Error::CommandLineArguments(parser.errors());
    case Yuni::GetOpt::ReturnCode::help:
        return false;
    default:
        return true;
    }
}

void ValidateOptions(const ApiOptions& options)
{
    if (options.studyFolder.empty())
    {
        throw std::invalid_argument(
          "Study Folder is empty, please enter valid study path checkout --help");
    }
}
void printWeek(const ConstantDataFromAntares& constant, const WeeklyDataFromAntares& weekly, const ApiOptions& options, const WeeklyProblemId& id){
      if (options.writeMps)
    {
        std::string mps;
        weekly.solver_->ExportModelAsMpsFormat(false, false, &mps);
        std::cout<<"Printing problem: "<<problemName(id)<<std::endl;

        std::cout << "******************************** BEGIN MPS ********************************"
                  << std::endl;
        std::cout << mps << std::endl;
        std::cout << "******************************** END MPS ********************************"
                  << std::endl;
        if(!options.outputFolder.empty()){
            std::filesystem::path outputPath = options.outputFolder;
            static std::once_flag once;
           std::call_once(once,  [&outputPath]
            {
                if(std::filesystem::exists(outputPath)){
                    std::filesystem::remove_all(outputPath);
                }
                std::filesystem::create_directories(outputPath);
            });

            std::filesystem::path mpsFile = outputPath / (problemName(id)+".mps");
            std::ofstream ofs(mpsFile);
            ofs << mps;
            ofs.close();
        }
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
}


void printProblems(const ApiOptions& options)
{

    Antares::Solver::SingleProblemGetter getter(options.studyFolder);
    auto constant = getter.getConstantData();
    auto nbYears = getter.nbYears();
    auto nbWeeks = getter.nbWeeks();
    std::cout << " * Number of years: " << nbYears << std::endl;
    std::cout << " * Number of weeks per year: " << nbWeeks << std::endl;
    int firstYear = options.year== -1 ? 0 : options.year;
    int lastYear = options.year == -1 ? nbYears : options.year + 1;
     std::cout << "Displaying problems for years [" << firstYear << "," << lastYear-1 << "]" <<std::endl;
     std::cout << " year: "<< year <<std::endl;
    int firstWeek = options.week== -1 ? 1 : options.week;
    int lastWeek = options.week == -1 ? nbWeeks +1 : options.week + 1;
    std::cout << " Displaying problems for weeks [" << firstWeek << "," << lastWeek-1 << "]" <<std::endl;
    
     for(int year = firstYear; year < lastYear; ++year){
    
        for(int week = firstWeek; week < lastWeek; ++week){
            std::cout << " week: "<< week <<std::endl;
            const WeeklyProblemId id = {year, week};
            auto weekly = getter.getWeeklyData(id);
            printWeek(constant, weekly, options, id);
        }
    
    }
}

int main(int argc, const char** argv)
{ 
    ApiOptions options;
    try
    {
        if (!ParseOptions(options, argc, argv))
        {
            return 0;
        }
        ValidateOptions(options);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing options: " << e.what() << std::endl
                    << "Use --help to display usage." << std::endl;
        return 1;
    }

    printProblems(options);
    return 0;
}
