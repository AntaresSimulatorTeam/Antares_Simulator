#pragma once

// TODO: merge this into LinearProblemFiller API ?
namespace Antares::optim::standard_api
{
    class PortHolder
    {
    public:
        // Returns the linear expression of a port's pin
        // port pin = sum(keys * values), key being the ID of a variable in the MIP
        [[nodiscard]] virtual std::map<std::string, double> getPortPin(std::string name, int timestamp,
                                                         const Antares::optim::api::LinearProblemData &linearProblemData) const = 0;
    };
}
