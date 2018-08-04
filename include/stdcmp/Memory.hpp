// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef				__NTS_MEMORY_HPP__
# define			__NTS_MEMORY_HPP__
# include			<stdint.h>
# include			"AComponent.hpp"

namespace			nts
{
  template<int			Capacity,
	   int			Pin>
  class				Memory : public nts::AComponent<Pin>
  {
  protected:
    std::array<uint8_t, Capacity> memory;

    uint8_t			GetData(size_t		pos)
    {
      if (pos >= Capacity)
	throw nts::MemoryOverflow("Asked for an outside location in a memory component");
      return (memory[pos]);
    }

    void			SetData(size_t		pos,
					uint8_t		dat)
    {
      if (pos >= Capacity)
	throw nts::MemoryOverflow("Asked for an outside location in a memory component");
      memory[pos] = dat;
    }

    size_t			GetMemorySize(void) const
    {
      return (Capacity);
    }

    Memory(const nts::Timer	&timer)
      : AComponent<Pin>(timer)
    {}
    virtual ~Memory(void)
    {}
  };
}

#endif	//			__NTS_MEMORY_HPP__
