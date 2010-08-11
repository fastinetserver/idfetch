BINS = segget
OBJS = $(addsuffix .o,$(BINS))
PKGCONFIG_MODULES = json libcurl libcrypto
MISSING_PLUGINS_LIBS = 
LIBS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))
CXXFLAGS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))

CXXFLAGS_WARNINGS = -pedantic -Wall -Wextra -Wformat -Weffc++
#CXXFLAGS_WARNINGS = -pedantic -Wextra -Wformat -Weffc++
CXXFLAGS_OPT = -O2 -g

LIBS = $(LIBS_PKGCONFIG)
#MIBS = -lncurses
CXXFLAGS = $(CXXFLAGS_PKGCONFIG) -lncurses $(CXXFLAGS_WARNINGS) $(CXXFLAGS_OPT)

all: $(BINS)
	
%.o: %.cxx
	$(CXX) -c -o $@ $(CXXFLAGS) $^

segget: segget.o connection.o checksum.o config.o distfile.o log.o mirror.o network.o networkbroker.o phase.o pkg.o response.o segment.o settings.o stats.o str.o tui.o utils.o ui_server.o proxyfetcher.o requestserver.o scriptserver.o
	$(CXX) -o $@ -lncurses $(LIBS) $(CIBS) $^
#%: %.o
#	$(CXX) -o $@ $(LIBS) $(CIBS) $^

clean:
	-rm -f $(BINS) $(OBJS)
	-rm -f *.o
	-rm -f *.log

.PRECIOUS: %.o
