#pragma once

#include "vector"
#include "string"

using namespace std;

class ComponentFiller;

struct ComponentAndPort {
    shared_ptr<ComponentFiller> componentFiller;
    string portName;
};

class PortConnection
{
private:
    ComponentAndPort componentAndPort1; // TODO : à terme, ne pas référencer directement des fillers
    ComponentAndPort componentAndPort2;
public:
    PortConnection(ComponentAndPort componentAndPort1, ComponentAndPort componentAndPort2)
            : componentAndPort1(std::move(componentAndPort1)), componentAndPort2(std::move(componentAndPort2))
    {}
    [[nodiscard]] ComponentAndPort getComponentAndPort1() const { return componentAndPort1; }
    [[nodiscard]] ComponentAndPort getComponentAndPort2() const { return componentAndPort2; }
};
class PortConnectionsManager
{
private:
    vector<PortConnection> connections;
public:
    void addConnection(ComponentAndPort componentAndPort1, ComponentAndPort componentAndPort2)
    {
        connections.emplace_back(std::move(componentAndPort1), std::move(componentAndPort2));
    }
    vector<ComponentAndPort> getConectionsTo(ComponentFiller* componentFiller, const string& portId)
    {
        // TODO: implement a better search algorithm
        vector<ComponentAndPort> connectedComponents;
        for (const auto& c: connections) {
            if (c.getComponentAndPort1().componentFiller.get() == componentFiller
                && c.getComponentAndPort1().portName == portId) {
                connectedComponents.emplace_back(c.getComponentAndPort2());
            } else if (c.getComponentAndPort2().componentFiller.get() == componentFiller
                       && c.getComponentAndPort2().portName == portId) {
                connectedComponents.emplace_back(c.getComponentAndPort1());
            }
        }
        return connectedComponents;
    }
};
