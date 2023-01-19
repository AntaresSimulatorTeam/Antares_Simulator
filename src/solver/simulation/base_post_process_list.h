#pragma once

#include <vector>

namespace Antares::Solver::Simulation
{

struct optRunimeData
{
    unsigned int year = 0;
    unsigned int week = 0;
    unsigned int hourInTheYear = 0;
};

class basePostProcessCommand
{
public:
    basePostProcessCommand() = default;
    virtual ~basePostProcessCommand() = default;
    virtual void acquireOptRuntimeData(const struct optRunimeData& opt_runtime_data) = 0;
    virtual void run() = 0;
};

class interfacePostProcessList
{
public:
    interfacePostProcessList() = default;
    virtual ~interfacePostProcessList() = default;
    void runAll(const struct optRunimeData& opt_runtime_data)
    {
        for (auto postProcess : post_process_list)
        {
            postProcess.acquireOptRuntimeData(opt_runtime_data);
            postProcess.run();
        }
    }

protected:
    /*
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    const uint thread_number_ = 0;
    */
    std::vector<basePostProcessCommand> post_process_list;
};

} // namespace Antares::Solver::Simulation