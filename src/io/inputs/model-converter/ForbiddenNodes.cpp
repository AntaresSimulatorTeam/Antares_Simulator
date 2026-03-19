#include <antares/io/inputs/model-converter/ForbiddenNodes.h>

namespace Antares::IO::Inputs::ModelConverter
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
} // namespace Antares::IO::Inputs::ModelConverter
