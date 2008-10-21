all: libcanvas.a include

ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC)
endif

include $(DEVKITPPC)/wii_rules

include ../../userpaths.mk

INCLUDE := -isystem $(boostpath) -isystem $(LIBOGC_INC) \
	-isystem $(wiispritepath)/include -isystem $(wiispritepath)/../libpng/include

warnings = -Wchar-subscripts -Wcomment -Wformat -Wformat-security \
 -Wmissing-braces -Wparentheses -Wsequence-point -Wreturn-type -Wunused-value \
 -Wswitch -Wtrigraphs -Wunused-function -Wunused-variable -Wunused-parameter \
 -Wunused-label -Wfloat-equal -Wshadow \
 -Wlarger-than-10000 -Wcast-qual -Wcast-align -Wsign-compare \
 -Wredundant-decls #-Wuninitialized
 
cwarnings = $(warnings) -Wnonnull -Wimplicit-int -Werror-implicit-function-declaration -Wmain \
	 -Wtraditional  -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	 -Wnested-externs
	 
cppwarnings = $(warnings) -Woverloaded-virtual -Wctor-dtor-privacy

optimizations = -O0 -fno-guess-branch-probability -ffast-math

COMMONCOMPILEFLAGS = -g -mrvl $(optimizations) $(MACHDEP) $(INCLUDE) -Werror -fexceptions -ftabstop=2

CFLAGS	= $(COMMONCOMPILEFLAGS) $(cwarnings)
CXXFLAGS	= $(COMMONCOMPILEFLAGS) $(cppwarnings) -ffor-scope
CPPFLAGS = $(INCLUDE)

LDFLAGS	=	-g $(MACHDEP) -mrvl -Wl,-Map,$(patsubst %.elf,%.map,$(notdir $@))

LIBS	:=	-lwiisprite -lpng -lz -lfat -lwiiuse -lbte -logc -lm

LIBPATHS	:=-L $(LIBOGC_LIB) -L $(wiispritepath)/lib -L $(wiispritepath)/../libpng/lib
					
.LIBPATTERNS = lib%.so lib%.a
VPATH	:=	LIBPATHS

DEPSDIR = deps


CFILES		:=	$(wildcard *.c)
CPPFILES	:=	$(wildcard *.cpp)
sFILES		:=	$(wildcard *.s)
SFILES		:=	$(wildcard *.S)

export OFILES	:=	$(patsubst %.cpp,%.o,$(CPPFILES)) \
 $(patsubst %.s,%.o,$(sFILES)) \
 $(patsubst %.c,%.o,$(CFILES)) \
 $(patsubst %.S,%.o,$(SFILES))
					
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

libcanvas.a : $(OFILES)

.PHONY: all clean rebuild include

clean :
	$(RM) $(wildcard *.elf) $(wildcard *.dol) $(wildcard *.o) $(wildcard *.a)
	
rebuild : clean all

DEPENDS	:=	$(OFILES:.o=.d)
-include $(foreach depend,$(DEPENDS),$(DEPSDIR)/$(depend))

.DELETE_ON_ERROR : $(OFILES) *.elf

include
	@[ -d $@ ] || mkdir -p $@
	cp *.h include/