#pragma once

#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{
/*!
 * Sets link bounds for first step of adequacy patch or leaves default values if adequacy patch is
 * not used.
 */
void setNTCbounds(double& Xmax,
                  double& Xmin,
                  VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* ProblemeHebdo);

} // namespace AdequacyPatch
} // end namespace Data
} // namespace Antares
