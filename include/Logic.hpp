// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef				__HBS_LOGIC_HPP__
# define			__HBS_LOGIC_HPP__
# include			<cstddef>
# include			"IComponent.hpp"

namespace			hbs
{
  inline bool			LogicKnown(hbs::Tristate v)
  {
    return (v == hbs::FALSE || v == hbs::TRUE);
  }

  inline hbs::Tristate		LogicNot(hbs::Tristate v)
  {
    if (v == hbs::TRUE)
      return (hbs::FALSE);
    if (v == hbs::FALSE)
      return (hbs::TRUE);
    if (v == hbs::BROKEN)
      return (hbs::BROKEN);
    return (hbs::UNDEFINED);
  }

  inline hbs::Tristate		LogicAnd(const hbs::Tristate *values, size_t count)
  {
    for (size_t i = 0; i < count; ++i)
      if (values[i] == hbs::BROKEN)
	return (hbs::BROKEN);
    for (size_t i = 0; i < count; ++i)
      if (values[i] == hbs::FALSE)
	return (hbs::FALSE);
    for (size_t i = 0; i < count; ++i)
      if (values[i] != hbs::TRUE)
	return (hbs::UNDEFINED);
    return (hbs::TRUE);
  }

  inline hbs::Tristate		LogicOr(const hbs::Tristate *values, size_t count)
  {
    for (size_t i = 0; i < count; ++i)
      if (values[i] == hbs::BROKEN)
	return (hbs::BROKEN);
    for (size_t i = 0; i < count; ++i)
      if (values[i] == hbs::TRUE)
	return (hbs::TRUE);
    for (size_t i = 0; i < count; ++i)
      if (values[i] != hbs::FALSE)
	return (hbs::UNDEFINED);
    return (hbs::FALSE);
  }

  inline hbs::Tristate		LogicNand(const hbs::Tristate *values, size_t count)
  {
    return (LogicNot(LogicAnd(values, count)));
  }

  inline hbs::Tristate		LogicNor(const hbs::Tristate *values, size_t count)
  {
    return (LogicNot(LogicOr(values, count)));
  }

  inline hbs::Tristate		LogicXor(const hbs::Tristate *values, size_t count)
  {
    size_t			parity = 0;

    for (size_t i = 0; i < count; ++i)
      {
	if (values[i] == hbs::BROKEN)
	  return (hbs::BROKEN);
	if (!LogicKnown(values[i]))
	  return (hbs::UNDEFINED);
	parity ^= (values[i] == hbs::TRUE);
      }
    return (parity ? hbs::TRUE : hbs::FALSE);
  }

  inline hbs::Tristate		LogicXnor(const hbs::Tristate *values, size_t count)
  {
    return (LogicNot(LogicXor(values, count)));
  }
}

#endif	//			__HBS_LOGIC_HPP__
