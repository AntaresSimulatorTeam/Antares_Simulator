#include "LoadingError.hpp"

namespace Antares
{
namespace Error
{
StudyFolderDoesNotExist::StudyFolderDoesNotExist(const Yuni::String& folder) :
 LoadingError(std::string("Study folder") + folder.c_str() + " does not exist.")
{
}

ReadingStudy::ReadingStudy() : LoadingError("Got a fatal error reading the study.")
{
}

NoAreas::NoAreas() : LoadingError("No area found. A valid study contains contains at least one.")
{
}

InvalidFileName::InvalidFileName() : LoadingError("Invalid file names detected.")
{
}

RuntimeInfoInitialization::RuntimeInfoInitialization() :
 LoadingError("Error initializing runtime infos.")
{
}

WritingPID::WritingPID(const Yuni::String& file) :
 LoadingError(std::string("Impossible to write pid file ") + file.c_str())
{
}

InvalidOptimizationRange::InvalidOptimizationRange() :
 LoadingError("Invalid command line value for --optimization-range ('day' or 'week' expected)")
{
}

InvalidSimulationMode::InvalidSimulationMode() :
 LoadingError(
   "Only one simulation mode is allowed: --expansion, --economy, --adequacy or --adequacy-draft")
{
}

InvalidSolver::InvalidSolver(const std::string& solver) :
 LoadingError("Can't use solver '" + solver + "' : it does not match any available OR-Tools solver")
{
}

InvalidStudy::InvalidStudy(const Yuni::String& study) :
 LoadingError(std::string("The folder `") + study.c_str() + "` does not seem to be a valid study")
{
}

InvalidVersion::InvalidVersion(const char* version, const char* latest) :
 LoadingError(std::string("Invalid version for the study : found `") + version + "`, expected <=`"
              + latest + '`')
{
}

NoStudyProvided::NoStudyProvided() :
 LoadingError("A study folder is required. Use '--help' for more information")
{
}

InvalidNumberOfMCYears::InvalidNumberOfMCYears(uint nbYears) :
 LoadingError("Invalid number of MC years, got " + std::to_string(nbYears) + ", expected <= 50000")
{
}

IncompatibleParallelOptions::IncompatibleParallelOptions() :
 LoadingError("Options --parallel and --force-parallel are incompatible")
{
}

IncompatibleOptRangeHydroPricing::IncompatibleOptRangeHydroPricing() :
 LoadingError("Simplex optimization range and hydro pricing mode : values are not compatible ")
{
}

IncompatibleOptRangeUCMode::IncompatibleOptRangeUCMode() :
 LoadingError("Simplexe optimization range and unit commitment mode : values are not compatible")
{
}

IncompatibleDailyOptHeuristicForArea::IncompatibleDailyOptHeuristicForArea(
  const Antares::Data::AreaName& name) :
 LoadingError(std::string("Area ") + name.c_str()
              + " : simplex daily optimization and use heuristic target == no are not compatible")
{
}

WritingProgressFile::WritingProgressFile(const Yuni::String& file) :
 LoadingError(std::string("I/O error: impossible to write ") + file.c_str())
{
}

} // namespace Error
} // namespace Antares
