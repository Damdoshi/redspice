// Jason Brillante "Damdoshi"
// EFRITS SAS 2022-2026
// Hanged Bunny Studio 2014-2021
//
// RED Spice

#ifndef                 __COMPONENT_MENU_HPP__
# define                __COMPONENT_MENU_HPP__

# include               <algorithm>
# include               <cctype>
# include               <sstream>
# include               <string>
# include               <vector>
# include               "Circuit.hpp"
# include               "Screen.hpp"
# include               "Shortcuts.hpp"

namespace               hbs
{
  struct                ComponentInfo
  {
    const char          *type;
    int                 pins;
    const char          *short_desc;
    const char          *long_desc;
  };

  struct                ComponentMenuGeometry
  {
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    int                 list_w;
    int                 detail_w;
    int                 line;
    int                 search_y;
    int                 first_y;
    int                 rows;
  };

  static inline const ComponentInfo      *ComponentInfoTable(void)
  {
    static const ComponentInfo infos[] =
      {
        {"input", 1, "entree interactive", "Entree logique modifiable au clic molette pendant la simulation. Utile pour injecter manuellement un niveau 0 ou 1 dans le circuit."},
        {"output", 1, "sortie logique", "Sortie simple qui expose l'etat logique calcule par le reseau auquel elle est connectee."},
        {"clock", 1, "horloge", "Source d'horloge periodique pilotee par le timer de simulation."},
        {"terminal", 1, "afficheur texte", "Terminal de sortie permettant d'observer une valeur de maniere lisible pendant l'execution."},
        {"true", 1, "constante 1", "Source logique constante au niveau haut."},
        {"false", 1, "constante 0", "Source logique constante au niveau bas."},

        {"4001", 14, "quad NOR 2 entrees", "Circuit CMOS 4001: quatre portes NOR independantes a deux entrees en boitier DIP 14."},
        {"4002", 14, "double NOR 4 entrees", "Circuit CMOS 4002: deux portes NOR independantes a quatre entrees en boitier DIP 14."},
        {"4008", 16, "additionneur 4 bits", "Circuit CMOS 4008: additionneur binaire complet sur 4 bits avec retenue d'entree et de sortie."},
        {"4011", 14, "quad NAND 2 entrees", "Circuit CMOS 4011: quatre portes NAND independantes a deux entrees en boitier DIP 14."},
        {"4013", 14, "double bascule D", "Circuit CMOS 4013: deux bascules D independantes avec set/reset et sorties complementaires."},
        {"4017", 16, "compteur decade", "Circuit CMOS 4017: compteur Johnson decade avec dix sorties decodees."},
        {"4027", 16, "double bascule JK", "Circuit CMOS 4027: deux bascules JK independantes avec set/reset et sorties complementaires."},
        {"4030", 14, "quad XOR 2 entrees", "Circuit CMOS 4030: quatre portes OU exclusif independantes a deux entrees en boitier DIP 14."},
        {"4040", 16, "compteur 12 bits", "Circuit CMOS 4040: compteur binaire asynchrone 12 etages avec reset."},
        {"4046", 16, "PLL CMOS", "Circuit CMOS 4046: boucle a verrouillage de phase avec oscillateur controle en tension et comparateurs de phase."},
        {"4047", 14, "mono/astable", "Circuit CMOS 4047: multivibrateur monostable ou astable, utile pour generer des impulsions ou une horloge simple."},
        {"4060", 16, "oscillateur + compteur", "Circuit CMOS 4060: oscillateur et compteur/diviseur binaire integre."},
        {"4069", 14, "six inverseurs", "Circuit CMOS 4069: six portes inverseuses independantes en boitier DIP 14."},
        {"4071", 14, "quad OR 2 entrees", "Circuit CMOS 4071: quatre portes OU independantes a deux entrees en boitier DIP 14."},
        {"4081", 14, "quad AND 2 entrees", "Circuit CMOS 4081: quatre portes ET independantes a deux entrees en boitier DIP 14."},
        {"4094", 16, "registre decalage", "Circuit CMOS 4094: registre a decalage 8 bits avec latch de sortie."},
        {"4098", 16, "double monostable", "Circuit CMOS 4098: deux multivibrateurs monostables retriggerables avec sorties complementaires."},
        {"4514", 24, "decodeur 4 vers 16", "Circuit CMOS 4514: decodeur/demultiplexeur 4 vers 16 avec latch d'adresse."},
        {"4521", 24, "diviseur 24 etages", "Circuit CMOS 4521: diviseur de frequence binaire a 24 etages."},
        {"4536", 16, "timer programmable", "Circuit CMOS 4536: timer programmable et diviseur binaire pour temporisations longues."},
        {"4538", 16, "double monostable", "Circuit CMOS 4538: double multivibrateur monostable de precision, retriggerable et resetable."},
        {"4541", 14, "timer programmable", "Circuit CMOS 4541: oscillateur programmable avec compteur/diviseur et sortie timer."},

        {"7400", 14, "quad NAND 2 entrees", "TTL 7400 / 74HC00: quatre portes NAND independantes a deux entrees."},
        {"7402", 14, "quad NOR 2 entrees", "TTL 7402 / 74HC02: quatre portes NOR independantes a deux entrees."},
        {"7404", 14, "six inverseurs", "TTL 7404 / 74HC04: six portes inverseuses independantes."},
        {"7408", 14, "quad AND 2 entrees", "TTL 7408 / 74HC08: quatre portes ET independantes a deux entrees."},
        {"7432", 14, "quad OR 2 entrees", "TTL 7432 / 74HC32: quatre portes OU independantes a deux entrees."},
        {"7486", 14, "quad XOR 2 entrees", "TTL 7486 / 74HC86: quatre portes OU exclusif independantes a deux entrees."},
        {"7474", 14, "double bascule D", "TTL 7474 / 74HC74: deux bascules D sur front avec preset et clear."},
        {"74138", 16, "decodeur 3 vers 8", "TTL 74138 / 74HC138: decodeur/demultiplexeur 3 vers 8 avec sorties actives a l'etat bas."},
        {"74139", 16, "double decodeur 2 vers 4", "TTL 74139 / 74HC139: deux decodeurs/demultiplexeurs 2 vers 4 independants."},
        {"74151", 16, "multiplexeur 8 vers 1", "TTL 74151 / 74HC151: multiplexeur de donnees 8 vers 1."},
        {"74157", 16, "quad multiplexeur 2 vers 1", "TTL 74157 / 74HC157: quatre multiplexeurs 2 vers 1 partageant les lignes de selection."},
        {"74161", 16, "compteur 4 bits sync", "TTL 74161 / 74HC161: compteur binaire synchrone 4 bits avec chargement parallele."},
        {"74164", 14, "registre SIPO 8 bits", "TTL 74164 / 74HC164: registre a decalage serie vers parallele 8 bits."},
        {"74165", 16, "registre PISO 8 bits", "TTL 74165 / 74HC165: registre a decalage parallele vers serie 8 bits."},
        {"74193", 16, "compteur up/down 4 bits", "TTL 74193 / 74HC193: compteur binaire 4 bits up/down avec chargement parallele."},
        {"74245", 20, "transceiver bus 8 bits", "TTL 74245 / 74HC245: transceiver de bus octal trois etats avec direction."},
        {"74283", 16, "additionneur 4 bits", "TTL 74283 / 74HC283: additionneur binaire complet 4 bits avec retenue."},
        {"74373", 20, "latch octal", "TTL 74373 / 74HC373: latch transparent octal avec sorties trois etats."},
        {"74573", 20, "latch octal", "TTL 74573 / 74HC573: latch transparent octal avec brochage oriente bus."},
        {"74595", 16, "registre SIPO + latch", "TTL 74595 / 74HC595: registre a decalage 8 bits avec latch de sortie et sortie serie."},

        {"2716", 24, "EPROM 2K x 8", "Memoire EPROM 2716: 2048 octets de ROM parallele avec bus d'adresse et bus de donnees."},
        {"28C64", 28, "EEPROM 8K x 8", "Memoire EEPROM 28C64: 8192 octets de ROM programmable parallele."},
        {"28C256", 28, "EEPROM 32K x 8", "Memoire EEPROM 28C256: 32768 octets de ROM programmable parallele."},
        {"6264", 28, "SRAM 8K x 8", "Memoire SRAM 6264: 8192 octets de RAM statique parallele."},
        {"62256", 28, "SRAM 32K x 8", "Memoire SRAM 62256: 32768 octets de RAM statique parallele."},
        {"6502", 40, "CPU 6502", "Microprocesseur 6502: bus d'adresse 16 bits, bus de donnees 8 bits, horloge, reset et signaux de controle."},
        {"6522", 40, "VIA 6522", "Interface VIA 6522: deux ports paralleles 8 bits, timers, shift register et lignes de controle."},
        {"screen", 1, "ecran RGB 6 bits", "Composant virtuel d'ecran: consomme des signaux video et affiche une image dans BCAD."},

        {NULL, 14, "", ""}
      };
    return (infos);
  }

