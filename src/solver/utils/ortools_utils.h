#pragma once

#include <map>
#include <string>

#include <antares/writer/i_writer.h>
#include "named_problem.h"
#include "ortools/linear_solver/linear_solver.h"

using operations_research::MPSolver, operations_research::MPObjective;

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
                          const std::string& solverName,
                          bool MIP);

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

namespace Antares::Optimization
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
    const char prefix_;
};

class IMixedIntegerProblemManager {
public:
  virtual bool visit(int idxVar) const = 0;
  virtual bool isMIP() const = 0;
  static std::unique_ptr<IMixedIntegerProblemManager> factory(bool integer,
                                                              const std::vector<bool>& integerVariable);
};

class Relaxation : public IMixedIntegerProblemManager
{
public:
  bool visit(int idxVar) const override;
  bool isMIP() const override;
};

class MILP : public IMixedIntegerProblemManager
{
public:
  MILP(const std::vector<bool>& integerVariable);
  bool visit(int idxVar) const override;
  bool isMIP() const override;
private:
  const std::vector<bool>& integerVariable;
};

class ProblemSimplexeNommeConverter
{
public:
    explicit ProblemSimplexeNommeConverter(
      const std::string& solverName,
      const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);

    MPSolver* Convert(const IMixedIntegerProblemManager&);

private:
    const std::string& solverName_;
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;
    Nomenclature variableNameManager_ = Nomenclature('x');
    Nomenclature constraintNameManager_ = Nomenclature('c');

    void CreateVariable(unsigned idxVar,
                        MPSolver* solver,
                        MPObjective* const objective,
                        bool integerVar);
    void CopyVariables(MPSolver* solver, const IMixedIntegerProblemManager&);
    void UpdateContraints(unsigned idxRow, MPSolver* solver);
    void CopyConstraints(MPSolver* solver);
    void TuneSolverSpecificOptions(MPSolver* solver) const;
    void CopyMatrix(const MPSolver* solver);
};
} // namespace Optimization::Antares
