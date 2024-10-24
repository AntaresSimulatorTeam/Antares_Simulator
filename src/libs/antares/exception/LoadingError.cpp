#include "antares/exception/LoadingError.hpp"
#include <sstream>

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
 LoadingError("Only one simulation mode is allowed: --expansion, --economy, --adequacy")
{
}

static std::string InvalidSolverHelper(const std::string& solver, const std::string& availableSolvers)
{
  std::ostringstream message;
  message
    << "Can't use solver '"
    << solver
    << "' : it does not match any available OR-Tools solver. Possible choices are "
    << availableSolvers;
  return message.str();
}

InvalidSolver::InvalidSolver(const std::string& solver, const std::string& availableSolvers) : LoadingError(InvalidSolverHelper(solver, availableSolvers))
{
}

InvalidStudy::InvalidStudy(const std::string& study) :
 LoadingError(std::string("The folder `") + study + "` does not seem to be a valid study")
{
}

InvalidVersion::InvalidVersion(const char* version, const char* latest) :
 LoadingError(std::string("Invalid version for the study : found `") + version + "`, expected <=`"
              + latest + '`')
{}

static std::string InvalidSolverSpecificParametersHelper(const std::string& solver,
                                                         const std::string& specificParameters)
{
    std::ostringstream message;
    message << "Specific parameters '" << specificParameters
            << "' are not valid or not supported for solver " << solver;
    return message.str();
}

InvalidSolverSpecificParameters::InvalidSolverSpecificParameters(
  const std::string& solver,
  const std::string& specificParameters):
    LoadingError(InvalidSolverSpecificParametersHelper(solver, specificParameters))
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

IncompatibleMILPWithoutOrtools::IncompatibleMILPWithoutOrtools() :
 LoadingError("Unit Commitment mode 'milp' must be used with an OR-Tools solver ")
{
}

IncompatibleMILPOrtoolsSolver::IncompatibleMILPOrtoolsSolver() :
 LoadingError("'milp' mode does not work with OR-Tools using Sirius solver")
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

std::string InvalidParametersForThermalClusters::buildMessage(
  const std::map<int, Yuni::String>& clusterNames) const
{
    const std::string startMessage("Conflict between Min Stable Power, Pnom, spinning and capacity "
                                   "modulation for the following clusters : ");
    std::string clusters;
    for (const auto& it : clusterNames)
    {
        clusters += it.second.c_str();
        clusters += ";";
    }
    if (!clusters.empty())
        clusters.pop_back(); // Remove final semicolon
    return startMessage + clusters;
}

InvalidParametersForThermalClusters::InvalidParametersForThermalClusters(
  const std::map<int, Yuni::String>& clusterNames) :
 LoadingError(buildMessage(clusterNames))
{
}

CommandLineArguments::CommandLineArguments(uint errors) :
 LoadingError("Invalid command-line arguments provided : " + std::to_string(errors)
              + " error(s) found")
{
}

IncompatibleSimulationModeForAdqPatch::IncompatibleSimulationModeForAdqPatch() :
 LoadingError("Adequacy Patch can only be used with Economy Simulation Mode")
{
}

NoAreaInsideAdqPatchMode::NoAreaInsideAdqPatchMode() :
 LoadingError("Minimum one area must be inside adequacy patch mode when using adequacy patch")
{
}

IncompatibleHurdleCostCSR::IncompatibleHurdleCostCSR() :
 LoadingError("Incompatible options include.hurdleCost and curtailmentSharing.includeHurdleCost")
{
}

AdqPatchDisabledLMR::AdqPatchDisabledLMR() :
 LoadingError("Incompatible options LMR disabled and priceTakingOrder equal Dens")
{
}

IncompatibleOutputOptions::IncompatibleOutputOptions(const std::string& text) : LoadingError(text)
{
}

IncompatibleCO2CostColumns::IncompatibleCO2CostColumns() :
 LoadingError("Number of columns for CO2 Cost can be one or same as number of TS in Availability")
{
}

IncompatibleFuelCostColumns::IncompatibleFuelCostColumns() :
 LoadingError("Number of columns for Fuel Cost can be one or same as number of TS in Availability")
{
}

} // namespace Error
} // namespace Antares
