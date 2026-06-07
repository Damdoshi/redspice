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
#CFLAGS		+=	-g -g3 -ggdb
CFLAGS		+=	-O2 -ffast-math -march=native

LFLAGS		=	-ldl

## Define some stuff
ECHO		=	/bin/echo -e
DEFAULT		=	"\033[00m"
GREEN		=	"\033[0;32m"
TEAL		=	"\033[1;36m"
RED		=	"\033[0;31m"

## Source
INC		=	-I./include/ -I./include/stdcmp -I./include/vircmp
SRC		=	$(shell find . -name "*.cpp" ! -name "main.cpp" | sort)
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

## Package ---------------------------------------------------------------------
PREFIX		?=	/usr
DESTDIR		?=
PACKAGE_NAME	?=	$(BINNAME)
PACKAGE_VERSION	?=	1.0+$(shell date +%s)
PACKAGE_ARCH	?=	$(shell dpkg --print-architecture 2>/dev/null || echo amd64)
PACKAGE_ROOT	=	.pkg/$(PACKAGE_NAME)
PACKAGE_FILE	=	$(PACKAGE_NAME)_$(PACKAGE_VERSION)_$(PACKAGE_ARCH).deb

.PHONY: install package package-clean

install: all
		@install -d $(DESTDIR)$(PREFIX)/bin
		@install -m 0755 $(BINNAME) $(DESTDIR)$(PREFIX)/bin/$(BINNAME)
		@if [ -d res ]; then \
			install -d $(DESTDIR)$(PREFIX)/share/$(BINNAME); \
			cp -a res $(DESTDIR)$(PREFIX)/share/$(BINNAME)/; \
		fi

package: all
		@test -x $(BINNAME) || { echo "$(BINNAME) was not built"; exit 1; }
		@rm -rf $(PACKAGE_ROOT)
		@install -d $(PACKAGE_ROOT)/DEBIAN
		@install -d $(PACKAGE_ROOT)$(PREFIX)/bin
		@install -m 0755 $(BINNAME) $(PACKAGE_ROOT)$(PREFIX)/bin/$(BINNAME)
		@if [ -d res ]; then \
			install -d $(PACKAGE_ROOT)$(PREFIX)/share/$(BINNAME); \
			cp -a res $(PACKAGE_ROOT)$(PREFIX)/share/$(BINNAME)/; \
		fi
		@printf '%s\n' \
			'Package: $(PACKAGE_NAME)' \
			'Version: $(PACKAGE_VERSION)' \
			'Section: electronics' \
			'Priority: optional' \
			'Architecture: $(PACKAGE_ARCH)' \
			'Depends: liblapin-dev' \
			'Maintainer: EFRITS <contact@efrits.fr>' \
			'Description: BunnyCAD electronic circuit editor and simulator' \
			' BunnyCAD is a simple electronic circuit editor and simulator.' \
			> $(PACKAGE_ROOT)/DEBIAN/control
		@dpkg-deb --root-owner-group --build $(PACKAGE_ROOT) $(PACKAGE_FILE)
		@$(ECHO) $(GREEN) "[DEB]" $(TEAL) $(PACKAGE_FILE) $(DEFAULT)

package-clean:
		@rm -rf .pkg
		@rm -f $(PACKAGE_NAME)_*.deb
