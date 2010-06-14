BINS = segget
OBJS = $(addsuffix .o,$(BINS))
PKGCONFIG_MODULES = json libcurl
MISSING_PLUGINS_LIBS = 
LIBS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))
CXXFLAGS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))

CXXFLAGS_WARNINGS = -pedantic -Wall -Wextra -Wformat -Weffc++
#CXXFLAGS_WARNINGS = -pedantic -Wextra -Wformat -Weffc++
CXXFLAGS_OPT = -O2 -g

LIBS = $(LIBS_PKGCONFIG)
#MIBS = -lncurses
CXXFLAGS = $(CXXFLAGS_PKGCONFIG) -lncurses $(CXXFLAGS_WARNINGS) $(CXXFLAGS_OPT)

all: clean $(BINS)
	
%.o: %.cxx
	$(CXX) -c -o $@ $(CXXFLAGS) $^
%: %.o
	$(CXX) -o $@ $(LIBS) $(CIBS) $^

clean:
	-rm -f $(BINS) $(OBJS)
	-rm -f debug.txt

.PRECIOUS: %.o
