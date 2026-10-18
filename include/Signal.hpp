#ifndef HBS_SIGNAL_HPP
#define HBS_SIGNAL_HPP
#include "IComponent.hpp"

namespace hbs
{
  Tristate ResolveSignal(IComponent &component, size_t pin);
}
#endif
