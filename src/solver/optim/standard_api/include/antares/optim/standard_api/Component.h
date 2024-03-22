#pragma once

#include "string"
#include "map"

namespace Antares::optim::standard_api
{
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
        std::string id_;
        Model model_;
        std::map<std::string, double> doubleParameterValues_;
        std::map<std::string, std::string> stringParameterValues_;
    public:
        Component(std::string id, Model model, std::map<std::string, double> doubleParameterValues,
                  std::map<std::string, std::string> stringParameterValues) : id_(std::move(id)), model_(model),
                                                               doubleParameterValues_(std::move(doubleParameterValues)),
                                                               stringParameterValues_(std::move(stringParameterValues))
        {}

        [[nodiscard]] std::string getId() const
        { return id_; }

        [[nodiscard]] Model getModel() const
        { return model_; }

        [[nodiscard]] double getDoubleParameterValue(const std::string &key) const
        { return doubleParameterValues_.at(key); }

        [[nodiscard]] std::string getStringParameterValue(const std::string &key) const
        { return stringParameterValues_.at(key); }
    };
}