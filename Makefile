#
# Makefile für csmapfx
# (c) 2004-2010 by Thomas J. Gritzan <phygon@gmx.de>
#

INSTPATH = ~/Desktop

# Standart-Variablen
CPP = g++
CPPFLAGS=-DBOOST_SIGNALS_NO_DEPRECATION_WARNING -w
GPP = $(CPP) $(CPPFLAGS)
EXEC_STATIC = csmapfx
EXEC_DYNAMIC = csmapfx-dyn
REVISION = 133
ARCHIV = csmapfx-rev$(REVISION).tar.bz2
COMPILE = -O6 -finput-charset=ISO8859-15 -fexec-charset=ISO8859-15 -DNDEBUG -DBOOST_USER_CONFIG="<$(PWD)/boost/nodynlibs.hpp>"  -Wall -ffast-math -fstrict-aliasing -finline-functions -fomit-frame-pointer -fexpensive-optimizations -I/usr/include/fox-1.6 -I/usr/include/ruby-2.7.0 -I/usr/include/x86_64-linux-gnu/ruby-2.7.0
LINK_STATIC = -O6 -Wall -s /usr/lib/x86_64-linux-gnu/libFOX-1.6.a -L/usr/lib/x86_64-linux-gnu -L/usr/X11R6/lib -lXext -lX11 -lz -lbz2 -lpng -lm -lpthread -lrt -ljpeg
LINK_STATIC_SOME = -lXcursor
LINK_DYNAMIC = -O6 -s -lFOX-1.6 -lruby-2.7 -Wl,-rpath=.
OBJDIR = ./bin
HEADER = $(wildcard *.h)
SOURCES = $(wildcard *.cpp)
SOURCES += $(wildcard boost/signals/*.cpp)
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# Farben für vterm
COL = $(firstword $(COLUMNS) 80)
COL := $(shell expr $(COL) - 10)
COLORRED = echo -ne "\033[31m"
COLORGREEN = echo -ne "\033[32m"
COLORNORM = echo -ne "\033[0m"
MSGERR = (echo -e "\033[A\r\033[$(COL)C\033[31merror\033[0m\033[D\r"; false)
MSGDONE = echo -e "\033[A\r\033[$(COL)C\033[32mdone\033[0m\033[D\r"
MSGTEXT = echo -e "\033[A\r\033[$(COL)C\033[32mTEXT\033[0m\033[D\r"
# using: @$(subst TEXT,<insert text>,$(MSGTEXT))

# Build-Ziele
default: dynamic
all: static dynamic

static: $(EXEC_STATIC)
dynamic: $(EXEC_DYNAMIC)

# Builds
clean:
	@$(COLORRED); echo Clean... ;$(COLORNORM)
	-rm $(ARCHIV) $(EXEC_STATIC) $(EXEC_DYNAMIC) $(OBJECTS)
	-rmdir $(OBJDIR)
	make clean -C expressions
	@$(MSGDONE)

install: $(EXEC_STATIC)
	@echo "Installing to $(INSTPATH)... "
	@cp $(EXEC_STATIC) $(INSTPATH)
	@$(MSGDONE)

archiv: $(EXEC_STATIC)
	@echo "Building (static) archiv... "
	@tar -cjf $(ARCHIV) $(EXEC_STATIC) || $(MSGERR)
	@$(MSGDONE)

version.h: version.tmpl.h history.txt
	@echo "Generate version information... "
	@./svnrev.sh version.tmpl.h > version.h || $(MSGERR)
	@$(subst TEXT,rev $(REVISION),$(MSGTEXT))

$(EXEC_STATIC): $(OBJDIR) $(OBJECTS)
	@echo "Linking static executable... "
	@$(GPP) -o $@ $(OBJECTS) $(LINK_STATIC) || $(MSGERR)
	@$(MSGDONE)

$(EXEC_DYNAMIC): $(OBJDIR) $(OBJECTS)
	@echo "Linking dynamic executable... "
	@$(GPP) -o $@ $(OBJECTS) $(LINK_DYNAMIC) || $(MSGERR)
	@$(MSGDONE)

$(OBJDIR)/%.o: %.cpp $(HEADER) version.h
	@echo "Compiling: $<... "
	@$(GPP) -c -o $@ $< $(COMPILE) || $(MSGERR)
	@$(MSGDONE)

$(OBJDIR):
	mkdir -p $@

# <eof>
