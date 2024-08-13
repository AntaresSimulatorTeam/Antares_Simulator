#include <antares/solver/expressions/visitors/LinearStatus.h>

namespace Antares::Solver::Visitors
{
constexpr LinearStatus::LinearStatus(const Status& status):
    status_(status)
{
}

constexpr LinearStatus LinearStatus::operator*(const LinearStatus& other)
{
    switch (other)
    {
    case LinearStatus::NON_LINEAR:
        return LinearStatus::NON_LINEAR;
    case LinearStatus::CONSTANT:
        return *this;
    case LinearStatus::LINEAR:
        if (status_ == LinearStatus::CONSTANT)
        {
            return other;
        }
        else
        {
            return LinearStatus::NON_LINEAR;
        }
    };
}

constexpr LinearStatus LinearStatus::operator/(const LinearStatus& other)
{
    switch (other)
    {
    case LinearStatus::NON_LINEAR:
    case LinearStatus::LINEAR:
        return LinearStatus::NON_LINEAR;
    case LinearStatus::CONSTANT:
        return *this;
    };
}

constexpr LinearStatus LinearStatus::operator+(const LinearStatus& other)
{
    switch (other)
    {
    case LinearStatus::NON_LINEAR:
        return LinearStatus::NON_LINEAR;
    case LinearStatus::CONSTANT:
        return *this;
    case LinearStatus::LINEAR:
        if (other == LinearStatus::CONSTANT || other == LinearStatus::LINEAR)
        {
            return other;
        }
        else
        {
            return LinearStatus::NON_LINEAR;
        }
    };
}

constexpr LinearStatus LinearStatus::operator-(const LinearStatus& other)
{
    return operator+(other);
}
} // namespace Antares::Solver::Visitors
