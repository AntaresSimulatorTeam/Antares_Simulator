#include <antares/io/inputs/forbidden-nodes/ForbiddenNodes.h>

namespace Antares::IO::Inputs::ForbidNodes
{
bool ForbiddenNodes::isGloballyForbidden(const std::type_index& typeId) const
{
    return global_.contains(typeId);
}

bool ForbiddenNodes::isForbiddenByParent(const std::type_index& parentTypeId,
                                         const std::type_index& nodeTypeId) const
{
    const auto& it = rules_.find(parentTypeId);
    return (it != rules_.end()) && it->second.contains(nodeTypeId);
}
} // namespace Antares::IO::Inputs::ForbidNodes
