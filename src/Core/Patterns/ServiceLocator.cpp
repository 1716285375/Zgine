#include <Zgine/Core/Patterns/ServiceLocator.h>

namespace Zgine {

// Static member definition
std::unordered_map<std::type_index, void*> ServiceLocator::s_Services;

} // namespace Zgine
