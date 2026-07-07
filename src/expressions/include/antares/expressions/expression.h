// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include <antares/expressions/NodeRegistry.h>

namespace Antares::Expressions::Nodes
{
class Node;
}

namespace Antares::ModelerStudy::SystemModel
{

// TODO: add unit tests for this class
class Expression final
{
public:
    Expression() = default;

    explicit Expression(const std::string& value,
                        Expressions::NodeRegistry root,
                        std::string fileAndLineNb = ""):
        value_(value),
        root_(std::move(root)),
        fileAndLineNb_(std::move(fileAndLineNb)),
        empty_(!root_.node)
    {
    }

    const std::string& Value() const
    {
        return value_;
    }

    Expressions::Nodes::Node* RootNode() const
    {
        return root_.node;
    }

    bool Empty() const
    {
        return empty_;
    }

    const std::string& FileAndLineNb() const
    {
        return fileAndLineNb_;
    }

private:
    std::string value_;
    Expressions::NodeRegistry root_;
    /// used for error reporting only
    std::string fileAndLineNb_;
    bool empty_ = true;
};

} // namespace Antares::ModelerStudy::SystemModel
