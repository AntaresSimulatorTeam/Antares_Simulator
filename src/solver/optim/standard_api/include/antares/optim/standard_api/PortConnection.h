#pragma once

#include "vector"
#include "string"
#include "ConnectableLinearProblemFiller.h"

// TODO: move this to optim::api?
namespace Antares::optim::standard_api
{
    struct FillerAndPort
    {
        std::shared_ptr<ConnectableLinearProblemFiller> connectableFiller;
        std::string portName;
    };

    class PortConnection
    {
    private:
        FillerAndPort fillerAndPort1; // TODO : à terme, ne pas référencer directement des fillers
        FillerAndPort fillerAndPort2;
    public:
        PortConnection(FillerAndPort fillerAndPort1, FillerAndPort fillerAndPort2)
                : fillerAndPort1(std::move(fillerAndPort1)), fillerAndPort2(std::move(fillerAndPort2))
        {}

        [[nodiscard]] FillerAndPort getFillerAndPort1() const
        { return fillerAndPort1; }

        [[nodiscard]] FillerAndPort getFillerAndPort2() const
        { return fillerAndPort2; }
    };

    class PortConnectionsManager
    {
    private:
        std::vector<PortConnection> connections;
    public:
        void addConnection(FillerAndPort fillerAndPort1, FillerAndPort fillerAndPort2)
        {
            connections.emplace_back(std::move(fillerAndPort1), std::move(fillerAndPort2));
        }

        std::vector<FillerAndPort>
        getConnectionsTo(ConnectableLinearProblemFiller *connectableFiller, const std::string &portId)
        {
            // TODO: implement a better search algorithm
            std::vector<FillerAndPort> connectedComponents;
            for (const auto &c: connections) {
                if (c.getFillerAndPort1().connectableFiller.get() == connectableFiller
                    && c.getFillerAndPort1().portName == portId) {
                    connectedComponents.emplace_back(c.getFillerAndPort2());
                } else if (c.getFillerAndPort2().connectableFiller.get() == connectableFiller
                           && c.getFillerAndPort2().portName == portId) {
                    connectedComponents.emplace_back(c.getFillerAndPort1());
                }
            }
            return connectedComponents;
        }
    };
}