  static inline std::string             LowerString(const std::string &str)
  {
    std::string                         out = str;

    for (size_t i = 0; i < out.size(); ++i)
      out[i] = (char)std::tolower((unsigned char)out[i]);
    return (out);
  }

  static inline const ComponentInfo     &GetComponentInfo(const std::string &type)
  {
    const ComponentInfo                 *infos = ComponentInfoTable();

    for (size_t i = 0; infos[i].type != NULL; ++i)
      if (LowerString(type) == LowerString(infos[i].type))
        return (infos[i]);
    for (size_t i = 0; ; ++i)
      if (infos[i].type == NULL)
        return (infos[i]);
  }

  static inline bool                    StartsWith(const std::string &str,
                                                   const std::string &prefix)
  {
    return (str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix);
  }

  static inline bool                    IsDecimalName(const std::string &str)
  {
    if (str.empty())
      return (false);
    for (size_t i = 0; i < str.size(); ++i)
      if (str[i] < '0' || str[i] > '9')
        return (false);
    return (true);
  }

  static inline int                     ComponentPinGuess(const std::string &type)
  {
    const ComponentInfo                 &info = GetComponentInfo(type);
    std::string                         low = LowerString(type);

    if (info.type != NULL)
      return (info.pins);
    if (StartsWith(low, "74") || StartsWith(low, "40") || StartsWith(low, "45"))
      return (14);
    if (low.find("ram") != std::string::npos || low.find("rom") != std::string::npos)
      return (28);
    return (14);
  }

