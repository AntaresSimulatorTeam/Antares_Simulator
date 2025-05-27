#pragma once

#include <string>

namespace Antares::Optimisation::LinearProblemDataImpl {
    class IScenario {
    public:
        using Year = unsigned;
        using Chronicle = unsigned;
        virtual ~IScenario() = default;

        explicit IScenario(std::string group): group_(std::move(group)) {
        }

        virtual Chronicle getData(Year year) = 0;

        [[nodiscard]] std::string group() const {
            return group_;
        }

    private:
        std::string group_;
    };
} // namespace Antares::Optimisation::LinearProblemDataImpl
