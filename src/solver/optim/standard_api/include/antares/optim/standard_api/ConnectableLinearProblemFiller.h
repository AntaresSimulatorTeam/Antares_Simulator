#pragma once

#include "antares/optim/api/LinearProblemFiller.h"

// TODO: merge this into LinearProblemFiller API ?
namespace Antares::optim::standard_api
{
    class ConnectableLinearProblemFiller : public Antares::optim::api::LinearProblemFiller
    {
    public:
        [[nodiscard]] virtual std::map<std::string, double> getPortPin(std::string name, int timestamp,
                                                         const Antares::optim::api::LinearProblemData &linearProblemData) const = 0;
    };
}
