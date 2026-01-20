// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_HXX__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_HXX__

namespace Antares::TSGenerator::XCast
{
template<class PredicateT>
inline void XCast::loadFromStudy(const Data::Correlation& correlation, PredicateT& predicate)
{
    pData.template loadFromStudy<PredicateT>(study, correlation, predicate);
}

} // namespace Antares::TSGenerator::XCast

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_HXX__
