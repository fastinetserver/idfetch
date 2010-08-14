BINS = seggetd
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

seggetd: seggetd.o connection.o checksum.o config.o distfile.o log.o mirror.o network.o networkbroker.o phase.o pkg.o response.o segment.o settings.o stats.o str.o tui.o utils.o ui_server.o proxyfetcher.o requestserver.o scriptserver.o
	$(CXX) -o $@ -lncurses -lcryptopp $(LIBS) $(CIBS) $^
#%: %.o
#	$(CXX) -o $@ $(LIBS) $(CIBS) $^

clean:
	-rm -f $(BINS) $(OBJS)
	-rm -f *.o
	-rm -f *.log
	
install:
	-mkdir /etc/seggetd
	-cp *.conf /etc/seggetd
	-cp seggetd /usr/bin/seggetd
	-cp ./init.d/seggetd /etc/init.d/seggetd
	-mkdir -p /usr/share/seggetd
	-cp -R ./webui /usr/share/seggetd
	-mkdir -p /usr/share/seggetd/scripts
	-cp -R ./scripts /usr/share/seggetd
	-mkdir -p /var/tmp/seggetd/segments
	-mkdir -p /usr/portage/distfiles
	-mkdir -p /var/log/seggetd

.PRECIOUS: %.o