#pragma once

#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{

/*!
    * Determines restriction type for transmission links for first step of adequacy patch.
    *
    * @param ProblemeHebdo PROBLEME_HEBDO*: Weekly problem structure.
    *
    * @param Interco int: Index of the link.
    *
    * @return uint from an enumeration that describes the type of restrictions to put on this link for
    * adq purposes.
    */
ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* ProblemeHebdo, int Interco);

/*!
    * Determines restriction type for transmission links for first step of adequacy patch, when start
    * node is inside adq path (type 2).
    *
    * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
    *
    * @return uint from an enumeration that describes the type of restrictions to put on this link for
    * adq purposes.
    */
ntcSetToZeroStatus_AdqPatchStep1 SetNTCForAdequacyFirstStepOriginNodeInsideAdq(AdequacyPatchMode ExtremityNodeAdequacyPatchType);

/*!
    * Determines restriction type for transmission links for first step of adequacy patch, when start
    * node is outside adq path (type 1).
    *
    * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
    *
    * @param setToZeroNTCfromOutToIn_AdqPatch bool: Switch to cut links from nodes outside adq patch
    * (type 1) towards nodes inside adq patch (type 2).
    *
    * @param setToZeroNTCfromOutToOut_AdqPatch bool: Switch to cut links between nodes outside adq
    * patch (type 1).
    *
    * @return uint from an enumeration that describes the type of restrictions to put on this link for
    * adq purposes.
    */
ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeOutsideAdq(
    AdequacyPatchMode ExtremityNodeAdequacyPatchType,
    bool setToZeroNTCfromOutToIn_AdqPatch,
    bool setToZeroNTCfromOutToOut_AdqPatch);

/*!
    * Sets link bounds for first step of adequacy patch or leaves default values if adequacy patch is
    * not used.
    */
void setNTCbounds(double& Xmax,
    double& Xmin,
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
    const int Interco,
    PROBLEME_HEBDO* ProblemeHebdo);

} // end namespace Antares
} // end namespace Data
} // end namespace AdequacyPatch
