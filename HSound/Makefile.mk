all: sound.dol

ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC)
endif

include $(DEVKITPPC)/wii_rules

include ../../userpaths.mk

INCLUDE := -isystem $(boostpath) -isystem $(LIBOGC_INC) \
	-isystem $(wiispritepath)/include -isystem $(wiispritepath)/../libpng/include \
	-isystem $(rabbitcodepath)/include -I ../../canvas/canvas

warnings = -Wchar-subscripts -Wcomment -Wformat -Wformat-security \
 -Wmissing-braces -Wparentheses -Wsequence-point -Wreturn-type -Wunused-value \
 -Wswitch -Wtrigraphs -Wunused-function -Wunused-variable -Wunused-parameter \
 -Wunused-label -Wfloat-equal -Wshadow \
 -Wcast-qual -Wcast-align -Wsign-compare \
 -Wredundant-decls #-Wuninitialized
 
cwarnings = $(warnings) -Wnonnull -Wimplicit-int -Werror-implicit-function-declaration -Wmain \
	 -Wtraditional  -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	 -Wnested-externs
	 
cppwarnings = $(warnings) -Woverloaded-virtual -Wctor-dtor-privacy

optimizations = -O0 -fno-guess-branch-probability -ffast-math

COMMONCOMPILEFLAGS = -g -mrvl $(optimizations) $(MACHDEP) $(INCLUDE) -Werror -fexceptions -ftabstop=2 -fno-inline -fno-peephole

CFLAGS	= $(COMMONCOMPILEFLAGS) $(cwarnings)
CXXFLAGS	= $(COMMONCOMPILEFLAGS) $(cppwarnings) -ffor-scope
CPPFLAGS = $(INCLUDE)

LDFLAGS	=	-g $(MACHDEP) -mrvl -Wl,-Map,$(patsubst %.elf,%.map,$(notdir $@))

LIBS	:=	-ldb -lcanvas -lwiisprite -lpng -lz -lfat -lwiiuse -lbte -logc -lm -lmad -lsamplerate

LIBPATHS	:=-L $(LIBOGC_LIB) -L $(wiispritepath)/lib -L $(wiispritepath)/../libpng/lib \
 -L ../../canvas/canvas -L $(rabbitcodepath)
					
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
 $(patsubst %.S,%.o,$(SFILES)) \
 embeded.o
					
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

sound.elf : $(OFILES)
	@echo linking...
	@$(LD) $^ $(LDFLAGS) $(LIBPATHS) $(LIBS) -o $@
	
embeded.o : embeded.mp3
	@echo embeded.mp3
	@$(bin2o)

.PHONY: all clean rebuild

clean :
	$(RM) $(wildcard *.elf) $(wildcard *.dol) $(wildcard *.o)
	
rebuild : clean all

DEPENDS	:=	$(OFILES:.o=.d)
-include $(foreach depend,$(DEPENDS),$(DEPSDIR)/$(depend))

.DELETE_ON_ERROR : $(OFILES) *.elf
