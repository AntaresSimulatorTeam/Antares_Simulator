#include "applicationname.h"

namespace Yuni::Logs
{
std::optional<int>& threadNumber()
{
    static thread_local std::optional<int> tnum;
    return tnum;
}
} // namespace Yuni::Logs
