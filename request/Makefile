BINS = request
OBJS = $(addsuffix .o,$(BINS))
PKGCONFIG_MODULES = json
MISSING_PLUGINS_LIBS = 
LIBS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))
CXXFLAGS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))

CXXFLAGS_WARNINGS = -pedantic -Wall -Wextra -Wformat -Weffc++
#CXXFLAGS_WARNINGS = -pedantic -Wextra -Wformat -Weffc++
CXXFLAGS_OPT = -O0 -g3

LIBS = $(LIBS_PKGCONFIG)
CXXFLAGS = $(CXXFLAGS_PKGCONFIG) $(CXXFLAGS_WARNINGS) $(CXXFLAGS_OPT)

all: $(BINS)

%.o: %.cxx
	$(CXX) -c -o $@ $(CXXFLAGS) $^

request: request.o config.o phase.o response.o settings.o str.o utils.o
	$(CXX) -o $@ $(LIBS) $(CIBS) $^

clean:
	-rm -f $(BINS)
	-rm -f $(OBJS)
	-rm -f *~
	-rm -f *.o

.PRECIOUS: %.o
