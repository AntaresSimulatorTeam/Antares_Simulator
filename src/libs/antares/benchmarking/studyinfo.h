#pragma once

#include <antares/study.h>

namespace Benchmarking
{
/*
    ===  Study info item ===
*/
class StudyInfoItem
{
public:
    StudyInfoItem(std::string name) : name_(name) 
    {}
    virtual std::string name() { return name_; }
    virtual std::string value() = 0;
protected:
    std::string name_ = "";
};

class StudyInfoItem_integerValue : public StudyInfoItem
{
public:
    StudyInfoItem_integerValue(std::string name, unsigned int value) : StudyInfoItem(name), value_(value)
    {}
    std::string value() override { return std::to_string(value_); }
private:
    unsigned int value_ = 0;
};

class StudyInfoItem_charValue : public StudyInfoItem
{
public:
    StudyInfoItem_charValue(std::string name, const char* value) :
     StudyInfoItem(name), value_(value)
    {
    }
    std::string value() override
    {
        return value_;
    }

private:
    const char* value_ = "";
};

struct OptimizationInfo
{
    unsigned int nbVariables = 0;
    unsigned int nbConstraints = 0;
    unsigned int nbNonZeroCoeffs = 0;
};

/*
    === Info collectors ===
*/

class FileContent; // Forward declaration

class StudyInfoCollector
{
public:
    StudyInfoCollector(const Antares::Data::Study& study, FileContent& file_content)
        : study_(study), file_content_(file_content)
    {}
    void collect();
private:
    // Methods
    void collectAreasCount();
    void collectLinksCount();
    void collectPerformedYearsCount();
    void collectEnabledThermalClustersCount();
    void collectEnabledBindingConstraintsCount();
    void collectUnitCommitmentMode();
    void collectMaxNbYearsInParallel();
    void collectSolverVersion();

    // Member data
    FileContent& file_content_;
    const Antares::Data::Study& study_;

    // ----------------------------------------------------------------------------------------
    // TODO : add some more pieces of information about study : see following data members.
    // ----------------------------------------------------------------------------------------
    // Parallel execution
    unsigned int minNbYearsInParallel_ = 0;
    unsigned int nbCoreMode_ = 0;
};

class SimulationInfoCollector
{
public:
    SimulationInfoCollector(const OptimizationInfo& optInfo, FileContent& file_content)
        : opt_info_(optInfo), file_content_(file_content)
    {};

    void collect();

private:
    FileContent& file_content_;
    const OptimizationInfo& opt_info_;
};

/*
    === class FileContent ===
*/
class FileContent
{
public:
    FileContent() = default;

    using iterator = std::vector<std::unique_ptr<StudyInfoItem>>::iterator;
    iterator begin();
    iterator end();

    void addItem(StudyInfoItem* item)
    {
        items_.emplace_back(item);
    }

private:
    // List of study's information pieces (or items)
    std::vector<std::unique_ptr<StudyInfoItem>> items_;
};

/*
    === class FileWriter ===
*/

class FileWriter
{
public:   
    FileWriter(FileContent& fileContent) : fileContent_(fileContent) {}
    virtual void flush() = 0;

protected:
    // Member data
    FileContent& fileContent_;
};

class FileCSVwriter final : public FileWriter
{
public:
    explicit FileCSVwriter(Yuni::String& filePath, FileContent& fileContent) 
        : FileWriter(fileContent), filePath_(filePath)
    {}
    void flush() override;
private:
    Yuni::IO::File::Stream outputFile_;
    Yuni::String& filePath_;
};

} // namespace Benchmarking
