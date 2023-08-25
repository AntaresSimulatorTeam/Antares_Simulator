#pragma once

#include <map>
#include <string>

#include <antares/writer/i_writer.h>

#include "ortools/linear_solver/linear_solver.h"

#include "ortools_wrapper.h"

using namespace operations_research;

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver,
                                                   Antares::Solver::IResultWriter& writer,
                                                   const std::string& filename);

/*!
 *  \brief Return list of available ortools solver name on our side
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverName();

/*!
 *  \brief Create a MPSolver with correct linear or mixed variant
 *
 *  \return MPSolver
 */
MPSolver* MPSolverFactory(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* probleme,
                          const std::string& solverName);

std::string generateTempPath(const std::string& filename);
void removeTemporaryFile(const std::string& tmpPath);

class OrtoolsUtils
{
public:
    struct SolverNames
    {
        std::string LPSolverName, MIPSolverName;
    };
    static const std::map<std::string, struct SolverNames> solverMap;
};

namespace Antares
{
namespace Optimization
{

class Nomenclature
{
public:
    Nomenclature() = delete;

    explicit Nomenclature(char prefix) : prefix_(prefix)
    {
    }

    void SetTarget(const std::vector<std::string>& target)
    {
        target_ = &target;
    }

    std::string GetName(unsigned index) const
    {
        if (target_ == nullptr || target_->at(index).empty())
        {
            return prefix_ + std::to_string(index);
        }
        return target_->at(index);
    }

private:
    const std::vector<std::string>* target_ = nullptr;
    char prefix_;
};

class ProblemSimplexeNommeConverter
{
public:
    explicit ProblemSimplexeNommeConverter(
      const std::string& solverName,
      const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);

    MPSolver* Convert();

private:
    const std::string& solverName_;
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;
    Nomenclature variableNameManager_ = Nomenclature('x');
    Nomenclature constraintNameManager_ = Nomenclature('c');

    void UpdateCoefficient(unsigned idxVar, MPSolver* solver_, MPObjective* const objective_);
    void CopyVariables(MPSolver* solver_);
    void UpdateContraints(unsigned idxRow, MPSolver* solver_);
    void CopyRows(MPSolver* solver_);
    void TuneSolverSpecificOptions(MPSolver* solver) const;
    void CopyMatrix(const MPSolver* solver);
};
} // namespace Optimization
} // namespace Antares
