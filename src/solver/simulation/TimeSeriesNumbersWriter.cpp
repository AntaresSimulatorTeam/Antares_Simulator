//
// Created by marechaljas on 17/03/23.
//

#include "BindingConstraintsTimeSeriesNumbersWriter.h"
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"
#include <cstdint>
#include <filesystem>
#include <utility>

namespace Antares::Solver::Simulation {
BindingConstraintsTimeSeriesNumbersWriter::BindingConstraintsTimeSeriesNumbersWriter(IResultWriter& writer)
: writer_(writer)
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
static void genericStoreTimeseriesNumbers(Solver::IResultWriter& writer,
                                          const Matrix<uint32_t>& timeseriesNumbers,
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

    writer.addEntryFromBuffer(path.string(), buffer);
}

void BindingConstraintsTimeSeriesNumbersWriter::write(const Data::BindingConstraintGroupRepository &bindingConstraintGroupRepository) {
    for (auto const& group: bindingConstraintGroupRepository) {
        genericStoreTimeseriesNumbers(writer_,
                                      group->timeseriesNumbers,
                                      group->name(),
                                      "bindingconstraints");
    }

}
} // Simulation
