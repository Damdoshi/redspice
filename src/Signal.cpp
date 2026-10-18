// RED Spice - digital passive network resolution.
#include "Signal.hpp"
#include <algorithm>
#include <functional>
#include <set>
#include <vector>

namespace
{
  struct Terminal
  {
    hbs::IComponent *component;
    size_t pin;
    bool operator<(const Terminal &other) const
    {
      if (component != other.component)
        return std::less<hbs::IComponent *>()(component, other.component);
      return pin < other.pin;
    }
  };
  struct Path
  {
    Terminal terminal;
    bool weak;
    unsigned levels;
    bool operator<(const Path &other) const
    {
      if (terminal < other.terminal) return true;
      if (other.terminal < terminal) return false;
      if (weak != other.weak) return weak < other.weak;
      return levels < other.levels;
    }
  };
  struct Drive
  {
    bool low = false, high = false, broken = false, unknown = false;
    bool Active() const { return low || high || broken || unknown; }
    void Add(hbs::Tristate state, unsigned levels)
    {
      if (state == hbs::FALSE && (levels & 1)) low = true;
      if (state == hbs::TRUE && (levels & 2)) high = true;
      if (state == hbs::BROKEN) broken = true;
      if (state == hbs::UNDEFINED) unknown = true;
    }
    hbs::Tristate Value() const
    {
      if (broken || (low && high)) return hbs::BROKEN;
      if (high) return hbs::TRUE;
      if (low) return hbs::FALSE;
      return hbs::UNDEFINED;
    }
  };
  thread_local std::set<Terminal> resolving;
}

hbs::Tristate hbs::ResolveSignal(IComponent &component, size_t pin)
{
  Terminal origin{&component, pin};
  if (!resolving.insert(origin).second) return UNDEFINED;
  struct Guard
  {
    Terminal terminal;
    ~Guard() { resolving.erase(terminal); }
  } guard{origin};
  std::set<Path> visited;
  std::vector<Path> pending{{origin, false, 3}};
  Drive strong, weak;
  bool floating = false;
  while (!pending.empty())
    {
      Path path = pending.back();
      pending.pop_back();
      if (!path.levels || !visited.insert(path).second) continue;
      IComponent &node = *path.terminal.component;
      size_t n = path.terminal.pin;
      std::vector<SignalConnection> neighbours;
      node.GetConnections(n, neighbours);
      Tristate value = HIGH_IMPEDANCE;
      bool contributes = false;
      if (node.IsPassivePin(n))
        {
          floating = true;
          value = node.GetPassiveConnections(n, neighbours);
          contributes = value != HIGH_IMPEDANCE;
        }
      else if (node.DrivesPin(n))
        {
          value = node.Compute(n);
          floating = floating || value == HIGH_IMPEDANCE;
          contributes = value != HIGH_IMPEDANCE;
        }
      if (contributes)
        (path.weak ? weak : strong).Add(value, path.levels);
      for (const SignalConnection &next : neighbours)
        if (next.component)
          pending.push_back({{next.component, next.pin}, path.weak || next.weak,
                             path.levels & next.levels});
    }
  if (strong.Active()) return strong.Value();
  if (weak.Active()) return weak.Value();
  return floating ? HIGH_IMPEDANCE : UNDEFINED;
}
