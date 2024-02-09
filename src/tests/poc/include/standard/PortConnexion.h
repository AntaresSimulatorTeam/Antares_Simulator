#pragma once

#include "vector"
#include "string"

using namespace std;

class ComponentFiller;
class PortConnexion
{
private:
    pair<ComponentFiller*, string> componentAndPort1; // TODO : à terme, ne pas référencer directement des fillers
    pair<ComponentFiller*, string> componentAndPort2;
public:
    PortConnexion(pair<ComponentFiller*, string> componentAndPort1, pair<ComponentFiller*, string> componentAndPort2)
            : componentAndPort1(std::move(componentAndPort1)), componentAndPort2(std::move(componentAndPort2))
    {}
    [[nodiscard]] pair<ComponentFiller*, string> getComponentAndPort1() const { return componentAndPort1; }
    [[nodiscard]] pair<ComponentFiller*, string> getComponentAndPort2() const { return componentAndPort2; }
};
class PortConnexionsManager
{
private:
    vector<PortConnexion> connexions;
public:
    void addConnexion(pair<ComponentFiller*, string> componentAndPort1, pair<ComponentFiller*, string> componentAndPort2)
    {
        connexions.emplace_back(std::move(componentAndPort1), std::move(componentAndPort2));
    }
    vector<pair<ComponentFiller*, string>> getConexionsTo(ComponentFiller* componentFiller, const string& portId)
    {
        vector<pair<ComponentFiller*, string>> connectedComponents;
        for (const auto& c: connexions) {
            if (c.getComponentAndPort1().first == componentFiller
                && c.getComponentAndPort1().second == portId) {
                connectedComponents.emplace_back(c.getComponentAndPort2());
            } else if (c.getComponentAndPort2().first == componentFiller
                       && c.getComponentAndPort2().second == portId) {
                connectedComponents.emplace_back(c.getComponentAndPort1());
            }
        }
        return connectedComponents;
    }
};
