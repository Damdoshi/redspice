// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#include                        <algorithm>
#include                        <cstdlib>
#include                        <iostream>
#include                        <sstream>
#include                        "VideoScreen.hpp"

const std::string               &hbs::VideoScreen::GetType(void) const
{
  return (hbs::AComponent<8>::type = "screen");
}

void                            hbs::VideoScreen::ParseDefinition(const std::string &file)
{
  size_t                        x;
  size_t                        h;
  size_t                        w;

  definition = file;
  window_width = 320;
  window_height = 200;
  if (file.empty())
    {
      definition = "320x200";
      return ;
    }
  x = file.find('x');
  if (x == std::string::npos)
    x = file.find('X');
  if (x == std::string::npos)
    return ;
  w = strtoul(file.substr(0, x).c_str(), NULL, 10);
  h = strtoul(file.substr(x + 1).c_str(), NULL, 10);
  if (w == 0 || h == 0)
    return ;
  window_width = std::min<size_t>(w, 2048);
  window_height = std::min<size_t>(h, 2048);
  std::stringstream ss;
  ss << window_width << "x" << window_height;
  definition = ss.str();
}

static unsigned int             scale2(bool low, bool high)
{
  return ((low ? 1 : 0) | (high ? 2 : 0)) * 85;
}

unsigned int                    hbs::VideoScreen::ReadColor(void)
{
  unsigned int                  r;
  unsigned int                  g;
  unsigned int                  b;

  r = scale2(hbs::AComponent<8>::GetPin(3) == hbs::TRUE, hbs::AComponent<8>::GetPin(4) == hbs::TRUE);
  g = scale2(hbs::AComponent<8>::GetPin(5) == hbs::TRUE, hbs::AComponent<8>::GetPin(6) == hbs::TRUE);
  b = scale2(hbs::AComponent<8>::GetPin(7) == hbs::TRUE, hbs::AComponent<8>::GetPin(8) == hbs::TRUE);
  return ((r << 24) | (g << 16) | (b << 8) | 0xFF);
}

bool                            hbs::VideoScreen::IsRising(size_t pin)
{
  return (hbs::AComponent<8>::GetPrevious(pin) == hbs::FALSE &&
          hbs::AComponent<8>::GetPin(pin) == hbs::TRUE);
}

void                            hbs::VideoScreen::Capture(void)
{
  if (last_tick == hbs::AComponent<8>::timer.GetTime())
    return ;
  last_tick = hbs::AComponent<8>::timer.GetTime();

  // Force all input nets to be sampled for this tick.
  for (size_t i = 1; i <= 8; ++i)
    hbs::AComponent<8>::GetPin(i);

  if (IsRising(1))
    {
      cursor_x = 0;
      cursor_y = 0;
    }
  else if (IsRising(2))
    {
      cursor_x = 0;
      cursor_y = (cursor_y + 1) % window_height;
    }

  if (cursor_x < window_width && cursor_y < window_height)
    pixels[cursor_y * window_width + cursor_x] = ReadColor();
  cursor_x += 1;
  if (cursor_x >= window_width)
    {
      cursor_x = 0;
      cursor_y = (cursor_y + 1) % window_height;
    }
}

hbs::Position                   hbs::VideoScreen::PreviewOrigin(void) const
{
  return (hbs::AComponent<8>::position + hbs::Position{2.0, 0.0});
}

hbs::Size                       hbs::VideoScreen::PreviewSize(const hbs::Screen &screen) const
{
  return (hbs::Size{std::max(4.0, window_width / (double)screen.pin_size),
                    std::max(4.0, window_height / (double)screen.pin_size)});
}

const std::string               &hbs::VideoScreen::GetName(void) const
{
  return (hbs::AComponent<8>::GetName());
}

void                            hbs::VideoScreen::SetName(const std::string &name)
{
  hbs::AComponent<8>::SetName(name);
  hbs::AComponent<1>::SetName(name);
}

void                            hbs::VideoScreen::DisconnectFrom(const hbs::IComponent *component)
{
  hbs::AComponent<8>::DisconnectFrom(component);
  hbs::AComponent<1>::DisconnectFrom(component);
}

size_t                          hbs::VideoScreen::GetPinCount(void) const
{
  return (hbs::AComponent<8>::GetPinCount());
}

hbs::Position                   hbs::VideoScreen::GetPosition(void) const
{
  return (hbs::AComponent<8>::GetPosition());
}

hbs::Position                   hbs::VideoScreen::GetPinPosition(size_t pin) const
{
  if (pin < 1 || pin > 8)
    throw hbs::BadPin(GetType() + ": Bad pin.");
  return (hbs::AComponent<8>::position + hbs::Position{0.0, (double)(pin - 1)});
}

