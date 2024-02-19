#pragma once

#include "vector"
#include "string"

using namespace std;

class ComponentFiller;
class PortConnection
{
private:
    pair<shared_ptr<ComponentFiller>, string> componentAndPort1; // TODO : à terme, ne pas référencer directement des fillers
    pair<shared_ptr<ComponentFiller>, string> componentAndPort2;
public:
    PortConnection(pair<shared_ptr<ComponentFiller>, string> componentAndPort1, pair<shared_ptr<ComponentFiller>, string> componentAndPort2)
            : componentAndPort1(std::move(componentAndPort1)), componentAndPort2(std::move(componentAndPort2))
    {}
    [[nodiscard]] pair<shared_ptr<ComponentFiller>, string> getComponentAndPort1() const { return componentAndPort1; }
    [[nodiscard]] pair<shared_ptr<ComponentFiller>, string> getComponentAndPort2() const { return componentAndPort2; }
};
class PortConnectionsManager
{
private:
    vector<PortConnection> connections;
public:
    void addConnection(pair<shared_ptr<ComponentFiller>, string> componentAndPort1, pair<shared_ptr<ComponentFiller>, string> componentAndPort2)
    {
        connections.emplace_back(std::move(componentAndPort1), std::move(componentAndPort2));
    }
    vector<pair<shared_ptr<ComponentFiller>, string>> getConectionsTo(ComponentFiller* componentFiller, const string& portId)
    {
        // TODO: implement a better search algorithm
        vector<pair<shared_ptr<ComponentFiller>, string>> connectedComponents;
        for (const auto& c: connections) {
            if (c.getComponentAndPort1().first.get() == componentFiller
                && c.getComponentAndPort1().second == portId) {
                connectedComponents.emplace_back(c.getComponentAndPort2());
            } else if (c.getComponentAndPort2().first.get() == componentFiller
                       && c.getComponentAndPort2().second == portId) {
                connectedComponents.emplace_back(c.getComponentAndPort1());
            }
        }
        return connectedComponents;
    }
};
