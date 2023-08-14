#include <antares/logs/logs.h>
#include <sstream>

namespace Antares::Solver
{
void WriteCommandLineIntoLogs(int argc, char** argv)
{
    std::ostringstream buffer;
    for (int arg = 0; arg < argc; ++arg)
    {
        buffer << argv[arg];
        if (arg + 1 != argc)
            buffer << " ";
    }
    logs.info() << "  :: command: " << buffer.str();
}
} // namespace Antares::Solver
