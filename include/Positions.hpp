// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef			__POSITIONS_HPP__
# define		__POSITIONS_HPP__
# include		<list>
# include		"ILink.hpp"

namespace		hbs
{
  class			Track;

  typedef t_bunny_accurate_position Position;
  typedef t_bunny_accurate_size	Size;
  typedef std::list<
    std::pair<hbs::Position, hbs::ILink::Layer>
    >			Positions;

  struct		Packet
  {
    hbs::Track		*track;
    size_t		node;
    bool		operator==(Packet const &o) const
    {
      return (track == o.track && node == o.node);
    }
    bool		operator!=(Packet const &o) const
    {
      return (!(*this == o));
    }
    Packet		&operator=(Packet const &o)
    {
      track = o.track;
      node = o.node;
      return (*this);
    }
    Packet(void) : track(NULL), node((size_t)-1) {}
    Packet(hbs::Track *t, size_t n) : track(t), node(n) {}
    Packet(Packet const &p) : track(p.track), node(p.node) {}
    ~Packet(void) {}
  };
}

#endif	//		__POSITIONS_HPP__
