#include <fstream>
#include <mutex>

#include "yuni/core/getopt/parser.h"

#include <antares/logs/logs.h>
#include "antares/api/singleProblemGetter.h"
#include "antares/io/outputs/MPSGenerator.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"

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
    std::ostringstream oss;
    for (size_t i = 0; i < n; ++i)
    {
        size_t col = 0;
        ((oss << (i < vecs.size() ? to_string_any(vecs[i]) : "")
              << (++col < sizeof...(vecs) ? "\t" : "")),
         ...);
        oss << '\n';
    }
    logs.info() << "\n" << oss.str();
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
    logs.info() << " study folder: " << options.studyFolder;
    parser.add(options.outputFolder, 'o', "output", "Output folder");
    parser.add(options.year, 'y', "year", "year");
    parser.add(options.week, 'w', "week", "week");
    parser.addFlag(options.writeMps, 's', "write-mps");

    parser.remainingArguments(options.studyFolder);
    return parser;
}

bool ParseOptions(ApiOptions& options, int argc, const char* argv[])
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
          "Study Folder is not provided, please enter valid study path, checkout --help");
    }
}

void writeWeekMPS(const WeeklyDataFromAntares& weekly,
                  const std::filesystem::path& outputPath,
                  const WeeklyProblemId& id)
{
    auto name = problemName(id);

    IO::Outputs::MPSGenerator mpsGenerator(*weekly.linearProblem, name + ".mps");
    const std::string mps = mpsGenerator.run();

    logs.info() << "Printing problem: " << name << '\n';

    // logs.info() << "******************************** BEGIN MPS ********************************";
    // logs.info() << '\n' << mps;
    // logs.info() << "******************************** END MPS ********************************";
    if (!outputPath.empty())
    {
        static std::once_flag once;
        std::call_once(once, [&outputPath] { std::filesystem::create_directories(outputPath); });
        IO::Outputs::MPSFileWriter::write(outputPath / (name + ".mps"), mps);
    }
}

void writeMasterAndStructure(const std::filesystem::path& studyPath,
                             const std::filesystem::path& outputDir)
{
    using namespace Antares::Solver;
    using namespace Antares::Optimisation;
    using namespace Antares::Optimisation::LinearProblemApi;
    namespace fs = std::filesystem;

    logs.info() << "Building master problem and Benders decomposition...";

    if (!outputDir.empty())
    {
        static std::once_flag once;
        std::call_once(once, [&outputDir] { std::filesystem::create_directories(outputDir); });
    }

    ModelerData data = LoadFiles::loadAll(studyPath);
    logs.info() << "Modeler data loaded";

    BendersDecomposition bendersDecomposition;
    FillContext fillContext = {0, 167, 0, 167, 0};

    auto master = Modeler::buildMasterProblem(data, bendersDecomposition, fillContext, "xpress");

    if (!master.problem)
    {
        logs.warning() << "Master problem is empty - not writing master.mps or structure.txt";
        return;
    }

    auto mps = IO::Outputs::MPSGenerator(*master.problem, "master").run();
    IO::Outputs::MPSFileWriter::write(outputDir / "master.mps", mps);
    logs.info() << "Written: " << (outputDir / "master.mps").string();

    BendersDecompositionWriter writer(bendersDecomposition);
    std::ofstream of(outputDir / "structure.txt");
    writer.write(of);
    logs.info() << "Written: " << (outputDir / "structure.txt").string();
}

void printProblems(const ApiOptions& options)
{
    Antares::Solver::SingleProblemGetter getter(options.studyFolder);
    auto constant = getter.getConstantData();
    auto nbYears = getter.nbYears();
    auto nbWeeks = getter.nbWeeks();
    logs.info() << " * Number of years: " << nbYears;
    logs.info() << " * Number of weeks per year: " << nbWeeks;

    int firstWeek = options.week == -1 ? 1 : options.week;
    int lastWeek = options.week == -1 ? nbWeeks + 1 : options.week + 1;

    for (int year: getter.playedYears())
    {
        logs.info() << " year: " << year << '\n';
        for (int week = firstWeek; week < lastWeek; ++week)
        {
            logs.info() << " week: " << week << '\n';
            const WeeklyProblemId id = {year, week};
            auto weekly = getter.getWeeklyData(id, true);
            if (options.writeMps)
            {
                writeWeekMPS(weekly, options.outputFolder, id);
            }
        }
    }

    if (options.writeMps)
    {
        writeMasterAndStructure(options.studyFolder, options.outputFolder);
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
        logs.error() << "Error parsing options: " << e.what() << '\n'
                     << "Use --help to display usage.";
        return 1;
    }

    printProblems(options);
    return 0;
}
