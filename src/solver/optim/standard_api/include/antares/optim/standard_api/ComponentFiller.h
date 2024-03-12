#pragma once

#include "Component.h"
#include "PortConnection.h"

namespace Antares::optim::standard_api
{
    class ComponentFiller : public ConnectableLinearProblemFiller
    {
    private:
        PortConnectionsManager *portConnectionsManager_;
        Component component_;
    public:
        ComponentFiller(Component component, PortConnectionsManager &portConnectionsManager) :
                portConnectionsManager_(&portConnectionsManager), component_(std::move(component))
        {}

        void addVariables(Antares::optim::api::LinearProblem &problem, const Antares::optim::api::LinearProblemData &data) override;

        void addConstraints(Antares::optim::api::LinearProblem &problem, const Antares::optim::api::LinearProblemData &data) override;

        void addObjective(Antares::optim::api::LinearProblem &problem, const Antares::optim::api::LinearProblemData &data) override;

        void update(Antares::optim::api::LinearProblem &problem, const Antares::optim::api::LinearProblemData &data) override;

        [[nodiscard]] std::map<std::string, double>
        getPortPin(std::string name, int timestamp, const Antares::optim::api::LinearProblemData &linearProblemData) const;
    };
}