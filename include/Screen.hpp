// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2022
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef					__SCREEN_HPP__
# define				__SCREEN_HPP__
# include				<lapin.h>

namespace				hbs
{
  class					Circuit;
  class					Screen
  {
  private:
    t_bunny_window			*win;
    t_bunny_picture			*pic;
    t_bunny_position			camera;

  public:
    typedef t_bunny_accurate_position	Position;
    typedef t_bunny_accurate_size	Size;

    static constexpr unsigned int	White = WHITE;
    static constexpr unsigned int	Red = RED;
    static constexpr unsigned int	Blue = BLUE;
    static constexpr unsigned int	Yellow = YELLOW;
    static constexpr unsigned int	Teal = TEAL;
    static constexpr unsigned int	Green = GREEN;

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
					       unsigned int		col);
    void				Text(Position			p,
					     unsigned int		col,
					     const std::string		&str);
    
    bool				Loop(hbs::Circuit		&circ);
    
    Screen(void);
    ~Screen(void);
  };
}

#endif	//			__SCREEN_HPP__
