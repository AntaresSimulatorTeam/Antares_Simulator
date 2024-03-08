#pragma once

#include "string"
#include "map"

using namespace std;

enum Model
{
    THERMAL,
    BATTERY,
    BALANCE,
    PRICE_MINIM
};
class Component
{
private:
    string id_;
    Model model_;
    map<string, double> doubleParameterValues_;
    map<string, string> stringParameterValues_;
public:
    Component(string id, Model model, map<string, double> doubleParameterValues,
              map<string, string> stringParameterValues) : id_(std::move(id)), model_(model),
                                                           doubleParameterValues_(std::move(doubleParameterValues)),
                                                           stringParameterValues_(std::move(stringParameterValues))
    {}
    [[nodiscard]] string getId() const { return id_; }
    [[nodiscard]] Model getModel() const { return model_; }
    [[nodiscard]] double getDoubleParameterValue(const string& key) const { return doubleParameterValues_.at(key); }
    [[nodiscard]] string getStringParameterValue(const string& key) const { return stringParameterValues_.at(key); }
};