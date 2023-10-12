#pragma once

#include <utility>

class IntoUTF8ArgsTranslator
{
public:
    IntoUTF8ArgsTranslator(int argc, char** argv);
    std::pair<int, char**> convert();
    ~IntoUTF8ArgsTranslator();

private:
    int argc_;
    char** argv_;
};
