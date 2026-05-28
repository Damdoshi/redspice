## -----------------------------------------------------------------------------
## --Jason Brillante "Damdoshi" 2015------- ---   -   --- ---- --  --   - --- --
## ---------------------------------------- ---- --- - -- --- - - - -- --  -- --
## ---------------------------------------- ---- --- - -- --- - - - -- --  -- --
## -----|\-//------------------------------ ---- ---   -- --- - -  --- -- - - --
## -----||//___---------------------------- ---- --- - -- ---   - ---- -- - - --
## ----O  /    \--------------------------- ---- --- - -- --- - - ---- -- --  --
## ---<________/---------------------------   -   -   ---   - - - ---   - --- --
## -----------------------------------------------------------------------------

BINNAME		=	bcad
NAME		=	lib$(BINNAME).a
ERRLOG		=	errors~

CC		=	b++
CFLAGS		=	-W -Wall -fPIC -std=gnu++23
CFLAGS		+=	-Wno-missing-field-initializers
CFLAGS		+=	-g -g3 -ggdb
#CFLAGS		+=	-O2 -ffast-math -march=native

LFLAGS		=

## Define some stuff
ECHO		=	/bin/echo -e
DEFAULT		=	"\033[00m"
GREEN		=	"\033[0;32m"
TEAL		=	"\033[1;36m"
RED		=	"\033[0;31m"

## Source
INC		=	-I./include/ -I./include/stdcmp -I./include/vircmp
SRC		=	$(shell find . -name "*.cpp" | sort)
OBJ		=	$(SRC:.cpp=.o)

all:		erase $(BINNAME)
erase:
		@rm -f $(ERRLOG)
clean:		erase
		@rm -f $(OBJ)
fclean:		clean
		@rm -f $(NAME)
		@rm -f $(BINNAME)
re:		fclean all

## Compile ---------------------------------------------------------------------
$(NAME):	$(OBJ)
		@ar r $(NAME) $(OBJ) 2>> $(ERRLOG) && \
		 $(ECHO) $(GREEN) "[AR]" $(TEAL) $(NAME) $(DEFAULT) || \
		 $(ECHO) $(RED) "[XX]" $(TEAL) $(NAME)
		@ranlib $(NAME) && \
		 $(ECHO) $(GREEN) "[OK]" $(TEAL) $(NAME) $(DEFAULT) || \
		 $(ECHO) $(RED) "[XX]" $(TEAL) $(NAME)
$(BINNAME):	$(NAME) main.cpp
		@$(CC) main.cpp -o $(BINNAME) $(INC) -L./ $(NAME) $(CFLAGS) $(LFLAGS)
.cpp.o:
		@$(CC) $(INC) $(CFLAGS) -c $< -o $@ 2>> $(ERRLOG) && \
		 $(ECHO) $(GREEN) "[OK]" $< $(DEFAULT) || \
		 $(ECHO) $(RED) "[XX]" $< $(DEFAULT)