bool                            hbs::VideoScreen::IsUnder(const hbs::Screen &screen,
                                                          const t_bunny_position &pos) const
{
  hbs::Position                 p = {(double)pos.x, (double)pos.y};
  hbs::Position                 origin = PreviewOrigin();
  hbs::Size                     size = PreviewSize(screen);
  double                        bottom = std::max(7.0, size.y);

  return (p.x >= hbs::AComponent<8>::position.x &&
          p.x <= origin.x + size.x &&
          p.y >= hbs::AComponent<8>::position.y - 1.0 &&
          p.y <= hbs::AComponent<8>::position.y + bottom + 1.0);
}

static void                     draw_scaled_pixels(hbs::Screen &screen,
                                                   const hbs::Position &origin,
                                                   const hbs::Size &size,
                                                   size_t source_width,
                                                   size_t source_height,
                                                   const std::vector<unsigned int> &pixels)
{
  int                           dst_x;
  int                           dst_y;
  int                           dst_w;
  int                           dst_h;
  int                           min_x;
  int                           min_y;
  int                           max_x;
  int                           max_y;
  t_bunny_position              pos;

  dst_x = (int)((origin.x - screen.camera.x) * screen.pin_size + screen.pic->buffer.width / 2);
  dst_y = (int)((origin.y - screen.camera.y) * screen.pin_size + screen.pic->buffer.height / 2);
  dst_w = std::max(1, (int)(size.x * screen.pin_size));
  dst_h = std::max(1, (int)(size.y * screen.pin_size));
  min_x = std::max(0, dst_x);
  min_y = std::max(0, dst_y);
  max_x = std::min((int)screen.pic->buffer.width, dst_x + dst_w);
  max_y = std::min((int)screen.pic->buffer.height, dst_y + dst_h);
  for (int y = min_y; y < max_y; ++y)
    {
      size_t sy = (size_t)((long long)(y - dst_y) * (long long)source_height / dst_h);
      pos.y = y;
      for (int x = min_x; x < max_x; ++x)
        {
          size_t sx = (size_t)((long long)(x - dst_x) * (long long)source_width / dst_w);
          pos.x = x;
          bunny_set_pixel(&screen.pic->buffer, pos, pixels[sy * source_width + sx]);
        }
    }
}

void                            hbs::VideoScreen::Draw(hbs::Screen &screen) const
{
  hbs::Position                 origin = PreviewOrigin();
  hbs::Size                     size = PreviewSize(screen);

  screen.Text(hbs::AComponent<8>::position + hbs::Position{0, -1.5}, {10, 10}, hbs::Screen::White, GetName());
  screen.Text(hbs::AComponent<8>::position + hbs::Position{2, -1.5}, {10, 10}, hbs::Screen::Teal, GetType());
  screen.Text(hbs::AComponent<8>::position + hbs::Position{0.5, -0.5}, {8, 8}, hbs::Screen::Teal, definition);

  static const char             *labels[8] = {"VS", "HS", "R0", "R1", "G0", "G1", "B0", "B1"};
  for (size_t i = 1; i <= 8; ++i)
    {
      screen.Line(GetPinPosition(i), hbs::AComponent<8>::position + hbs::Position{1.5, (double)(i - 1)}, hbs::Screen::White);
      screen.Circle(GetPinPosition(i), {0.4, 0.4}, hbs::Screen::Teal, true);
      screen.Circle(GetPinPosition(i), {0.4, 0.4}, hbs::Screen::White, false);
      screen.Text(GetPinPosition(i) + hbs::Position{0.25, -0.25}, {6, 6}, hbs::Screen::White, labels[i - 1]);
    }

  draw_scaled_pixels(screen, origin, size, window_width, window_height, pixels);
  screen.Square(origin, size, hbs::Screen::White, false);
}

void                            hbs::VideoScreen::SetLink(size_t a,
                                                          hbs::IComponent &b,
                                                          size_t c,
                                                          const std::string &linkpath)
{
  hbs::AComponent<8>::SetLink(a, b, c, linkpath);
}

void                            hbs::VideoScreen::Dump(void) const
{
  hbs::AComponent<8>::Dump();
}

hbs::Tristate                   hbs::VideoScreen::Compute(size_t n)
{
  hbs::Tristate                 tri;

  if (n < 1 || n > 8)
    throw hbs::BadPin(GetType() + ": Bad pin.");
  if (hbs::AComponent<8>::AlreadyComputed(n, tri))
    return (tri);
  hbs::AComponent<8>::PresetOutput(n);
  Capture();
  return (hbs::AComponent<8>::GetPin(n));
}

bool                            hbs::VideoScreen::Displayable(void) const
{
  return (false);
}

std::string                     hbs::VideoScreen::GetDefinitionValue(void) const
{
  return (definition);
}

hbs::VideoScreen::VideoScreen(hbs::Timer        &timer,
                              const std::string &name,
                              const std::string &file,
                              const std::string &pos)
  : AComponent<1>(timer, name, pos),
    AComponent<8>(timer, name, pos),
    Output(timer, name, pos),
    window_width(320),
    window_height(200),
    cursor_x(0),
    cursor_y(0),
    last_tick((size_t)-1)
{
  ParseDefinition(file);
  pixels.assign(window_width * window_height, hbs::Screen::Black);
}

hbs::VideoScreen::~VideoScreen(void)
{}
