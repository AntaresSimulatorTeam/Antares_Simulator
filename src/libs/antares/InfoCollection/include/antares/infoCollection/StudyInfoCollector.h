//
// Created by marechaljas on 22/08/23.
//

#pragma once

#include "antares/study/study.h"
#include <sstream>

namespace Benchmarking {
class FileContent;

class StudyInfoCollector
{
public:
    StudyInfoCollector(const Antares::Data::Study& study) : study_(study)
    {
    }
    void toFileContent(FileContent& file_content);

private:
    // Methods
    void areasCountToFileContent(FileContent& file_content);
    void linksCountToFileContent(FileContent& file_content);
    void performedYearsCountToFileContent(FileContent& file_content);
    void enabledThermalClustersCountToFileContent(FileContent& file_content);
    void enabledBindingConstraintsCountToFileContent(FileContent& file_content);
    void unitCommitmentModeToFileContent(FileContent& file_content);
    void maxNbYearsInParallelToFileContent(FileContent& file_content);
    void solverVersionToFileContent(FileContent& file_content);

    void ORToolsUsed(FileContent& file_content);
    void ORToolsSolver(FileContent& file_content);

    // Member data
    const Antares::Data::Study& study_;
};


struct OptimizationInfo
{
    unsigned int nbVariables = 0;
    unsigned int nbConstraints = 0;
    unsigned int nbNonZeroCoeffs = 0;
};

class SimulationInfoCollector
{
public:
    SimulationInfoCollector(const OptimizationInfo& optInfo) : opt_info_(optInfo){};

    void toFileContent(FileContent& file_content);

private:
    const OptimizationInfo& opt_info_;
};

class ICustomBenchmarkData
{
public:
    ICustomBenchmarkData(const std::string& name, const std::string& unit) : name(name), unit(unit)
    {
    }
    std::string Name() const
    {
        return name;
    }
    std::string Unit() const
    {
        return unit;
    }
    virtual std::string Value() const = 0;

private:
    std::string name;
    std::string unit;
};

template<typename T>
class CustomBenchmarkData : public ICustomBenchmarkData
{
public:
    CustomBenchmarkData(const std::string& name, const std::string& unit, const T& value) :
     ICustomBenchmarkData(name, unit), value(value)
    {
    }

    std::string Value() const override
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

private:
    T value;
};
class CustomBenchmarkAgregator
{
public:
    CustomBenchmarkAgregator() = default;
    void AddBenchmark(ICustomBenchmarkData* data)
    {
        if (data)
        {
            std::ostringstream oss;
            oss << "{\n"
                << "\"name\": " << data->Name() << ",\n"
                << "\"value\": " << data->Value() << ",\n"
                << "\"unit\": " << data->Unit() << "\n"
                << "}\n";

            if (!result.empty())
            {
                result += ",\n";
            }
            result += oss.str();
        }
    }
    std::string Result() const
    {
        return result;
    }

private:
    std::string result;
};
}