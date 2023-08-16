//
// Created by marechaljas on 03/07/23.
//

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class BindingConstraintsTSNumberData : public TSNumberData
{
public:
    BindingConstraintsTSNumberData() = default;
    virtual ~BindingConstraintsTSNumberData() = default;

    bool reset(const Study& study) override;
    void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const override;

    void setData(const std::string& group_name, unsigned year, unsigned value);
    unsigned get(const std::string& group_name, unsigned year) const;
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    unsigned get_tsGenCount(const Study& study) const override;
private:
    std::map<std::string, MatrixType> rules_;
};

inline unsigned BindingConstraintsTSNumberData::get(const std::string& group_name, const unsigned year) const
{
    auto it = rules_.find(group_name);
    if (it == rules_.end()) {
        return 0;
    }
    return it->second[0][year];
}

inline CString<512, false> BindingConstraintsTSNumberData::get_prefix() const
{
    return "bc,";
}

inline unsigned BindingConstraintsTSNumberData::get_tsGenCount(const Study&) const {
    return 0;
}
}