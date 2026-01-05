#include <iostream>
#include <coroutine>
#include <fstream>
#include <mutex>
#include "yuni/core/getopt/parser.h"

#include "antares/api/singleProblemGetter.h"

#include "private/singleProblemGetterImpl.h"

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


template<typename T>
struct Generator {
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    
    std::coroutine_handle<promise_type> handle;
    
    Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Generator() { if (handle) handle.destroy(); }
    
    bool next() {
        handle.resume();
        return !handle.done();
    }
    
    T value() {
        return handle.promise().current_value;
    }
};


Generator<int> counter(int index, int nb, bool zeroFirst) {
    int shift = zeroFirst ? 0 : 1;
    int start = index == -1 ? shift: index;
    int end = index == -1 ? nb+shift : index+1;
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}



void printProblems(const ApiOptions& options)
{

    Antares::Solver::SingleProblemGetter getter(options.studyFolder);
    auto constant = getter.getConstantData();
    auto nbYears = getter.nbYears();
    auto nbWeeks = getter.nbWeeks();
    std::cout << " * Number of years: " << nbYears << std::endl;
    std::cout << " * Number of weeks per year: " << nbWeeks << std::endl;
    auto yearGen = counter(options.year, nbYears, true);
    while(yearGen.next()){
        std::cout << " year: "<< yearGen.value() <<std::endl;
        auto weekGen = counter(options.week, nbWeeks, false);
        while(weekGen.next()){

        std::cout << " week: "<< weekGen.value() <<std::endl;
            const WeeklyProblemId id = {yearGen.value(), weekGen.value()};
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
