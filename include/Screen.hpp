// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2023
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__SCREEN_HPP__
# define				__SCREEN_HPP__
# define				PINSIZE_DEFAULT		20
# include				<lapin.h>
# include				"Timer.hpp"

namespace				hbs
{
  class					Circuit;
  class					Screen
  {
  private:
    mutable t_bunny_window		*win;
    mutable t_bunny_picture		*pic;
    t_bunny_position			camera;
    int					pin_size;

  public:
    typedef t_bunny_accurate_position	Position;
    typedef t_bunny_accurate_size	Size;

    static constexpr unsigned int	White = WHITE;
    static constexpr unsigned int	Red = RED;
    static constexpr unsigned int	Blue = BLUE;
    static constexpr unsigned int	Yellow = YELLOW;
    static constexpr unsigned int	Teal = TEAL;
    static constexpr unsigned int	Green = GREEN;
    static constexpr unsigned int	Purple = PURPLE;
    static constexpr unsigned int	Pink = PINK2;

    int					PinSize(int			p = 0);

    void				Resize(const t_bunny_window	&win,
					       t_bunny_position		pos);
    bool				Draw(hbs::Circuit		&c);

    void				Circle(Position			pos,
					       Size			siz,
					       unsigned int		col,
					       bool			full = false);
    void				Line(Position			a,
					     Position			b,
					     unsigned int		col);
    void				Square(Position			pos,
					       Size			siz,
					       unsigned int		col,
					       bool			full = false);
    void				Text(Position			p,
					     Size			s,
					     unsigned int		col,
					     const std::string		&str);
    Size				TextSize(Size			s,
						 const std::string	&str);

    bool				Loop(hbs::Circuit		&circ,
					     hbs::Timer			&timer);

    Screen(void);
    ~Screen(void);
  };
}

struct				LoopData
{
  hbs::Circuit			&circuit;
  hbs::Timer			&timer;
  hbs::Screen			&screen;
};

#endif	//			__SCREEN_HPP__
