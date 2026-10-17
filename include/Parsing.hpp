// Jason Brillante "Damdoshi"
// Hanged Bunny Studio 2014-2018
//
// RED Spice

#ifndef			__PARSING_HPP__
# define		__PARSING_HPP__
# include		<string>
# include                <cstddef>

bool ReadNumber(const std::string &code, int &i, double &value);
bool ReadUnsigned(const std::string &code, int &i, size_t &value);
bool ValidIdentifier(const std::string &name);

bool			ReadWhitespace(const std::string		&code,
				       int				&i);
bool			ReadText(const std::string			&code,
				 int					&i,
				 const std::string			&token);
bool			ReadChar(const std::string			&code,
				 int					&i);
bool			CheckChar(const std::string			&code,
				  int					&i);

#endif	//		__PARSING_HPP__
