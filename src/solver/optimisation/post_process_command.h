#pragma once

class PROBLEME_HEBDO;

struct optRuntimeData
{
    optRuntimeData(unsigned int y, unsigned int w, unsigned int h) :
     year(y), week(w), hourInTheYear(h)
    {
    }

    unsigned int year = 0;
    unsigned int week = 0;
    unsigned int hourInTheYear = 0;
};

class PostProcessCommand
{
public:
    PostProcessCommand(PROBLEME_HEBDO* p) : problemeHebdo_(p)
    {
    }
    virtual void acquireOptRuntimeData(const optRuntimeData& opt_runtime_data) = 0;
    virtual void run() = 0;

protected:
    PROBLEME_HEBDO* problemeHebdo_;
};
