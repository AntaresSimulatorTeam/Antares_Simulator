#include <antares/logs.h>
#include <memory>

namespace Antares {
  namespace MemoryUtils {
    template<class T, class... Args>
    T* new_check_allocation(Args... args)
    {
      auto r = new (std::nothrow) T(args...);
      if (r == nullptr)
        {
          logs.error() << "Error allocating memory";
        }
      return r;
    }
  }
}
