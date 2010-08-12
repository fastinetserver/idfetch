BINS = tuiclient
OBJS = $(addsuffix .o,$(BINS))
PKGCONFIG_MODULES =
MISSING_PLUGINS_LIBS = 
LIBS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))
CXXFLAGS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))

CXXFLAGS_WARNINGS = -pedantic -Wall -Wextra -Wformat -Weffc++
#CXXFLAGS_WARNINGS = -pedantic -Wextra -Wformat -Weffc++
CXXFLAGS_OPT = -O2 -g

LIBS = $(LIBS_PKGCONFIG)
#CIBS = -lncurses
CXXFLAGS = $(CXXFLAGS_PKGCONFIG) -lncurses -lpthread $(CXXFLAGS_WARNINGS) $(CXXFLAGS_OPT)

all: clean $(BINS)
	
#log.o: log.cpp settings.h log.h
#segget.o: segget.cpp pkg.cpp distfile.cpp segment.cpp log.o stats.cpp connection.cpp tui.cpp settings.o mirror.cpp str.cpp checksum.cpp

#mirror.o: mirror.cpp tui.h
#	$(CXX) -c -o $@ mirror.cpp
#	
#$(CXXFLAGS) $^
#segget: segget.cpp log.o mirror.o distfile.o tui.o
#pkg.o distfile.o segment.o mirror.o connection.o settings.o stats.o tui.o log.o
#pkg.o: pkg.cpp pkg.h distfile.h  segment.h log.h stats.h connection.h tui.h settings.h config.h str.h mirror.h checksum.h \
#                     distfile.o  segment.o log.o stats.o connection.o tui.o settings.o config.o str.o mirror.o checksum.o

#distfile.o: distfile.cpp tui.o
#distfile.o: distfile.cpp checksum.o
#distfile.h segment.h log.h stats.h connection.h tui.h settings.h config.h str.h mirror.h checksum.h
#	$(CXX) -c -o $@ $(CXXFLAGS) $^
 
#segment.o: segment.cpp segment.h log.h stats.h connection.h tui.h settings.h config.h str.h

#segget.o: segget.cpp settings.o pkg.h distfile.h segment.h log.h stats.h connection.h tui.h settings.h config.h str.h mirror.h checksum.h
#                                                                                 
%.o: %.cxx
	$(CXX) -c -o $@ $(CXXFLAGS) $^
#%: %.o
#	$(CXX) -o $@ $(LIBS) $(CIBS) $^

tuiclient: tuiclient.o config.o colors.o distfilewindow.o helpwindow.o log.o mainwindow.o tuidistfile.o twindow.o scrollwindow.o settings.o str.o
	$(CXX) -o $@ -lncurses -lpthread $(LIBS) $(CIBS) $^

make clean_log:
	-rm -f ./logs/*.log

clean:
	-rm -f $(BINS) $(OBJS)
	-rm -f ./logs/*.log *.o

install:
	-mkdir /etc/seggetd
	-cp tuiclient.conf /etc/seggetd
	-cp tuiclient /usr/bin

.PRECIOUS: %.o
