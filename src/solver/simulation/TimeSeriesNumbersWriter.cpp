//
// Created by marechaljas on 17/03/23.
//

#include "BindingConstraintsTimeSeriesNumbersWriter.h"
#include <cstdint>
#include <filesystem>
#include <utility>

namespace Antares::Solver::Simulation {
BindingConstraintsTimeSeriesNumbersWriter::BindingConstraintsTimeSeriesNumbersWriter(std::shared_ptr<Antares::Solver::IResultWriter> writer)
: writer_(std::move(writer))
{

}

namespace // anonymous
{
    struct TSNumbersPredicate
    {
        uint32_t operator()(uint32_t value) const
        {
            return value + 1;
        }
    };
} // anonymous namespace

// TODO : remove duplication
static void genericStoreTimeseriesNumbers(Solver::IResultWriter::Ptr writer,
                                          const Matrix<Yuni::uint32>& timeseriesNumbers,
                                          const std::string& id,
                                          const std::string& directory)
{
    TSNumbersPredicate predicate;
    std::filesystem::path path = std::filesystem::path() / "ts-numbers" / directory.c_str() / id.c_str();
    path.replace_extension("txt");

    std::string buffer;
    timeseriesNumbers.saveToBuffer(buffer,
                                   0,         // precision
                                   true,      // print_dimensions
                                   predicate, // predicate
                                   true);     // save even if all coeffs are zero

    writer->addEntryFromBuffer(path.c_str(), buffer);
}

void BindingConstraintsTimeSeriesNumbersWriter::write(const Data::BindingConstraintsList &list) {
    for (auto const& [group, timeSeries]: list.TimeSeriesNumbers()) {
        genericStoreTimeseriesNumbers(writer_,
                                      timeSeries.timeseriesNumbers,
                                      group,
                                      "bindingconstraints");
    }

}
} // Simulation