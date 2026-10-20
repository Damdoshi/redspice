// RED Spice - bounded, local repair of existing copper after a small edit.
#ifndef HBS_ROUTER_HPP
#define HBS_ROUTER_HPP
#include "Positions.hpp"
#include <vector>

namespace hbs
{
  class IComponent;
  class Track;
  struct RoutingAnchor
  {
    Track *track;
    IComponent *component;
    size_t pin, attachment, node;
    Position before; // previous copper position
    Position pin_before;
  };
  typedef std::vector<RoutingAnchor> RoutingSnapshot;
  struct RoutingBudget
  {
    size_t max_expansions = 4096;
    size_t max_checks = 100000;
    unsigned milliseconds = 12;
  };
  struct RoutingResult
  {
    size_t repaired = 0, pending = 0, expanded = 0, checks = 0;
    bool limited = false;
  };
}
#endif
