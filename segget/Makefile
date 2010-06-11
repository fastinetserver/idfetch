BINS = segget
OBJS = $(addsuffix .o,$(BINS))
PKGCONFIG_MODULES = json libcurl
LIBS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))
CXXFLAGS_PKGCONFIG := $(foreach mod,$(PKGCONFIG_MODULES),$(shell pkg-config --libs $(mod)))

CXXFLAGS_WARNINGS = -pedantic -Wall -Wextra -Wformat -Weffc++
#CXXFLAGS_WARNINGS = -pedantic -Wextra -Wformat -Weffc++
CXXFLAGS_OPT = -O2 -g

LIBS = $(LIBS_PKGCONFIG)
CXXFLAGS = $(CXXFLAGS_PKGCONFIG) $(CXXFLAGS_WARNINGS) $(CXXFLAGS_OPT)

all: $(BINS)
	
%.o: %.cxx
	$(CXX) -c -o $@ $(CXXFLAGS) $^
%: %.o
	$(CXX) -o $@ $(LIBS) $^

clean:
	-rm -f $(BINS) $(OBJS)
	-rm -f *.seg*

.PRECIOUS: %.o