  static inline std::string             ComponentShortDescription(const std::string &type)
  {
    const ComponentInfo                 &info = GetComponentInfo(type);
    std::string                         low = LowerString(type);

    if (info.type != NULL)
      return (info.short_desc);
    if (StartsWith(low, "74hc") || StartsWith(low, "74hct"))
      return ("circuit logique " + type + " / famille 74HC");
    if (StartsWith(low, "74") && IsDecimalName(low))
      return ("circuit logique TTL " + type);
    if (StartsWith(low, "40") || StartsWith(low, "45"))
      return ("circuit CMOS serie 4000 " + type);
    if (low.find("ram") != std::string::npos || low.find("rom") != std::string::npos)
      return ("memoire parallele " + type);
    return ("type " + type + " disponible dans la bibliotheque");
  }

  static inline std::string             ComponentLongDescription(const std::string &type)
  {
    const ComponentInfo                 &info = GetComponentInfo(type);
    std::string                         low = LowerString(type);

    if (info.type != NULL)
      return (info.long_desc);
    if (StartsWith(low, "74hc") || StartsWith(low, "74hct"))
      return ("Composant de la famille 74HC/74HCT. Sa fiche detaillee n'est pas encore renseignee, mais la recherche peut le retrouver par reference, famille, logique et mots presents ici.");
    if (StartsWith(low, "74"))
      return ("Composant logique de la famille TTL 74xx. La fiche detaillee n'est pas encore renseignee dans BCAD pour cette reference.");
    if (StartsWith(low, "40") || StartsWith(low, "45"))
      return ("Composant CMOS de la serie 4000. La fiche detaillee n'est pas encore renseignee dans BCAD pour cette reference.");
    return ("Composant declare comme creatable par le circuit courant. Sa fiche detaillee n'a pas encore ete documentee dans la table du menu composant.");
  }

