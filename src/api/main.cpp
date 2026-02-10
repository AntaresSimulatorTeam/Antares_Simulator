

#include <antares/logs/logs.h>
#include "antares/api/singleProblemGetter.h"

using namespace Antares::Solver;

int main(const int argc, const char** argv)
{
    if (argc < 2)
    {
        logs.error() << "Please provide valid study path.";
        logs.error() << "Usage:" << argv[0] << "/path/to/study";
        return 1;
    }

    const SingleProblemGetter getter(argv[1]);
    getter.printProblems();
    return 0;
}
