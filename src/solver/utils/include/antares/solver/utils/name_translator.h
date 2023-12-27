#pragma once

#include <vector>
#include <string>
#include <memory>

class NameTranslator
{
public:
    virtual ~NameTranslator() = default;
    virtual char** translate(const std::vector<std::string>& src,
                             std::vector<char*>& pointerVec) = 0;
    static std::unique_ptr<NameTranslator> create(bool useRealNames);
};

class RealName : public NameTranslator
{
public:
    ~RealName() override = default;
private:
    char** translate(const std::vector<std::string>& src,
                     std::vector<char*>& pointerVec) override;
};

class NullName : public NameTranslator
{
public:
    ~NullName() override = default;
private:
    char** translate(const std::vector<std::string>& src,
                     std::vector<char*>& pointerVec) override;
};
