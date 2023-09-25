#pragma once

#include <vector>
#include <memory>

#include "../simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data;
using namespace Antares::Date;
using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

namespace Antares::Solver::Simulation
{


struct optRuntimeData
{
    optRuntimeData(unsigned int y, unsigned int w, unsigned int h) :
        year(y), week(w), hourInTheYear(h)
    {}

    unsigned int year = 0;
    unsigned int week = 0;
    unsigned int hourInTheYear = 0;
};

class basePostProcessCommand
{
public:
    virtual ~basePostProcessCommand() = default;
    virtual void execute(const optRuntimeData& opt_runtime_data) = 0;

protected:
    // Memeber functions
    basePostProcessCommand(PROBLEME_HEBDO* problemeHebdo);

    // Data members
    PROBLEME_HEBDO* problemeHebdo_;
};

class interfacePostProcessList
{
public:
    virtual ~interfacePostProcessList() = default;

    // gp : we should put all these arguments in a structure, so that we pass a structure
    // gp : to the create(...) method, and to underlying calls to constructors.
    // gp : In case we need new data for a new post process, we would not have to change
    // gp : the constructors' signatures of the post process list classes. 
    static std::unique_ptr<interfacePostProcessList> create(AdqPatchParams& adqPatchParams,
                                                            PROBLEME_HEBDO* problemeHebdo,
                                                            uint thread_number,
                                                            AreaList& areas,
                                                            SheddingPolicy sheddingPolicy,
                                                            SimplexOptimization splxOptimization,
                                                            Calendar& calendar);
    void runAll(const optRuntimeData& opt_runtime_data);
    
protected:
    // Member functions
    interfacePostProcessList(PROBLEME_HEBDO* problemeHebdo, uint thread_number);

    // Data mambers
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    const unsigned int thread_number_ = 0;
    std::vector<std::unique_ptr<basePostProcessCommand>> post_process_list;
};

} // namespace Antares::Solver::Simulation