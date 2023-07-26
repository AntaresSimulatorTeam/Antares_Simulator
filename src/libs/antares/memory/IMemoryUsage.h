//
// Created by marechaljas on 26/07/23.
//

#pragma once

class IMemoryUsage {
public:
    virtual ~IMemoryUsage() = default;
    virtual unsigned NbYearsParallel() = 0;
    virtual bool GatheringInformationsForInput() = 0;
    virtual void AddRequiredMemoryForInput(unsigned int) = 0;
    virtual void AddRequiredMemoryForOutput(unsigned int) = 0;
};