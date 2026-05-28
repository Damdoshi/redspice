// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef			__POSITIONS_HPP__
# define		__POSITIONS_HPP__
# include		<list>
# include		"ILink.hpp"

namespace		hbs
{
  typedef t_bunny_accurate_position Position;
  typedef t_bunny_accurate_size	Size;
  typedef std::list<
    std::pair<hbs::Position, hbs::ILink::Layer>
    >			Positions;

  struct		Packet
  {
    hbs::ILink		*link;
    size_t		ori_pin;
    hbs::Positions::iterator pos;
    bool		operator==(Packet const &o) const
    {
      return (link == o.link && ori_pin == o.ori_pin && pos == o.pos);
    }
    bool		operator!=(Packet const &o) const
    {
      return (!(*this == o));
    }
    Packet		&operator=(Packet const &o)
    {
      link = o.link;
      ori_pin = o.ori_pin;
      pos = o.pos;
      return (*this);
    }
    Packet(void) : link(NULL), ori_pin(0) {
      hbs::Positions ps;
      pos = ps.end();
    }
    Packet(Packet const &p)
      : link(p.link),
	ori_pin(p.ori_pin),
	pos(p.pos)
    {}
    ~Packet(void) {}
  };
}

#endif	//		__POSITIONS_HPP__
