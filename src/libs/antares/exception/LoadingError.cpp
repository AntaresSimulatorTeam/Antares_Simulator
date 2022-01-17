#include "LoadingError.hpp"

namespace Antares
{
namespace Error
{
StudyFolderDoesNotExist::StudyFolderDoesNotExist(const Yuni::String& folder) :
 std::runtime_error(std::string("Study folder") + folder.c_str() + " does not exist.")
{
}

ReadingStudy::ReadingStudy() : std::runtime_error("Got a fatal error reading the study.")
{
}

NoAreas::NoAreas() :
 std::runtime_error("No area found. A valid study contains contains at least one.")
{
}

InvalidFileName::InvalidFileName() : std::runtime_error("Invalid file names detected.")
{
}

RuntimeInfoInitialization::RuntimeInfoInitialization() :
 std::runtime_error("Error initializing runtime infos.")
{
}

WritingPID::WritingPID(const Yuni::String& file) :
 std::runtime_error(std::string("Impossible to write pid file ") + file.c_str())
{
}

InvalidOptimizationRange::InvalidOptimizationRange() :
 std::runtime_error(
   "Invalid command line value for --optimization-range ('day' or 'week' expected)")
{
}

InvalidSimulationMode::InvalidSimulationMode() :
 std::runtime_error(
   "Only one simulation mode is allowed: --expansion, --economy, --adequacy or --adequacy-draft")
{
}

InvalidSolver::InvalidSolver(const std::string& solver) :
 std::runtime_error("Can't use solver '" + solver
                    + "' : it does not match any available OR-Tools solver")
{
}

InvalidStudy::InvalidStudy(const Yuni::String& study) :
 std::runtime_error(std::string("The folder `") + study.c_str()
                    + "` does not seem to be a valid study")
{
}

InvalidVersion::InvalidVersion(const char* version, const char* latest) :
 std::runtime_error(std::string("Invalid version for the study : found `") + version
                    + "`, expected <=`" + latest + '`')
{
}

NoStudyProvided::NoStudyProvided() :
 std::runtime_error("A study folder is required. Use '--help' for more information")
{
}

InvalidNumberOfMCYears::InvalidNumberOfMCYears(uint nbYears) :
 std::runtime_error("Invalid number of MC years, got " + std::to_string(nbYears)
                    + ", expected <= 50000")
{
}

IncompatibleParallelOptions::IncompatibleParallelOptions() :
 std::runtime_error("Options --parallel and --force-parallel are incompatible")
{
}

IncompatibleOptRangeHydroPricing::IncompatibleOptRangeHydroPricing() :
 std::runtime_error(
   "Simplex optimization range and hydro pricing mode : values are not compatible ")
{
}

IncompatibleOptRangeUCMode::IncompatibleOptRangeUCMode() :
 std::runtime_error(
   "Simplexe optimization range and unit commitment mode : values are not compatible")
{
}

IncompatibleDailyOptHeuristicForArea::IncompatibleDailyOptHeuristicForArea(
  const Antares::Data::AreaName& name) :
 std::runtime_error(
   std::string("Area ") + name.c_str()
   + " : simplex daily optimization and use heuristic target == no are not compatible")
{
}

WritingProgressFile::WritingProgressFile(const Yuni::String& file) :
 std::runtime_error(std::string("I/O error: impossible to write ") + file.c_str())
{
}

} // namespace Error
} // namespace Antares
