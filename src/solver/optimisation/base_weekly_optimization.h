#pragma once

namespace Antares
{
namespace Solver
{
namespace Simulation
{
    class interfaceWeeklyOptimization
    {
    public:
        virtual void solve(uint weekInTheYear, int hourInTheYear) = 0;
        virtual void solveCSR(Antares::Data::AreaList& areas, uint year, uint week, uint numSpace) {};
        static std::unique_ptr<interfaceWeeklyOptimization> create(bool adqPatchEnabled,
            PROBLEME_HEBDO* problemesHebdo,
            uint numSpace);

    protected:
        explicit interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo, uint numSpace);
        PROBLEME_HEBDO* problemeHebdo_ = nullptr;
        uint thread_number_ = 0;
    };
} // namespace Simulation
} // namespace Solver
} // namespace Antares
