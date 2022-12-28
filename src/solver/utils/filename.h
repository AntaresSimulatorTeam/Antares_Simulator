#pragma once

#include <string>
#include <memory>

// ------------------------------------
// Optimization period : base class
// ------------------------------------
class optPeriod
{
public:
    virtual std::string to_string() const = 0;
};


// ------------------------------------
// Daily optimization
// ------------------------------------
class optDaily : public optPeriod
{
public:
    optDaily(unsigned int day, unsigned int week, unsigned int year);
    std::string to_string() const; // Shouldn't we have override key-word as well ?

private:
    unsigned int day_ = 0;
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};


// ------------------------------------
// Weekly optimization
// ------------------------------------
class optWeekly : public optPeriod
{
public:
    optWeekly(unsigned int week, unsigned int year);
    std::string to_string() const;

private:
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};

std::shared_ptr<optPeriod> createOptimizationPeriod(bool isOptimizationWeekly,
                                                    unsigned int day,
                                                    unsigned int week,
                                                    unsigned int year); 

std::string createOptimizationFilename(const std::string& title,
                                       std::shared_ptr<optPeriod> opt_period,
                                       const unsigned int optNumber,
                                       const std::string& extension);

std::string createCriterionFilename(std::shared_ptr<optPeriod> opt_period, const unsigned int optNumber);
std::string createMPSfilename(std::shared_ptr<optPeriod> opt_period, const unsigned int optNumber);