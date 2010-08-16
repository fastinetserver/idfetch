BINS = tuiclient
OBJS = $(addsuffix .o,$(BINS))
PKGCONFIG_MODULES =
MISSING_PLUGINS_LIBS = 
LIBS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))
CXXFLAGS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))

CXXFLAGS_WARNINGS = -pedantic -Wall -Wextra -Wformat -Weffc++
CXXFLAGS_OPT = -O2 -g

LIBS = $(LIBS_PKGCONFIG)
CXXFLAGS = $(CXXFLAGS_PKGCONFIG) -lncurses -lpthread $(CXXFLAGS_WARNINGS) $(CXXFLAGS_OPT)

all: clean $(BINS)
	
%.o: %.cxx
	$(CXX) -c -o $@ $(CXXFLAGS) $^

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
