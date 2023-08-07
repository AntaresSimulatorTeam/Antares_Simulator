#include <antares/logs.h>
#include <sstream>

void WriteCommandLineIntoLogs(int argc, char** argv)
{
  std::ostringstream buffer;
  for (int arg = 0; arg < argc; arg++) {
      buffer << argv[arg];
      if (arg + 1 != argc)
          buffer << " ";
  }
  Antares::logs.info() << "  :: command: " << buffer.str();
}