  static inline std::vector<std::string> SearchWords(const std::string &query)
  {
    std::vector<std::string>            words;
    std::istringstream                  iss(LowerString(query));
    std::string                         word;

    while (iss >> word)
      words.push_back(word);
    return (words);
  }

  static inline bool                    ComponentMatchesQuery(const std::string &type,
                                                              const std::string &query)
  {
    std::vector<std::string>            words = SearchWords(query);
    std::string                         haystack;

    if (words.empty())
      return (true);
    haystack = LowerString(type + " " + ComponentShortDescription(type) + " " +
                           ComponentLongDescription(type));
    for (size_t i = 0; i < words.size(); ++i)
      if (haystack.find(words[i]) == std::string::npos)
        return (false);
    return (true);
  }

  static inline int                     ComponentMatchCount(const Circuit &c,
                                                            const std::string &query)
  {
    const std::vector<std::string>      &types = c.GetCreatableTypes();
    int                                 count = 0;

    for (size_t i = 0; i < types.size(); ++i)
      if (ComponentMatchesQuery(types[i], query))
        count += 1;
    return (count);
  }

  static inline ComponentMenuGeometry   GetComponentMenuGeometry(const Screen &screen)
  {
    ComponentMenuGeometry               g;
    int                                 win_w = static_cast<int>(screen.pic->buffer.width);
    int                                 win_h = static_cast<int>(screen.pic->buffer.height);

    g.y = ShortcutBarHeight + 10;
    g.line = 24;
    g.w = std::max(360, win_w - 20);
    g.h = std::max(260, win_h - g.y - 20);
    g.x = (win_w - g.w) / 2;
    if (g.x < 0)
      g.x = 0;
    g.detail_w = std::min(360, std::max(260, g.w / 4));
    g.list_w = g.w - g.detail_w - 18;
    if (g.list_w < 360)
      {
        g.list_w = g.w;
        g.detail_w = 0;
      }
    g.search_y = g.y + 34;
    g.first_y = g.y + 82;
    g.rows = std::max(1, (g.h - 126) / g.line);
    return (g);
  }

  static inline bool                    IsInsideComponentMenu(const Screen &screen,
                                                              t_bunny_position pos)
  {
    ComponentMenuGeometry               g = GetComponentMenuGeometry(screen);

    return (screen.search_panel && pos.x >= g.x && pos.x <= g.x + g.w &&
            pos.y >= g.y && pos.y <= g.y + g.h);
  }

  static inline bool                    ComponentMenuChoiceAt(const Circuit &c,
                                                              const Screen &screen,
                                                              t_bunny_position pos,
                                                              std::string &type)
  {
    ComponentMenuGeometry               g = GetComponentMenuGeometry(screen);
    const std::vector<std::string>      &types = c.GetCreatableTypes();
    int                                 rel;
    int                                 wanted;
    int                                 seen = 0;

    type = "";
    if (!IsInsideComponentMenu(screen, pos))
      return (false);
    if (pos.x < g.x + 8 || pos.x >= g.x + g.list_w - 8)
      return (false);
    if (pos.y < g.first_y || pos.y >= g.first_y + g.rows * g.line)
      return (false);
    rel = pos.y - g.first_y;
    wanted = rel / g.line;
    for (size_t i = 0; i < types.size(); ++i)
      {
        if (!ComponentMatchesQuery(types[i], screen.search_query))
          continue ;
        if (seen++ < screen.search_offset)
          continue ;
        if (wanted-- == 0)
          {
            type = types[i];
            return (true);
          }
      }
    return (false);
  }
}

#endif
