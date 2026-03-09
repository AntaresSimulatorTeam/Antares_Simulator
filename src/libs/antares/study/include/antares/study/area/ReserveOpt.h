// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <stdexcept>
#include <string>

template<class T>
class DebugOptional
{
public:
    using value_type = T;

    DebugOptional() = default;
    DebugOptional(const DebugOptional&) = default;
    DebugOptional(DebugOptional&&) noexcept = default;
    DebugOptional& operator=(const DebugOptional&) = default;
    DebugOptional& operator=(DebugOptional&&) noexcept = default;

    DebugOptional(std::string name):
        name_(std::move(name))
    {
    }

    DebugOptional(const T& v, std::string name = ""):
        opt_(v),
        name_(std::move(name))
    {
    }

    DebugOptional(T&& v, std::string name = ""):
        opt_(std::move(v)),
        name_(std::move(name))
    {
    }

    // -- standard optional interface forwarding --

    constexpr bool has_value() const noexcept
    {
        return opt_.has_value();
    }

    constexpr explicit operator bool() const noexcept
    {
        return opt_.operator bool();
    }

    //
    // Core: every access goes through value_with_context()
    //
    T& value_with_context(const char* file, int line)
    {
        if (!opt_)
        {
            throw make_error(file, line);
        }
        return opt_.value();
    }

    const T& value_with_context(const char* file, int line) const
    {
        if (!opt_)
        {
            throw make_error(file, line);
        }
        return opt_.value();
    }

    //
    // Standard-looking API, but routed through value_with_context()
    //
    T& value(const char* file = __builtin_FILE(), int line = __builtin_LINE())
    {
        return value_with_context(file, line);
    }

    const T& value(const char* file = __builtin_FILE(), int line = __builtin_LINE()) const
    {
        return value_with_context(file, line);
    }

    T&& value_rvalue(const char* file = __builtin_FILE(), int line = __builtin_LINE())
    {
        if (!opt_)
        {
            throw make_error(file, line);
        }
        return std::move(opt_).value();
    }

    const T&& value_const_rvalue(const char* file = __builtin_FILE(),
                                 int line = __builtin_LINE()) const
    {
        if (!opt_)
        {
            throw make_error(file, line);
        }
        return std::move(opt_).value();
    }

    //
    // operator->
    //
    T* operator->()
    {
        return &value(); // routes through file/line version
    }

    const T* operator->() const
    {
        return &value(); // routes through file/line version
    }

    //
    // operator*
    //
    T& operator*()
    {
        return value();
    }

    const T& operator*() const
    {
        return value();
    }

    // forwarding for emplace, reset, etc.
    template<class... Args>
    T& emplace(Args&&... args)
    {
        return opt_.emplace(std::forward<Args>(args)...);
    }

    void reset() noexcept
    {
        opt_.reset();
    }

    // Access to debug name
    const std::string& name() const noexcept
    {
        return name_;
    }

private:
    std::runtime_error make_error(const char* file, int line) const
    {
        std::string msg = "Bad optional access";

        if (!name_.empty())
        {
            msg += " in optional<" + name_ + ">";
        }

        msg += " at ";
        msg += file;
        msg += ":";
        msg += std::to_string(line);

        return std::runtime_error(msg);
    }

    std::optional<T> opt_;
    std::string name_;
};

/// Optional type with debug information
template<class T>
using ReserveOpt = DebugOptional<T>;

/// Convenience macro for accessing value() with automatic file/line
#define RESERVE_VALUE(opt) (opt).value(__FILE__, __LINE__)
