// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
//
// RED Spice

#ifndef                         __HBS_VIDEO_SCREEN_HPP__
# define                        __HBS_VIDEO_SCREEN_HPP__
# include                       <string>
# include                       <vector>
# include                       "Output.hpp"

namespace                       hbs
{
  class                         VideoScreen : public hbs::AComponent<8>, public hbs::Output
  {
  private:
    std::string                 definition;
    size_t                      window_width;
    size_t                      window_height;
    size_t                      cursor_x;
    size_t                      cursor_y;
    size_t                      last_tick;
    std::vector<unsigned int>   pixels;

    const std::string           &GetType(void) const;
    void                        ParseDefinition(const std::string &file);
    unsigned int                ReadColor(void);
    bool                        IsRising(size_t pin);
    void                        Capture(void);
    hbs::Size                   PreviewSize(const hbs::Screen &screen) const;
    hbs::Position               PreviewOrigin(void) const;

  public:
    void                        SetLink(size_t              pnthis,
                                        hbs::IComponent     &component,
                                        size_t              pntarg,
                                        const std::string   &pos);
    void                        Draw(hbs::Screen            &screen) const;
    void                        Move(const hbs::Position    &pos)
    {
      hbs::AComponent<8>::Move(pos);
    }
    const std::string           &GetName(void) const final;
    void                        SetName(const std::string &name) final;
    void                        DisconnectFrom(const hbs::IComponent *component) final;
    size_t                      GetPinCount(void) const final;
    hbs::Position               GetPosition(void) const final;
    hbs::Position               GetPinPosition(size_t       pin) const;
    bool                        IsUnder(const hbs::Screen    &screen,
                                        const t_bunny_position &pos) const;

    hbs::Tristate               Compute(size_t              n = 1);
    void                        Dump(void) const;
    bool                        Displayable(void) const;
    std::string                 GetDefinitionValue(void) const;
    bool                        DrivesPin(size_t pin) const { (void)pin; return (false); }

    VideoScreen(hbs::Timer      &timer,
                const std::string &name,
                const std::string &file,
                const std::string &pos);
    virtual ~VideoScreen(void);
  };
}

#endif //                       __HBS_VIDEO_SCREEN_HPP__
