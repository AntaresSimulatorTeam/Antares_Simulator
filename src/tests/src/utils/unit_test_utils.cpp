#include "unit_test_utils.h"

#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

#include <antares/logs/logs.h>

std::function<bool(const std::exception&)> checkMessage(std::string expected_message)
{
    return [expected_message](const std::exception& e)
    {
        BOOST_CHECK_EQUAL(e.what(), expected_message);
        return true;
    };
}

std::function<bool(const std::exception&)> containsMessage(std::string expected_message)
{
    return [expected_message](const std::exception& e)
    {
        std::string raised_msg = e.what();
        BOOST_CHECK(raised_msg.find(expected_message) != std::string::npos);
        return true;
    };
}

namespace Antares::UnitTests
{
CaptureAntaresLogs::CaptureAntaresLogs()
{
    logs.callback.connect(this, &CaptureAntaresLogs::onLogMessage);
}

CaptureAntaresLogs::~CaptureAntaresLogs()
{
    logs.callback.clear();
    destroyBoundEvents();
}

void CaptureAntaresLogs::onLogMessage(int level, const std::string& message)
{
    switch (level)
    {
        using namespace Yuni::Logs::Verbosity;
    case Warning::level:
        warnings_.insert(message);
        break;
    case Error::level:
        errors_.insert(message);
        break;
    case Fatal::level:
        fatals_.insert(message);
        break;
    case Info::level:
        infos_.insert(message);
    default:
        break;
    }
}

const std::set<std::string>& CaptureAntaresLogs::getFatals() const
{
    return fatals_;
}

const std::set<std::string>& CaptureAntaresLogs::getErrors() const
{
    return errors_;
}

const std::set<std::string>& CaptureAntaresLogs::getWarnings() const
{
    return warnings_;
}

const std::set<std::string>& CaptureAntaresLogs::getInfos() const
{
    return infos_;
}
} // namespace Antares::UnitTests
