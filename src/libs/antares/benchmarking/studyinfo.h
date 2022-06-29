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
    === class StudyInfoContainer ===
*/
class StudyInfoContainer
{
public:
    StudyInfoContainer(const Antares::Data::Study& study, const OptimizationInfo& optInfo);

    using iterator = std::vector<std::unique_ptr<StudyInfoItem>>::iterator;
    iterator begin();
    iterator end();

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
    void collectOptimizationInfo();
    void collectSolverVersion();

    template<class ClassItem, typename T>
    void addItem(std::string name, T value)
    {
        items_.emplace_back(new ClassItem(name, value));
    }


    // Member data
    const Antares::Data::Study& study_;

    // ----------------------------------------------------------------------------------------
    // TODO : add some more pieces of information about study : see following data members.
    // ----------------------------------------------------------------------------------------
    // Parallel execution
    unsigned int minNbYearsInParallel_ = 0;
    unsigned int nbCoreMode_ = 0;

    // Optimization problem
    const OptimizationInfo& pOptimizationInfo;
    // List of study's information pieces (or items)
    std::vector<std::unique_ptr<StudyInfoItem>> items_;
};

/*
    === class StudyInfoWriter ===
*/

class StudyInfoWriter
{
public:   
    StudyInfoWriter(StudyInfoContainer& fileContent);
    virtual void flush() = 0;

protected:
    // Member data
    StudyInfoContainer& fileContent_;
};

class StudyInfoCSVwriter final : public StudyInfoWriter
{
public:
    explicit StudyInfoCSVwriter(Yuni::String& filePath, StudyInfoContainer& fileContent) 
        : StudyInfoWriter(fileContent), filePath_(filePath)
    {}
    void flush() override;
private:
    Yuni::IO::File::Stream outputFile_;
    Yuni::String& filePath_;
};

} // namespace Benchmarking
